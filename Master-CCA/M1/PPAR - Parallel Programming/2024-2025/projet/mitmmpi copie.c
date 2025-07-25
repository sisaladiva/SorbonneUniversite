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
#include <omp.h>
#include <math.h>
typedef uint64_t u64;       /* portable 64-bit integer */
typedef uint32_t u32;       /* portable 32-bit integer */
struct __attribute__ ((packed)) entry { u32 k; u64 v; };  /* hash table entry */

/***************************** global variables ******************************/

u64 n = 0;         /* block size (in bits) */
u64 mask;          /* this is 2**n - 1 */
int nres = 0;

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

// murmur64 hash functions, tailorized for 64-bit ints / Cf. Daniel Lemire
u64 murmur64(u64 x)
{
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return x;
}

/* represent n in 4 bytes */
void human_format(u64 n, char *target)
{
    if (n < 1000) {
        sprintf(target, "%" PRId64, n);
        return;
    }
    if (n < 1000000) {
        sprintf(target, "%.1fK", n / 1e3);
        return;
    }
    if (n < 1000000000) {
        sprintf(target, "%.1fM", n / 1e6);
        return;
    }
    if (n < 1000000000000ll) {
        sprintf(target, "%.1fG", n / 1e9);
        return;
    }
    if (n < 1000000000000000ll) {
        sprintf(target, "%.1fT", n / 1e12);
        return;
    }
}

/******************************** SPECK block cipher **************************/

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

/******************************** dictionary ********************************/

/*
 * "classic" hash table for 64-bit key-value pairs, with linear probing.  
 * It operates under the assumption that the keys are somewhat random 64-bit integers.
 * The keys are only stored modulo 2**32 - 5 (a prime number), and this can lead 
 * to some false positives.
 */
static const u32 EMPTY = 0xffffffff;
static const u64 PRIME = 0xfffffffb;

u64 total_inserted = 0;  // Compteur global pour le nombre d'éléments insérés
/* allocate a hash table with `size` slots (12*size bytes) */
void dict_setup(u64 size) {
    dict_size = size;
    char hdsize[8];
    human_format(dict_size * sizeof(*A), hdsize);
    printf("Total dictionary size: %sB\n", hdsize);  // Affiche la taille de la table de hachage

    A = malloc(sizeof(*A) * dict_size);
    if (A == NULL)
        err(1, "impossible to allocate the dictionnary");

    #pragma omp parallel for
    for (u64 i = 0; i < dict_size; i++)
        A[i].k = EMPTY;

    printf("Taille de la table de hachage: %llu\n", dict_size);  // Imprime la taille de la table
}
void dict_setup_local(u64 size, struct entry **local_table) {
    *local_table = malloc(sizeof(struct entry) * size);
    if (*local_table == NULL) {
        err(1, "Impossible d'allouer une sous-table locale");
    }
    for (u64 i = 0; i < size; i++) {
        (*local_table)[i].k = EMPTY;
    }
}

/* Insert the binding key |----> value in the dictionnary */
/*void dict_insert(u64 key, u64 value)
{
    u64 h = murmur64(key) % dict_size;
    u64 i = 0;  // Compteur pour le sondage quadratique
    while (A[h].k != EMPTY) {
        i++;
        h = (h + i * i) % dict_size;  // Sondage quadratique
    }
    A[h].k = key % PRIME;
    A[h].v = value;
}*/

void dict_insert(u64 key, u64 value) {
    u64 h = murmur64(key) % dict_size;  // Calculer le hachage
    u64 i = 0;  // Compteur pour le sondage quadratique
    while (A[h].k != EMPTY) {  // Trouver un emplacement vide
        i++;
        h = (h + i * i) % dict_size;  // Sondage quadratique
    }
    A[h].k = key % PRIME;  // Stocker la clé
    A[h].v = value;  // Stocker la valeur

    // Incrémenter le compteur d'éléments insérés
    #pragma omp atomic
    total_inserted++;
}

/* Query the dictionnary with this `key`.  Write values (potentially) 
 *  matching the key in `values` and return their number. The `values`
 *  array must be preallocated of size (at least) `maxval`.
 *  The function returns -1 if there are more than `maxval` results.
 */
int dict_probe(u64 key, int maxval, u64 values[])
{
    u32 k = key % PRIME;
    u64 h = murmur64(key) % dict_size;
    int nval = 0;
    for (;;) {
        if (A[h].k == EMPTY)
            return nval;
        if (A[h].k == k) {
        	if (nval == maxval)
        		return -1;
            values[nval] = A[h].v;
            nval += 1;
        }
        h += 1;
        if (h == dict_size)
            h = 0;
   	}
}


double log2(double x) {
    return log(x) / log(2);
}


