#include <stdio.h>

void afficher_histogramme(int* echantillons, int taille) {
    // Trouver la valeur maximale dans les échantillons
    int max_value = 0;
    for (int i = 0; i < taille; ++i) {
        if (echantillons[i] > max_value) {
            max_value = echantillons[i];
        }
    }

    // Créer un tableau pour l'histogramme
    int histogramme[max_value + 1];
    for (int i = 0; i <= max_value; ++i) {
        histogramme[i] = 0;
    }

    // Remplir l'histogramme
    for (int i = 0; i < taille; ++i) {
        histogramme[echantillons[i]]++;
    }

    // Afficher l'histogramme
    printf("Histogramme :\n");
    for (int i = 0; i <= max_value; ++i) {
        printf("%d : ", i);
        for (int j = 0; j < histogramme[i]; ++j) {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    // Exemple d'utilisation
    int echantillons[10] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 4};
    int taille = sizeof(echantillons) / sizeof(echantillons[0]);
    afficher_histogramme(echantillons, taille);
    return 0;
}
