#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>



//BERNOULLI
int bernoulli(double p) {
    double G = (double)rand() / RAND_MAX;  // Génération d'une variable aléatoire uniforme entre 0 et 1
    if (G <= p) {
        return 1;
    } else {
        return 0;
    }
}



//BINOMIALE AVEC BERNOULLI
int binomiale_with_bernoulli(int n, double p) {
    int res = 0;
    int i;
    for (i = 0; i < n; i++) {
        res += bernoulli(p);
    }
    return res;
}
/*
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


*/
void binomial_coefficients(int n, int* coefficients) {
    for (int k = 0; k <= n; k++) {
        coefficients[k] = 1;
        for (int i = 1; i <= k; i++) {
            coefficients[k] = coefficients[k] * (n - i + 1) / i;
        }
    }
}

// Fonction pour générer des échantillons binomiaux sans Bernoulli
int* binomial_without_bernoulli(int n, double p, int nb_samples) {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    // Allocation de mémoire pour les échantillons binomiaux et les tableaux de coefficients
    int* binomial_variables = (int*)malloc(nb_samples * sizeof(int));
    int* coefficients = (int*)malloc((n + 1) * sizeof(int));
    double* probabilities = (double*)malloc((n + 1) * sizeof(double));
    double* cumulative_probabilities = (double*)malloc((n + 1) * sizeof(double));
    
    if (binomial_variables == NULL || coefficients == NULL || probabilities == NULL || cumulative_probabilities == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(1);
    }

    // Calculer les coefficients binomiaux
    binomial_coefficients(n, coefficients);

    // Calculer les probabilités binomiales
    for (int i = 0; i <= n; i++) {
        probabilities[i] = coefficients[i] * pow(p, i) * pow(1 - p, n - i);
    }

    // Calculer les probabilités cumulatives
    cumulative_probabilities[0] = probabilities[0];
    for (int i = 1; i <= n; i++) {
        cumulative_probabilities[i] = cumulative_probabilities[i - 1] + probabilities[i];
    }

    // Générer les échantillons binomiaux en utilisant les échantillons uniformes
    for (int k = 0; k < nb_samples; k++) {
        double uniform_sample = (double)rand() / RAND_MAX;
        // Recherche binaire pour trouver l'intervalle de la probabilité cumulative
        int left = 0, right = n;
        while (left < right) {
            int mid = (left + right) / 2;
            if (uniform_sample <= cumulative_probabilities[mid]) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        binomial_variables[k] = left;
    }

    // Libération de la mémoire allouée pour les tableaux temporaires
    free(coefficients);
    free(probabilities);
    free(cumulative_probabilities);

    return binomial_variables;
}


//EXPONENTIELLE
double exponential(double lam) {
    double u = (double)rand() / RAND_MAX;  // Génération d'un échantillon U uniformément distribué sur [0, 1]
    double x = -1.0 / lam * log(1 - u);

    return x;
}




//POISSON
int* poisson(double lam, int sample) {
    // Initialisation des résultats
    int *results = (int *)malloc(sample * sizeof(int));

    // Initialisation des compteurs et des produits
    int *k = (int *)malloc(sample * sizeof(int));
    double *u_product = (double *)malloc(sample * sizeof(double));
    int *mask = (int *)malloc(sample * sizeof(int));

    for (int i = 0; i < sample; i++) {
        results[i] = 0;
        k[i] = 0;
        u_product[i] = 1.0;
        mask[i] = 1;
    }

    // Pré-calcul de exp(-lam)
    double exp_neg_lam = exp(-lam);

    clock_t start_time = clock();

    // Boucle jusqu'à ce que tous les échantillons soient générés
    int mask_any = 1;
    while (mask_any) {
        mask_any = 0;

        for (int i = 0; i < sample; i++) {
            if (mask[i]) {
                // Génération d'un nombre aléatoire uniforme
                double u = (double)rand() / RAND_MAX;
                u_product[i] *= u;

                // Mise à jour des compteurs
                k[i] += 1;

                // Vérification si le produit est inférieur au seuil
                if (u_product[i] < exp_neg_lam) {
                    results[i] = k[i] - 1;
                    mask[i] = 0;
                }

                // Vérification si des éléments du masque sont toujours actifs
                if (mask[i]) {
                    mask_any = 1;
                }
            }
        }
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Generated %d samples of a Poisson distribution using rejection method with lam = %f in %f seconds.\n", sample, lam, elapsed_time);

    // Retourne les résultats
    return results;
}



// Fonction Box-Muller pour générer des nombres aléatoires suivant une distribution normale
void box_muller(int sample_size, double *samples) {
    int i;
    double u1, u2, z0, z1;

    for (i = 0; i < sample_size; i += 2) {
        u1 = (double)rand() / RAND_MAX;
        u2 = (double)rand() / RAND_MAX;

        z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2); // Première valeur de la paire
        z1 = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2); // Deuxième valeur de la paire

        samples[i] = z0;
        if (i + 1 < sample_size) {
            samples[i + 1] = z1;
        }
    }
}
void chi_square(int n_samples, int degrees_of_freedom, double *chi2_samples) {
    int i, j;
    for (i = 0; i < n_samples; i++) {
        chi2_samples[i] = 0;
        for (j = 0; j < degrees_of_freedom; j++) {
            double z;
            box_muller(1, &z);
            chi2_samples[i] += z * z;
        }
    }
}


