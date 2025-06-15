#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Fonction pour générer des échantillons de Bernoulli
int bernoulli(double p) {
    double G = (double)rand() / RAND_MAX;  // Génération d'une variable aléatoire uniforme entre 0 et 1
    if (G <= p) {
        return 1;
    } else {
        return 0;
    }
}

// Fonction pour générer un échantillon binomial en utilisant la fonction bernoulli
int binomiale_with_bernoulli(int n, double p) {
    int res = 0;
    int i;
    for (i = 0; i < n; i++) {
        res += bernoulli(p);
    }
    return res;
}

// Fonction pour calculer le coefficient binomial C(n, k)
int binomial_coefficient(int n, int k) {
    if (k < 0 || k > n) {
        return 0;
    }
    if (k == 0 || k == n) {
        return 1;
    }
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
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    // Allocation de mémoire pour les échantillons binomiaux
    int* binomial_variables = (int*)malloc(nb_samples * sizeof(int));
    if (binomial_variables == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(1);
    }

    // Génération des échantillons binomiaux
    for (int k = 0; k < nb_samples; k++) {
        double cumulative_prob = 0;
        double uniform_sample = (double)rand() / RAND_MAX;
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





//exponentielle
double exponential(double lam) {
    // Génération d'un échantillon U uniformément distribué sur [0, 1]
    double u = (double)rand() / RAND_MAX;
    double x = -1.0 / lam * log(1 - u);

    return x;
}




//loi de poisson avec la methode de rejet
int poisson_rejection(double lam) {
    while (1) {
        // Générer un échantillon à partir d'une distribution exponentielle
        double x = exponential(lam);
        int k = (int)x;  // Tronquer x pour obtenir un entier

        // Générer une variable aléatoire uniforme
        double u = (double)rand() / RAND_MAX;

        // Calculer la probabilité de la loi de Poisson
        double p = exp(-lam);
        for (int i = 1; i <= k; i++) {
            p *= lam / i;
        }

        // Accepter ou rejeter l'échantillon
        if (u <= p) {
            return k;
        }
    }
}


int main() {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    //BERNOULLI
    double p = 0.5;  // Paramètre p de la distribution de Bernoulli
    int n = 10000;   // Nombre d'échantillons à générer
    int i, sum = 0;
    clock_t temps_debut_bernouilli = clock();
    for (i = 0; i < n; i++) {
        sum += bernoulli(p);
    }
    clock_t temps_fin_bernouilli = clock();
    // Calcul de la moyenne empirique
    double mean_empirical = (double)sum / n;
    // Affichage de la moyenne empirique et du paramètre p
    printf("Temps d'execution du generateur Bernoulli : %f secondes\n", (double)(temps_fin_bernouilli - temps_debut_bernouilli) / CLOCKS_PER_SEC);

    printf("Moyenne empirique : %f\n", mean_empirical);
    printf("Paramètre p de la distribution de Bernoulli : %f\n", p);


    // BINOMIALE AVEC BERNOULLI
    int j;
    int* samples = malloc(n * sizeof(int));  // Tableau pour stocker les échantillons binomiaux

    // Mesure du temps de génération des échantillons binomiaux
    clock_t temps_debut = clock();
    for (j = 0; j < n; j++) {
        samples[j] = binomiale_with_bernoulli(100, p);
    }
    clock_t temps_fin = clock();

    // Affichage du temps d'exécution
    printf("Temps d'execution du generateur binomial avec Bernoulli : %f secondes\n", (double)(temps_fin - temps_debut) / CLOCKS_PER_SEC);

    free(samples);  // Libération de la mémoire allouée pour le tableau des échantillons

    // BINOMIALE SANS BERNOULLI
    int nn = 100;      // Nombre d'essais

    // Génération des échantillons binomiaux
    int* samples_binomial = binomial(nn, p, n);

    // Mesure du temps d'exécution 
    clock_t temps_debut_binomial = clock();
    int* samples_binomial2 = binomial(nn, p, n);
    clock_t temps_fin_binomial = clock();

    // Affichage du temps d'exécution 
    printf("Temps d'execution du generateur binomial sans Bernoulli : %f secondes\n", (double)(temps_fin_binomial - temps_debut_binomial) / CLOCKS_PER_SEC);
    free(samples_binomial);  // Libération de la mémoire allouée pour les échantillons binomiaux

double lam = 2.0;
int num_samples = 10000; // Nombre d'échantillons à générer
clock_t start_exp = clock();
for (int i = 0; i < num_samples; i++) {
    double sample = exponential(lam); // Appelez la fonction exponentielle ici
}
clock_t end_exp = clock();
double time_exp = ((double) (end_exp - start_exp)) / CLOCKS_PER_SEC;
printf("Temps d'exécution de la fonction exponentielle : %f secondes\n", time_exp);


clock_t start_poisson = clock();
for (int i = 0; i < num_samples; i++) {
    int sample = poisson_rejection(lam); // Appelez la fonction poisson_rejection ici
}
clock_t end_poisson = clock();
double time_poisson = ((double) (end_poisson - start_poisson)) / CLOCKS_PER_SEC;
printf("Temps d'exécution de la fonction poisson_rejection : %f secondes\n", time_poisson);


    return 0;
}
