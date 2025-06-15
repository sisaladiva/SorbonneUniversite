#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <getopt.h>
#include <err.h>
#include <omp.h>

typedef uint64_t u64;
typedef uint32_t u32;

#define CHUNK_SIZE 1024
#define NUM_THREADS 8
#define BUFFER_SIZE 1024

struct __attribute__ ((packed)) entry { 
    u32 k; 
    u64 v; 
};

// Structure pour le buffer circulaire
struct circular_buffer {
    struct entry *entries;
    size_t size;
    size_t head;
    size_t tail;
    size_t count;
};

// Variables globales
u64 n = 0;
u64 mask;
u64 dict_size;
struct circular_buffer *buffers;
int num_buffers;

u32 P[2][2] = {{0, 0}, {0xffffffff, 0xffffffff}};
u32 C[2][2];

// Fonctions du buffer circulaire
struct circular_buffer* buffer_create(size_t size) {
    struct circular_buffer *buffer = malloc(sizeof(struct circular_buffer));
    buffer->entries = malloc(sizeof(struct entry) * size);
    buffer->size = size;
    buffer->head = buffer->tail = buffer->count = 0;
    return buffer;
}

void buffer_free(struct circular_buffer *buffer) {
    free(buffer->entries);
    free(buffer);
}

bool buffer_push(struct circular_buffer *buffer, struct entry item) {
    if (buffer->count == buffer->size) return false;
    
    buffer->entries[buffer->head] = item;
    buffer->head = (buffer->head + 1) % buffer->size;
    buffer->count++;
    return true;
}

bool buffer_pop(struct circular_buffer *buffer, struct entry *item) {
    if (buffer->count == 0) return false;
    
    *item = buffer->entries[buffer->tail];
    buffer->tail = (buffer->tail + 1) % buffer->size;
    buffer->count--;
    return true;
}

// Fonctions SPECK (inchangées)
#define ROTL32(x,r) (((x)<<(r)) | (x>>(32-(r))))
#define ROTR32(x,r) (((x)>>(r)) | ((x)<<(32-(r))))
#define ER32(x,y,k) (x=ROTR32(x,8), x+=y, x^=k, y=ROTL32(y,3), y^=x)
#define DR32(x,y,k) (y^=x, y=ROTR32(y,3), x^=k, x-=y, x=ROTL32(x,8))

void Speck64128KeySchedule(const u32 K[],u32 rk[]) {
    u32 i,D=K[3],C=K[2],B=K[1],A=K[0];
    for(i=0;i<27;) {
        rk[i]=A; ER32(B,A,i++);
        rk[i]=A; ER32(C,A,i++);
        rk[i]=A; ER32(D,A,i++);
    }
}

void Speck64128Encrypt(const u32 Pt[], u32 Ct[], const u32 rk[]) {
    u32 i;
    Ct[0]=Pt[0]; Ct[1]=Pt[1];
    for(i=0;i<27;)
        ER32(Ct[1],Ct[0],rk[i++]);
}

void Speck64128Decrypt(u32 Pt[], const u32 Ct[], u32 const rk[]) {
    int i;
    Pt[0]=Ct[0]; Pt[1]=Ct[1];
    for(i=26;i>=0;)
        DR32(Pt[1],Pt[0],rk[i--]);
}

// Fonction de hachage optimisée
u64 murmur64(u64 x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return x;
}

// Nouvelles fonctions optimisées pour le dictionnaire
void dict_setup() {
    num_buffers = omp_get_max_threads();
    buffers = malloc(sizeof(struct circular_buffer*) * num_buffers);
    
    #pragma omp parallel for
    for(int i = 0; i < num_buffers; i++) {
        buffers[i] = buffer_create(BUFFER_SIZE);
    }
}

void dict_cleanup() {
    for(int i = 0; i < num_buffers; i++) {
        buffer_free(buffers[i]);
    }
    free(buffers);
}

