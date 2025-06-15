#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <omp.h>

double my_gettimeofday()
{
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

// Calcul de Fibonacci pour plusieurs valeurs avec vectorisation
void fib_vectorized(int *results, int n)
{
    int a[n], b[n];
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        a[i] = 0;
        b[i] = 1;
    }

    for (int step = 2; step <= n; step++) {
        #pragma omp simd
        for (int i = 0; i < n; i++) {
            int temp = a[i] + b[i];
            a[i] = b[i];
            b[i] = temp;
        }
    }

   
    return b[n-1]
}

int main(int argc, char **argv)
{
    int n = 45;

    if (argc == 2)
        n = atoi(argv[1]);

    // Tableau pour stocker les résultats
    int *results = (int *)malloc(n * sizeof(int));

    double debut = my_gettimeofday();

    fib_vectorized(results, n);

    double fin = my_gettimeofday();

    printf("Fibonacci results:\n");
    for (int i = 0; i < n; i++) {
        printf("fib(%d) = %d\n", i, results[i]);
    }

    printf("Total computation time (with gettimeofday()) : %g s\n", fin - debut);

    free(results);
    return 0;
}