/*

int main() {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

///BERNOULLI///
    clock_t temps_debutbern = clock();
    double p = 0.5;  // Paramètre p de la distribution de Bernoulli
    int n = 100000;   // Nombre d'échantillons à générer
    int i, sum = 0;
    for (i = 0; i < n; i++) {
        sum += bernoulli(p);
    }
    clock_t temps_finbern = clock();
        
    double mean_empirical = (double)sum / n;        // Calcul de la moyenne empirique
    
    printf("Temps d'execution de la distribution de Bernoulli seule : %f secondes\n", (double)(temps_finbern - temps_debutbern) / CLOCKS_PER_SEC);
    printf("Moyenne empirique : %f\n", mean_empirical);
    printf("Paramètre p de la distribution de Bernoulli : %f\n", p);



///BINOMIALE AVEC BERNOULLI///
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



    //LOI EXPONENTIELLE
    srand(time(NULL));

    double lam = 2.0;  // Paramètre lambda de la loi exponentielle
    // Mesure du temps de génération des échantillons de loi exponentielle
    clock_t temps_debut_exp = clock();
    double sum_exp = 0.0;
    for (int i = 0; i < n; i++) {
        double sample = exponential(lam);
        sum_exp += sample;
    }
    clock_t temps_fin_exp = clock();

    // Affichage du temps d'exécution et de la moyenne empirique
    printf("Temps d'exécution de la loi exponentielle : %f secondes\n", (double)(temps_fin_exp - temps_debut_exp) / CLOCKS_PER_SEC);
    printf("Paramètre lambda : %f\n", lam);

 //LOI DE POISSON
    double lambda_poisson = 5.0;  // Paramètre lambda de la loi de Poisson
    int nb_samples_poisson = 100000; // Nombre d'échantillons à générer pour la loi de Poisson
int* poisson_samples = malloc(nb_samples_poisson * sizeof(int));  // Tableau pour stocker les échantillons de la loi de Poisson
    if (poisson_samples == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(1);
    }
    // Mesure du temps de génération des échantillons de la loi de Poisson
    clock_t temps_debut_poisson = clock();
    
    for (int k = 0; k < nb_samples_poisson; k++) {
        poisson_samples[k] = poisson_rejection(lambda_poisson);
    }
    clock_t temps_fin_poisson = clock();


    // Affichage du temps d'exécution et de la moyenne empirique
    printf("Temps d'exécution de la loi de Poisson : %f secondes\n", (double)(temps_fin_poisson - temps_debut_poisson) / CLOCKS_PER_SEC);
    printf("Paramètre lambda de la loi de Poisson : %f\n", lambda_poisson);

    free(poisson_samples); // Libération de la mémoire allouée pour les échantillons de la loi de Poisson




    return 0;
}
*/