// Fonctions f et g optimisées
u64 f(u64 k) {
    u32 K[4] = {k & 0xffffffff, k >> 32, 0, 0};
    u32 rk[27];
    u32 Ct[2];
    
    Speck64128KeySchedule(K, rk);
    Speck64128Encrypt(P[0], Ct, rk);
    
    return ((u64)Ct[0] ^ ((u64)Ct[1] << 32)) & mask;
}

u64 g(u64 k) {
    u32 K[4] = {k & 0xffffffff, k >> 32, 0, 0};
    u32 rk[27];
    u32 Pt[2];
    
    Speck64128KeySchedule(K, rk);
    Speck64128Decrypt(Pt, C[0], rk);
    
    return ((u64)Pt[0] ^ ((u64)Pt[1] << 32)) & mask;
}

bool is_good_pair(u64 k1, u64 k2) {
    u32 Ka[4] = {k1 & 0xffffffff, k1 >> 32, 0, 0};
    u32 Kb[4] = {k2 & 0xffffffff, k2 >> 32, 0, 0};
    u32 rka[27], rkb[27], mid[2], Ct[2];
    
    Speck64128KeySchedule(Ka, rka);
    Speck64128KeySchedule(Kb, rkb);
    
    Speck64128Encrypt(P[1], mid, rka);
    Speck64128Encrypt(mid, Ct, rkb);
    
    return (Ct[0] == C[1][0]) && (Ct[1] == C[1][1]);
}

// Fonction de recherche parallélisée
int golden_claw_search(int maxres, u64 k1[], u64 k2[]) {
    double start = omp_get_wtime();
    int nres = 0;
    u64 N = 1ull << n;
    
    // Phase 1: Remplissage parallèle du dictionnaire
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE)
    for (u64 x = 0; x < N; x++) {
        int thread_id = omp_get_thread_num();
        struct entry e = {.k = x % PRIME, .v = x};
        buffer_push(buffers[thread_id], e);
    }
    
    double mid = omp_get_wtime();
    printf("Fill: %.1fs\n", mid - start);
    
    // Phase 2: Recherche parallèle
    #pragma omp parallel
    {
        u64 local_candidates = 0;
        #pragma omp for schedule(dynamic, CHUNK_SIZE)
        for (u64 z = 0; z < N; z++) {
            u64 y = g(z);
            
            // Vérification dans tous les buffers
            for (int b = 0; b < num_buffers; b++) {
                struct entry e;
                while (buffer_pop(buffers[b], &e)) {
                    if (e.k == y % PRIME && is_good_pair(e.v, z)) {
                        #pragma omp critical
                        {
                            if (nres < maxres) {
                                k1[nres] = e.v;
                                k2[nres] = z;
                                nres++;
                                printf("SOLUTION FOUND!\n");
                            }
                        }
                    }
                }
            }
        }
    }
    
    printf("Probe: %.1fs\n", omp_get_wtime() - mid);
    return nres;
}

// Main et fonctions de traitement des options de ligne de commande inchangées
void usage(char **argv) {
    printf("%s [OPTIONS]\n\n", argv[0]);
    printf("Options:\n");
    printf("--n N                       block size [default 24]\n");
    printf("--C0 N                      1st ciphertext (in hex)\n");
    printf("--C1 N                      2nd ciphertext (in hex)\n");
    printf("\n");
    printf("All arguments are required\n");
    exit(0);
}

