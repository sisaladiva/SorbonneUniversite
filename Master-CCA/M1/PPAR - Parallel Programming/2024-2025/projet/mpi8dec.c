#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <getopt.h>
#include <err.h>
#include <assert.h>
#include <mpi.h>

typedef uint64_t u64;       /* portable 64-bit integer */
typedef uint32_t u32;       /* portable 32-bit integer */
struct __attribute__ ((packed)) entry { u32 k; u64 v; };  /* hash table entry */

/***************************** global variables ******************************/

u64 n = 0;         /* block size (in bits) */
u64 mask;          /* this is 2**n - 1 */
u64 dict_size;     /* number of slots in the hash table */
struct entry *A;   /* the hash table */

/* (P, C) : two plaintext-ciphertext pairs */
u32 P[2][2] = {{0, 0}, {0xffffffff, 0xffffffff}};
u32 C[2][2];

/************************ tools and utility functions *************************/

double wtime()
{
    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (double)ts.tv_sec + ts.tv_usec / 1E6;
}

u64 murmur64(u64 x)
{
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return x;
}

void dict_setup(u64 size) {
    dict_size = size;
    A = malloc(sizeof(*A) * dict_size);
    if (A == NULL) err(1, "impossible to allocate the dictionary");
    for (u64 i = 0; i < dict_size; i++) A[i].k = 0xffffffff;
}

void dict_insert(u64 key, u64 value)
{
    u64 h = murmur64(key) % dict_size;
    for (;;) {
        if (A[h].k == 0xffffffff) break;
        h += 1;
        if (h == dict_size) h = 0;
    }
    assert(A[h].k == 0xffffffff);
    A[h].k = key % 0xfffffffb;
    A[h].v = value;
}

int dict_probe(u64 key, int maxval, u64 values[])
{
    u32 k = key % 0xfffffffb;
    u64 h = murmur64(key) % dict_size;
    int nval = 0;
    for (;;) {
        if (A[h].k == 0xffffffff) return nval;
        if (A[h].k == k) {
            if (nval == maxval) return -1;
            values[nval] = A[h].v;
            nval += 1;
        }
        h += 1;
        if (h == dict_size) h = 0;
    }
}

#define ROTL32(x,r) (((x)<<(r)) | (x>>(32-(r))))
#define ROTR32(x,r) (((x)>>(r)) | ((x)<<(32-(r))))
#define ER32(x,y,k) (x=ROTR32(x,8), x+=y, x^=k, y=ROTL32(y,3), y^=x)
#define DR32(x,y,k) (y^=x, y=ROTR32(y,3), x^=k, x-=y, x=ROTL32(x,8))

void Speck64128KeySchedule(const u32 K[],u32 rk[])
{
    u32 i,D=K[3],C=K[2],B=K[1],A=K[0];
    for(i=0;i<27;){
        rk[i]=A; ER32(B,A,i++);
        rk[i]=A; ER32(C,A,i++);
        rk[i]=A; ER32(D,A,i++);
    }
}

void Speck64128Encrypt(const u32 Pt[], u32 Ct[], const u32 rk[])
{
    u32 i;
    Ct[0]=Pt[0]; Ct[1]=Pt[1];
    for(i=0;i<27;)
        ER32(Ct[1],Ct[0],rk[i++]);
}

void Speck64128Decrypt(u32 Pt[], const u32 Ct[], u32 const rk[])
{
    int i;
    Pt[0]=Ct[0]; Pt[1]=Ct[1];
    for(i=26;i>=0;)
        DR32(Pt[1],Pt[0],rk[i--]);
}

u64 f(u64 k)
{
    assert((k & mask) == k);
    u32 K[4] = {k & 0xffffffff, k >> 32, 0, 0};
    u32 rk[27];
    Speck64128KeySchedule(K, rk);
    u32 Ct[2];
    Speck64128Encrypt(P[0], Ct, rk);
    return ((u64) Ct[0] ^ ((u64) Ct[1] << 32)) & mask;
}

u64 g(u64 k)
{
    assert((k & mask) == k);
    u32 K[4] = {k & 0xffffffff, k >> 32, 0, 0};
    u32 rk[27];
    Speck64128KeySchedule(K, rk);
    u32 Pt[2];
    Speck64128Decrypt(Pt, C[0], rk);
    return ((u64) Pt[0] ^ ((u64) Pt[1] << 32)) & mask;
}

