#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define SIZE 1000  // Taille du tableau de données
#define NBINS 200  // Nombre d'intervalles
#define NTRIALS 10000000  // Nombre d'essais pour la distribution binomiale
#define P 0.5  // Probabilité de succès pour la distribution binomiale

int main() {
    int bins[NBINS] = {0}; // Tableau pour compter les occurrences dans chaque intervalle
    const gsl_rng_type * T;
    gsl_rng * r;
    int i, j;

    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    // Générer les données suivant une distribution binomiale
    for(i = 0; i < SIZE; i++) {
        unsigned int k = gsl_ran_binomial(r, P, NTRIALS);
        if (k < NBINS) {
            bins[k]++;
        }
    }

    // Afficher l'histogramme
    for(i = 0; i < NBINS; i++) {
        printf("%2d : ", i);
        for(j = 0; j < bins[i]; j+=10) { // Ajuster le facteur de mise à l'échelle si nécessaire
            printf("*");
        }
        printf("\n");
    }

    gsl_rng_free(r);

    return 0;
}


















