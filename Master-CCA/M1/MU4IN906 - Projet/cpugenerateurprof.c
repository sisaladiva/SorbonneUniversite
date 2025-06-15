#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "rng.h"

// Prototypes des fonctions du générateur CMRG
void PostInitDataCMRG();
void FreeCMRG();
double CMRGUniform01();

// Fonction de génération uniforme utilisant CMRG
double CMRGUniform01() {
    // Utiliser les matrices et les seeds pour générer un nombre uniforme entre 0 et 1
    double s1[3], s2[3];
    static int index = 0;
    int m1 = 2147483647, m2 = 2145483479;
    for (int i = 0; i < 3; i++) {
        s1[i] = CMRGp[0][index][i];
        s2[i] = CMRGp[0][index][i + 3];
    }
    MatVecModM(A1, s1, s1, m1);
    MatVecModM(A2, s2, s2, m2);

    for (int i = 0; i < 3; i++) {
        CMRGp[0][index][i] = (int) s1[i];
        CMRGp[0][index][i + 3] = (int) s2[i];
    }

    index = (index + 1) % Mtraj;
    double u = fmod((s1[0] - s2[0] + m1), m1) / m1;
    return u;
}

//BERNOULLI
int bernoulli(double p) {
    double G = CMRGUniform01();  // Génération d'une variable aléatoire uniforme entre 0 et 1
    return (G <= p) ? 1 : 0;
}

//BINOMIALE AVEC BERNOULLI
int binomiale_with_bernoulli(int n, double p) {
    int res = 0;
    for (int i = 0; i < n; i++) {
        res += bernoulli(p);
    }
    return res;
}

int binomial_coefficient(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    k = (k < n - k) ? k : n - k; // Utilisation de la symétrie
    int coeff = 1;
    for (int i = 0; i < k; i++) {
        coeff *= (n - i);
        coeff /= (i + 1);
    }
    return coeff;
}

// Fonction pour générer des échantillons binomiaux
int* binomial(int n, double p, int nb_samples) {
    // Allocation de mémoire pour les échantillons binomiaux
    int* binomial_variables = (int*)malloc(nb_samples * sizeof(int));
    if (binomial_variables == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(1);
    }

    // Génération des échantillons binomiaux
    for (int k = 0; k < nb_samples; k++) {
        double cumulative_prob = 0;
        double uniform_sample = CMRGUniform01();
        for (int i = 0; i <= n; i++) {
            double prob = binomial_coefficient(n, i) * pow(p, i) * pow(1 - p, n - i);
            cumulative_prob += prob;
            if (uniform_sample <= cumulative_prob) {
                binomial_variables[k] = i;
                break;
            }
        }
    }

    return binomial_variables;
}

//EXPONENTIELLE
double exponential(double lam) {
    double u = CMRGUniform01();  // Génération d'un échantillon U uniformément distribué sur [0, 1]
    return -log(1 - u) / lam;
}

//POISSON
int poisson_rejection(double lam) {
    while (1) {
        double x = exponential(lam);
        int k = (int)x;  // Tronquer x pour obtenir un entier
        double u = CMRGUniform01();
        double p = exp(-lam);
        for (int i = 1; i <= k; i++) {
            p *= lam / i;
        }
        if (u <= p) {
            return k;
        }
    }
}

//CHI CARREE
double chi_square(int* observed, double* expected, int intervals) {
    double chi_square = 0.0;
    for (int i = 0; i < intervals; i++) {
        chi_square += pow(observed[i] - expected[i], 2) / expected[i];
    }
    return chi_square;
}

int main() {
    // Initialisation du générateur CMRG
    PostInitDataCMRG();

    // Variables de test
    double p = 0.5;
    int n = 100000;
    int sum = 0;

    // Test de la distribution de Bernoulli
    clock_t temps_debutbern = clock();
    for (int i = 0; i < n; i++) {
        sum += bernoulli(p);
    }
    clock_t temps_finbern = clock();
    double mean_empirical = (double)sum / n;
    printf("Temps d'execution de la distribution de Bernoulli seule : %f secondes\n", (double)(temps_finbern - temps_debutbern) / CLOCKS_PER_SEC);
    printf("Moyenne empirique : %f\n", mean_empirical);
    printf("Paramètre p de la distribution de Bernoulli : %f\n", p);

    // Test de la distribution binomiale avec Bernoulli
    int* samples = malloc(n * sizeof(int));
    clock_t temps_debut = clock();
    for (int j = 0; j < n; j++) {
        samples[j] = binomiale_with_bernoulli(100, p);
    }
    clock_t temps_fin = clock();
    printf("Temps d'execution du generateur binomial avec Bernoulli : %f secondes\n", (double)(temps_fin - temps_debut) / CLOCKS_PER_SEC);
    free(samples);

    // Test de la distribution binomiale sans Bernoulli
    int nn = 100;
    int* samples_binomial = binomial(nn, p, n);
    clock_t temps_debut_binomial = clock();
    int* samples_binomial2 = binomial(nn, p, n);
    clock_t temps_fin_binomial = clock();
    printf("Temps d'execution du generateur binomial sans Bernoulli : %f secondes\n", (double)(temps_fin_binomial - temps_debut_binomial) / CLOCKS_PER_SEC);
    free(samples_binomial);

    // Test de la loi exponentielle
    double lam = 2.0;
    double sum_exp = 0.0;
    clock_t temps_debut_exp = clock();
    for (int i = 0; i < n; i++) {
        double sample = exponential(lam);
        sum_exp += sample;
    }
    clock_t temps_fin_exp = clock();
    printf("Temps d'exécution de la loi exponentielle : %f secondes\n", (double)(temps_fin_exp - temps_debut_exp) / CLOCKS_PER_SEC);
    printf("Paramètre lambda : %f\n", lam);

    // Test de la loi de Poisson
    double lambda_poisson = 5.0;
    int nb_samples_poisson = 100000;
    int* poisson_samples = malloc(nb_samples_poisson * sizeof(int));
    if (poisson_samples == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(1);
    }
    clock_t temps_debut_poisson = clock();
    for (int k = 0; k < nb_samples_poisson; k++) {
        poisson_samples[k] = poisson_rejection(lambda_poisson);
    }
    clock_t temps_fin_poisson = clock();
    printf("Temps d'exécution de la loi de Poisson : %f secondes\n", (double)(temps_fin_poisson - temps_debut_poisson) / CLOCKS_PER_SEC);
    printf("Paramètre lambda de la loi de Poisson : %f\n", lambda_poisson);
    free(poisson_samples);

    // Libération de la mémoire et du générateur CMRG
    FreeCMRG();

    return 0;
}