int main() {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    // Ouverture du fichier de sortie
    FILE *output_file = fopen("bernoulli_samples.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }

    // Paramètres pour la distribution de Bernoulli
    double p = 0.5;
    int n = 1000000;
    int i, sum = 0;

    // Mesure du temps d'exécution de la distribution de Bernoulli seule
    clock_t temps_debutbern = clock();

    // Génération des échantillons et enregistrement dans le fichier de sortie
    for (i = 0; i < n; i++) {
        int sample = bernoulli(p);
        fprintf(output_file, "%d\n", sample); // Enregistrement de l'échantillon dans le fichier de sortie
        sum += sample;
    }

    clock_t temps_finbern = clock();

    // Calcul de la moyenne empirique
    double mean_empirical = (double)sum / n;

    // Affichage des résultats
    printf("Temps d'execution de la distribution de Bernoulli seule : %f secondes\n", (double)(temps_finbern - temps_debutbern) / CLOCKS_PER_SEC);
    printf("Moyenne empirique : %f\n", mean_empirical);
    printf("Paramètre p de la distribution de Bernoulli : %f\n", p);

    // Fermeture du fichier de sortie
    fclose(output_file);



    // BINOMIALE AVEC BERNOULLI
    int j;
    int* samples = malloc(n * sizeof(int));  // Tableau pour stocker les échantillons binomiaux

    // Mesure du temps de génération des échantillons binomiaux avec Bernoulli
    clock_t temps_debut = clock();
    for (j = 0; j < n; j++) {
        samples[j] = binomiale_with_bernoulli(100, p);
    }
    clock_t temps_fin = clock();

    // Enregistrement des résultats dans un fichier de sortie
    FILE *output_file1 = fopen("binomial_with_bernoulli_samples.txt", "w");
    if (output_file1 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }
    for (j = 0; j < n; j++) {
        fprintf(output_file1, "%d\n", samples[j]); // Enregistrement de chaque échantillon dans le fichier
    }
    fclose(output_file1);

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

    // Enregistrement des résultats dans un fichier de sortie
    FILE *output_file2 = fopen("binomial_without_bernoulli_samples.txt", "w");
    if (output_file2 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }
    for (j = 0; j < n; j++) {
        fprintf(output_file2, "%d\n", samples_binomial[j]); // Enregistrement de chaque échantillon dans le fichier
    }
    fclose(output_file2);

    // Affichage du temps d'exécution 
    printf("Temps d'execution du generateur binomial sans Bernoulli : %f secondes\n", (double)(temps_fin_binomial - temps_debut_binomial) / CLOCKS_PER_SEC);
    free(samples_binomial);  // Libération de la mémoire allouée pour les échantillons binomiaux


// LOI EXPONENTIELLE
    double lam = 2.0;  // Paramètre lambda de la loi exponentielle

    // Mesure du temps de génération des échantillons de loi exponentielle
    clock_t temps_debut_exp = clock();
    double sum_exp = 0.0;
    for (int i = 0; i < n; i++) {
        double sample = exponential(lam);
        sum_exp += sample;
    }
    clock_t temps_fin_exp = clock();

    // Enregistrement des résultats dans un fichier de sortie
    FILE *output_file3 = fopen("exponential_samples.txt", "w");
    if (output_file1 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        fprintf(output_file3, "%f\n", exponential(lam)); // Enregistrement de chaque échantillon dans le fichier
    }
    fclose(output_file3);

    // Affichage du temps d'exécution et du paramètre lambda
    printf("Temps d'exécution de la loi exponentielle : %f secondes\n", (double)(temps_fin_exp - temps_debut_exp) / CLOCKS_PER_SEC);
    printf("Paramètre lambda : %f\n", lam);

    // LOI DE POISSON
   double lambda_poisson = 5.0;  // Paramètre lambda de la loi de Poisson
    int nb_samples_poisson = 1000000; // Nombre d'échantillons à générer pour la loi de Poisson

    // Mesure du temps de génération des échantillons de la loi de Poisson
    clock_t temps_debut_poisson = clock();

    // Génération des échantillons de la loi de Poisson et enregistrement dans un fichier
    FILE *output_file4 = fopen("poisson_samples.txt", "w");
    if (output_file4 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }
    int* samples_poisson = poisson(lambda_poisson, nb_samples_poisson);
    for (int k = 0; k < nb_samples_poisson; k++) {
        fprintf(output_file4, "%d\n", samples_poisson[k]); // Enregistrement de chaque échantillon dans le fichier
    }
    fclose(output_file4);
    free(samples_poisson);

    clock_t temps_fin_poisson = clock();

    // Affichage du temps d'exécution et du paramètre lambda
    printf("Temps d'exécution de la loi de Poisson : %f secondes\n", (double)(temps_fin_poisson - temps_debut_poisson) / CLOCKS_PER_SEC);
    printf("Paramètre lambda de la loi de Poisson : %f\n", lambda_poisson);
 int degrees_of_freedom = 10;
    double *chi2_samples = (double *)malloc(n * sizeof(double));
    if (chi2_samples == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        return 1;
    }  
    
    // Mesure du temps d'exécution
    clock_t starttt, enddd;
    double cpu_time_used;

    starttt = clock();
    chi_square(n, degrees_of_freedom, chi2_samples);
    enddd = clock();

    cpu_time_used = ((double) (enddd - starttt)) / CLOCKS_PER_SEC;

    FILE *output_file5 = fopen("chi2_samples.txt", "w");
    if (output_file5 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        free(chi2_samples);
        return 1;
    }
    for (int i = 0; i < n; i++) {
        fprintf(output_file5, "%.6f\n", chi2_samples[i]);
    }
    fclose(output_file5);

    free(chi2_samples);

    // Affichage du temps d'exécution
    printf("Temps d'exécution de chi_square: %.6f secondes\n", cpu_time_used);


    return 0;
}