bool is_good_pair(u64 k1, u64 k2)
{
    u32 Ka[4] = {k1 & 0xffffffff, k1 >> 32, 0, 0};
    u32 Kb[4] = {k2 & 0xffffffff, k2 >> 32, 0, 0};
    u32 rka[27];
    u32 rkb[27];
    Speck64128KeySchedule(Ka, rka);
    Speck64128KeySchedule(Kb, rkb);
    u32 mid[2];
    u32 Ct[2];
    Speck64128Encrypt(P[1], mid, rka);
    Speck64128Encrypt(mid, Ct, rkb);
    return (Ct[0] == C[1][0]) && (Ct[1] == C[1][1]);
}

int golden_claw_search(int maxres, u64 k1[], u64 k2[], u64 start_index, u64 end_index)
{
    double start = wtime();
    for (u64 x = start_index; x <= end_index; x++) {
        u64 z = f(x);
        dict_insert(z, x);
    }

    double mid = wtime();
    printf("Rank local Fill: %.1fs\n", mid - start);

    int nres = 0;
    u64 ncandidates = 0;
    u64 x[256];
    for (u64 z = start_index; z <= end_index; z++) {
        u64 y = g(z);
        int nx = dict_probe(y, 256, x);
        assert(nx >= 0);
        ncandidates += nx;
        for (int i = 0; i < nx; i++)
            if (is_good_pair(x[i], z)) {
                if (nres == maxres) return -1;
                k1[nres] = x[i];
                k2[nres] = z;
                nres += 1;
            }
    }
    printf("Rank Probe: %.1fs. %" PRId64 " candidate pairs tested\n", wtime() - mid, ncandidates);
    return nres;
}

void usage(char **argv)
{
    printf("%s [OPTIONS]\n\n", argv[0]);
    printf("Options:\n");
    printf("--n N                       block size [default 24]\n");
    printf("--C0 N                      1st ciphertext (in hex)\n");
    printf("--C1 N                      2nd ciphertext (in hex)\n");
    printf("--start N                   Start index for search\n");
    printf("--end N                     End index for search\n");
    printf("\nAll arguments are required\n");
    exit(0);
}
void process_command_line_options(int argc, char **argv, u64 *start_index, u64 *end_index)
{
    static struct option long_options[] = {
        {"n", required_argument, 0, 'n'},
        {"C0", required_argument, 0, 'C'},
        {"C1", required_argument, 0, 'D'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
        case 'n':
            n = strtoull(optarg, NULL, 10);
            mask = (1ULL << n) - 1;
            break;
        case 'C':
            C[0][0] = strtoull(optarg, NULL, 16);
            break;
        case 'D':
            C[1][0] = strtoull(optarg, NULL, 16);
            break;
        case 's':
            *start_index = strtoull(optarg, NULL, 10);
            break;
        case 'e':
            *end_index = strtoull(optarg, NULL, 10);
            break;
        default:
            usage(argv);
        }
    }

    // Set default start and end if not provided
    if (*start_index == 0 && *end_index == 0) {
        *start_index = 0;
        *end_index = (1ULL << n) - 1;  // Set a default end index based on the block size
    }

    if (n == 0 || *start_index >= *end_index) {
        printf("Error: Invalid or missing parameters.\n");
        usage(argv);
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    u64 start_index = 0, end_index = 0;
    process_command_line_options(argc, argv, &start_index, &end_index);

    u64 local_start = start_index + rank * ((end_index - start_index + 1) / size);
    u64 local_end = (rank == size - 1) ? end_index : local_start + ((end_index - start_index + 1) / size) - 1;

    dict_setup(1 << 20);  // Setup dictionary with an arbitrary large size

    u64 k1[256], k2[256];
    int maxres = 256;
    int nres = golden_claw_search(maxres, k1, k2, local_start, local_end);

    int *recv_counts = NULL;
    int *displs = NULL;
    if (rank == 0) {
        recv_counts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
    }

    MPI_Gather(&nres, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    u64 *global_k1 = NULL;
    u64 *global_k2 = NULL;
    if (rank == 0) {
        int total_results = 0;
        for (int i = 0; i < size; i++) {
            displs[i] = total_results;
            total_results += recv_counts[i];
        }

        global_k1 = malloc(total_results * sizeof(u64));
        global_k2 = malloc(total_results * sizeof(u64));
    }

    MPI_Gatherv(k1, nres, MPI_UINT64_T, global_k1, recv_counts, displs, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    MPI_Gatherv(k2, nres, MPI_UINT64_T, global_k2, recv_counts, displs, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < recv_counts[0]; i++) {
            printf("Pair %llu: k1 = %016llx, k2 = %016llx\n", i, global_k1[i], global_k2[i]);
        }
        free(global_k1);
        free(global_k2);
        free(recv_counts);
        free(displs);
    }

    MPI_Finalize();
    return 0;
}
