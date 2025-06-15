#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>  // Inclure la bibliothèque OpenMP

// Fonction pour générer des échantillons de Bernoulli
int bernoulli(double p) {
    double G = (double)rand() / RAND_MAX;  // Génération d'une variable aléatoire uniforme entre 0 et 1
    if (G <= p) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    int n = 1000000;    // Nombre d'échantillons à générer
    double p = 0.5; // Paramètre p de la distribution de Bernoulli
    int sum = 0;       // Somme des échantillons générés

    // Nombre de threads OpenMP
    int num_threads = omp_get_max_threads();

    // Tableaux pour stocker les temps de début et de fin de chaque processus
    double debut[num_threads];
    double fin[num_threads];

    // Mesure du temps d'exécution
    double debut_global = omp_get_wtime();

    // Paralléliser la boucle avec OpenMP
    #pragma omp parallel
    {
        int tid = omp_get_thread_num(); // ID du thread
        debut[tid] = omp_get_wtime();   // Temps de début du thread

        #pragma omp for reduction(+:sum)
        for (int i = 0; i < n; i++) {
            sum += bernoulli(p);
        }

        fin[tid] = omp_get_wtime(); // Temps de fin du thread
    }

    // Temps d'exécution global
    double fin_global = omp_get_wtime();
    double temps_execution_global = fin_global - debut_global;

    // Affichage des résultats
    printf("Moyenne empirique : %f\n", (double)sum / n);
    printf("Temps d'exécution global : %f secondes\n", temps_execution_global);
    printf("Temps d'exécution de chaque processus:\n");
    for (int i = 0; i < num_threads; i++) {
        printf("Processus %d : %f secondes\n", i, fin[i] - debut[i]);
    }

    return 0;
}
