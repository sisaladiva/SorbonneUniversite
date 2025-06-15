#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 1000000000

int main() {
    double pi = 0.0;
    double term;
#pragma omp parallel for reduction(+:pi)
    for (long i = 0; i <= N; i++) {
        term = 1.0 / (1.0 + ((double)i / N) * ((double)i / N));
        pi += term;
    }

    pi = pi * 4.0 / N;

    printf("Approximate value of pi: %.15f\n", pi);

    return 0;
}
