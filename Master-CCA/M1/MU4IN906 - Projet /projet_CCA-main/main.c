#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Fonction pour générer une variable aléatoire suivant une loi uniforme
double loiUniforme(double min, double max) {
    // Initialisation de la graine pour la génération aléatoire
    srand(time(NULL));

    // Génération d'un nombre aléatoire entre 0 et RAND_MAX
    double random = (double)rand() / RAND_MAX;

    // Transformation pour obtenir un nombre dans l'intervalle [min, max]
    return min + random * (max - min);
}

int main(void){
    float sum = 0;
    for(int i = 0; i < 100; i++){
        sum += loiUniforme(0,1);
    }
    
    return 0;
}