void process_command_line_options(int argc, char **argv) {
    struct option longopts[4] = {
        {"n", required_argument, NULL, 'n'},
        {"C0", required_argument, NULL, '0'},
        {"C1", required_argument, NULL, '1'},
        {NULL, 0, NULL, 0}
    };
    char ch;
    int set = 0;
    while ((ch = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
        switch (ch) {
            case 'n':
                n = atoi(optarg);
                mask = (1ull << n) - 1;
                break;
            case '0':
                set |= 1;
                u64 c0 = strtoull(optarg, NULL, 16);
                C[0][0] = c0 & 0xffffffff;
                C[0][1] = c0 >> 32;
                break;
            case '1':
                set |= 2;
                u64 c1 = strtoull(optarg, NULL, 16);
                C[1][0] = c1 & 0xffffffff;
                C[1][1] = c1 >> 32;
                break;
            default:
                errx(1, "Unknown option\n");
        }
    }
    if (n == 0 || set != 3) {
        usage(argv);
        exit(1);
    }
}

int main(int argc, char **argv) {
    process_command_line_options(argc, argv);
    printf("Running with n=%d, C0=(%08x, %08x) and C1=(%08x, %08x)\n", 
           (int)n, C[0][0], C[0][1], C[1][0], C[1][1]);
    
    // Initialize OpenMP
    omp_set_num_threads(NUM_THREADS);
    
    // Setup dictionary
    dict_setup();
    
    // Search
    u64 k1[16], k2[16];
    int nkey = golden_claw_search(16, k1, k2);
    
    // Cleanup
    dict_cleanup();
    
    // Validation
    if (nkey > 0) {
        for (int i = 0; i < nkey; i++) {
            if (is_good_pair(k1[i], k2[i])) {
                printf("Solution found: (%" PRIx64 ", %" PRIx64 ") [checked OK]\n", k1[i], k2[i]);
            }
        }
    }
    
    return 0;
}
















int golden_claw_search_mpi(int maxres, u64 k1[], u64 k2[], int rank, int num_processes)
{
    double start = wtime();
    u64 N = 1ull << n;
    u64 local_N = N / num_processes;
    u64 start_index = rank * local_N;
    u64 end_index = (rank == num_processes - 1) ? N : (rank + 1) * local_N;

    // Chaque processus initialise une table de hachage de taille dict_size / num_processes
    u64 local_dict_size = (1.125 * N ) / num_processes;
    u64 taillenouveaubloc = (1.125 * N ) / (8*num_processes);

    dict_setup(local_dict_size);  // Utilise une table plus petite pour chaque processus

printf(" La taille du dictionnaire local est %d\n", local_dict_size);

    // Chaque processus remplit sa propre table de hachage
    #pragma omp parallel for
    for (u64 x = 0; x < N; x++) {
        
    int number_bits= log2(num_processes);
    u64 mask = (1ULL << number_bits) - 1;
    // On prend les 2 derniers bits pour déterminer le processus cible
    int target_process = (x & mask);  // Les 2 derniers bits de la clé

        // Envoi des données uniquement au processus cible basé sur les bits
        if (target_process == rank) {
          
            u64 z = f(x);
            #pragma omp critical
            dict_insert(z, x);
            
        }
    }

    // Affiche un message pour chaque processus
    printf("Process %d has filled its hash table with keys from %llu to %llu\n", rank, start_index, end_index - 1);
   

    double mid = wtime();
    printf("Process %d: Fill complete in %.1fs\n", rank, mid - start);

    // Recherche et validation des solutions
   
    u64 ncandidates = 0;
    u64 x[256];
    for (u64 z = 0; z < N; z++) {
        u64 y = g(z);

        // Recherche locale basée sur les 2 derniers bits de y
        int nx = dict_probe_local(y, 256, x, rank, num_processes);
        assert(nx >= 0);
        ncandidates += nx;

        for (int i = 0; i < nx; i++) {
            
            if (is_good_pair(x[i], z)) {
                printf("JE SUIS DANS LE IF");
                if (nres == maxres) {
                    return -1;
                }
                k1[nres] = x[i];
                k2[nres] = z;
                printf("SOLUTION FOUND! (process %d)\n", rank);
                nres += 1;
            }
        }
    }
    printf("Process %d: Probe complete in %.1fs. %" PRId64 " candidate pairs tested\n", rank, wtime() - mid, ncandidates);

    return nres;
}
        