int dict_probe_local(u64 key, int maxval, u64 values[], int rank, int num_processes) {

    int number_bits= log2(num_processes);
    u64 mask = (1ULL << number_bits) - 1;
    // On prend les 2 derniers bits pour déterminer le processus cible
    int target_process = (key & mask);  

    // Si la clé appartient au processus courant (d'après ses 2 derniers bits), on la recherche localement
    if (target_process == rank) {
        return dict_probe(key, maxval, values);
    }
    return 0; // La clé ne correspond pas à ce processus
}

/***************************** MITM problem ***********************************/

/* f : {0, 1}^n --> {0, 1}^n.  Speck64-128 encryption of P[0], using k */
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


/* g : {0, 1}^n --> {0, 1}^n.  speck64-128 decryption of C[0], using k */
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

void dict_clear() {
    // Vérifie si la table de hachage existe
    if (A != NULL) {
        free(A); // Libère la mémoire allouée pour la table
        A = NULL; // Réinitialise le pointeur pour éviter les accès invalides
    }
    dict_size = 0; // Réinitialise la taille de la table
}

int golden_claw_search_mpi(int maxres, u64 k1[], u64 k2[], int rank, int num_processes)
{
    double start = wtime();
    u64 N = 1ull << n;  // N = 2^n, pour n=10, N = 1024
    u64 local_dict_size = (1.125 * N) / num_processes;  // Taille du dictionnaire local

    // Calculer le nombre maximal de processus autorisés
    u64 max_processes = (1.125 * N) / 1024;  // Taille minimale de 1024 entrées par thread

    // Ajuster le nombre de processus si nécessaire
    if (num_processes > max_processes) {
        printf("Avertissement: Le nombre de processus (%d) est trop élevé pour n=%d. Réduction à %llu processus.\n", num_processes, n, max_processes);
        num_processes = max_processes;  // Ajuster le nombre de processus à la valeur maximale
        local_dict_size = (1.125 * N) ;  // Recalculer la taille du dictionnaire local
    }

    // Afficher la taille du dictionnaire local
    printf("La taille du dictionnaire local est %llu\n", local_dict_size);

    for (int parity = 0; parity < 2; parity++) {
        dict_setup(local_dict_size );  // Initialisation du dictionnaire avec la moitié de la taille locale

        // Remplissage de la table de hachage
        #pragma omp parallel 
        {
            // Chaque thread utilise une structure locale pour stocker les entrées
            struct entry *local_entries = malloc(sizeof(struct entry) * (local_dict_size / omp_get_num_threads()));
            
            if (!local_entries) {
                printf("Erreur: Allocation mémoire échouée pour local_entries (process %d, thread %d)\n", rank, omp_get_thread_num());
                exit(EXIT_FAILURE);
            }

            u64 local_count = 0;  // Compteur pour les entrées locales
            int number_bits = log2(num_processes);  // Nombre de bits pour le masque
            u64 mask = (1ULL << number_bits) - 1;  // Masque pour le filtrage des processus

            #pragma omp for
            for (u64 x = 0; x < N; x++) {
                u64 z = f(x);  // Calcul de z à partir de x
                
                // Combine les conditions pour réduire les branches
                if (((z & 1) == parity) && ((x & mask) == rank)) {
                    // Vérification de la capacité avant d'ajouter une nouvelle entrée
                    if (local_count < local_dict_size / omp_get_num_threads()) {
                        local_entries[local_count].k = z % PRIME;  // Stocke la clé dans local_entries
                        local_entries[local_count].v = x;  // Stocke la valeur dans local_entries
                        local_count++;
                    } else {
                        // Gérer le dépassement de capacité
                        printf("Avertissement: Dépassement de capacité pour local_entries (process %d, thread %d)\n", rank, omp_get_thread_num());
                        // Vous pouvez choisir de sortir de la boucle ou de continuer à chercher d'autres entrées
                        // exit(EXIT_FAILURE); // Décommenter pour arrêter l'exécution en cas de dépassement
                    }
                }
            }

            // Fusionner les données locales dans la table globale
            #pragma omp critical
            for (u64 i = 0; i < local_count; i++) {
                dict_insert(local_entries[i].k, local_entries[i].v);  // Insertion dans le dictionnaire global
            }

            free(local_entries);  // Libération de la mémoire allouée pour local_entries

        }

        u64 ncandidates = 0;  // Compteur pour les candidats
        u64 x[256];  // Tableau pour stocker les candidats, taille fixe de 256

        // Structure pour stocker deux paires z, g(z)
        struct {
            u64 z[4];  // Tableau pour stocker jusqu'à 4 valeurs de z
            u64 gz[4]; // Tableau pour stocker les valeurs g(z) correspondantes
        } cache;

        // Parcourir z par paires
        for (u64 z = 0; z < N; z += 4) {
            // Calculer g(z) pour quatre valeurs consécutives une seule fois
            u64 gz0 = g(z);
            u64 gz1 = g(z + 1);
            u64 gz2 = g(z + 2);
            u64 gz3 = g(z + 3);

            // Traiter les quatre valeurs
            for (int i = 0; i < 4; i++) {
                u64 y;
                if (i == 0) y = gz0;
                else if (i == 1) y = gz1;
                else if (i == 2) y = gz2;
                else y = gz3;

                if ((y % 2) == parity) {
                    int nx = dict_probe_local(y, 256, x, rank, num_processes);  // Recherche dans le dictionnaire local
                    assert(nx >= 0);  // Vérifie que le nombre d'entrées trouvées est valide
                    ncandidates += nx;  // Incrémente le compteur de candidats

                    // Optimiser la boucle interne en vérifiant la condition is_good_pair
                    for (int k = 0; k < nx; k++) {
                        if (is_good_pair(x[k], z + i)) {
                            if (nres == maxres) {
                                return -1;  // Retourne -1 si le nombre maximum de résultats est atteint
                            }
                           if (nres < maxres) {
    k1[nres] = x[k];  // Stocke la clé dans k1
    k2[nres] = z + i;  // Stocke la valeur z dans k2
    nres += 1;  // Incrémente le compteur de résultats
} else {
    printf("Avertissement: Maximum de résultats atteint (process %d, nres: %llu, maxres: %d)\n", rank, nres, maxres);
}
                        }
                    }
                }
            }
        }

        // Traiter les valeurs restantes (0 à 3 valeurs)
        for (u64 z = N - (N % 4); z < N; z++) {
            u64 y = g(z);  // Calcul de g(z)
            if ((y % 2) == parity) {
                printf("Process %d: Appel de dict_probe_local avec y: %llu\n", rank, y);
int nx = dict_probe_local(y, 256, x, rank, num_processes);  // Recherche dans le dictionnaire local
if (nx < 0) {
    printf("Erreur: dict_probe_local a échoué (process %d, y: %llu)\n", rank, y);
} 
                assert(nx >= 0);  // Vérifie que le nombre d'entrées trouvées est valide
                ncandidates += nx;  // Incrémente le compteur de candidats

                for (int k = 0; k < nx; k++) {
                    if (is_good_pair(x[k], z)) {
                        if (nres == maxres) {
                            return -1;  // Retourne -1 si le nombre maximum de résultats est atteint
                        }
                        k1[nres] = x[k];  // Stocke la clé dans k1
                        k2[nres] = z;  // Stocke la valeur z dans k2
                        printf("SOLUTION FOUND! (process %d)\n", rank);  // Affiche que la solution a été trouvée
                        nres += 1;  // Incrémente le compteur de résultats
                    }
                }
            }
        }

        printf("Process %d: Hash table filled and searched with %s f(x).\n", 
               rank, (parity == 0) ? "even" : "odd");  // Affiche le statut de la recherche

        dict_clear();  // Efface le dictionnaire
    }

    double end_time = wtime();  // Mesure le temps de fin
    printf("Process %d: Execution time: %.2f seconds\n", rank, end_time - start);  // Affiche le temps d'exécution
printf("Total d'éléments insérés dans la table de hachage: %llu\n", total_inserted);
    return nres;  // Retourne le nombre de résultats trouvés
}
/************************** command-line options ****************************/

void usage(char **argv)
{
        printf("%s [OPTIONS]\n\n", argv[0]);
        printf("Options:\n");
        printf("--n N                       block size [default 24]\n");
        printf("--C0 N                      1st ciphertext (in hex)\n");
        printf("--C1 N                      2nd ciphertext (in hex)\n");
        printf("\n");
        printf("All arguments are required\n");
        exit(0);
}

void process_command_line_options(int argc, char ** argv)
{
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

/******************************************************************************/

int main(int argc, char **argv)
{
    // Initialisation de MPI
    MPI_Init(&argc, &argv);

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    process_command_line_options(argc, argv);
    printf("Running with n=%d, C0=(%08x, %08x) and C1=(%08x, %08x)\n", 
        (int)n, C[0][0], C[0][1], C[1][0], C[1][1]);


    // Search with MPI parallelisation
    u64 k1[16], k2[16];
    int nkey = golden_claw_search_mpi(16, k1, k2, rank, num_processes);
// Réduction MPI pour agréger les résultats de nkey

int nkey_global = 0;

MPI_Allreduce(&nkey, &nkey_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

if (rank == 0) {
    printf("Valeur totale de nkey : %d\n", nkey_global);
}

 
if (nkey_global <= 0) {
    fprintf(stderr, "Erreur : aucune solution trouvée.\n");
    //MPI_Finalize();
    //return EXIT_FAILURE;
}

// Validation des solutions si nkey_global > 0
for (int i = 0; i < nkey; i++) {
    assert(f(k1[i]) == g(k2[i]));
    assert(is_good_pair(k1[i], k2[i]));		
    printf("Solution found: (%" PRIx64 ", %" PRIx64 ") [checked OK] (process %d)\n", k1[i], k2[i], rank);
}
  
    // Finaliser MPI
    MPI_Finalize();

    return 0;
}