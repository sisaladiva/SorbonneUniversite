#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Structure pour représenter un nombre complexe
typedef struct Complex {
    double real;      // Partie réelle
    double imaginary; // Partie imaginaire
} Complex;

// Addition de nombres complexes
Complex add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imaginary = a.imaginary + b.imaginary;
    return result;
}

// Soustraction de nombres complexes
Complex subtract(Complex a, Complex b) {
    Complex result;
    result.real = a.real - b.real;
    result.imaginary = a.imaginary - b.imaginary;
    return result;
}

// Multiplication de nombres complexes
Complex multiply(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imaginary * b.imaginary;
    result.imaginary = a.real * b.imaginary + a.imaginary * b.real;
    return result;
}

// Ajouter des zéros pour étendre le vecteur à une taille de puissance de 2
Complex* resizeToPowerOfTwo(Complex* vector, int n, int* new_size) {
    int power = 1;
    while (power < n) {
        power *= 2;
    }

    *new_size = power;
    Complex* expanded_vector = (Complex*)calloc(power, sizeof(Complex));

    for (int i = 0; i < n; ++i) {
        expanded_vector[i] = vector[i];
    }

    return expanded_vector;
}



// Fonction FFT récursive
void fft(Complex* vector, int n, int is_inverse, int *size) {
    if (n <= 1) {
        return;
    }

    // Séparation du vecteur en parties impaire et paire
    Complex* even = (Complex*)malloc(n / 2 * sizeof(Complex));
    Complex* odd = (Complex*)malloc(n / 2 * sizeof(Complex));

    for (int i = 0; i < n / 2; ++i) {
        even[i] = vector[2 * i];
        odd[i] = vector[2 * i + 1];
    }

    // Récursion sur les parties impaire et paire
    fft(even, n / 2, is_inverse, size);
    fft(odd, n / 2, is_inverse, size);

    // Calcul de la transformée pour chaque étape de la récursion
    double angle = 2 * M_PI / (*size) * (is_inverse ? -1 : 1);
    Complex w = {1, 0};
    Complex wn = {cos(angle), sin(angle)};

    for (int i = 0; i < n / 2; ++i) {
        vector[i] = add(even[i], multiply(w, odd[i]));
        vector[i + n / 2] = subtract(even[i], multiply(w, odd[i]));

        if (is_inverse) {
            vector[i].real /= 2;
            vector[i].imaginary /= 2;
            vector[i + n / 2].real /= 2;
            vector[i + n / 2].imaginary /= 2;
        }

        w = multiply(w, wn);
    }

    free(even);
    free(odd);
}

// Fonction pour calculer la FFT d'un vecteur
Complex * calculate_fft(Complex* vector, int n, int *size) {
    int new_size;
   vector = resizeToPowerOfTwo(vector, n, &new_size);
    fft(vector, *size, 0, &new_size); // 0 pour la FFT normale

   *size = new_size;
return vector;
    
}
// Fonction IFFT récursive

Complex* ifft(Complex* vector, int n, int *size) {
    // Appel de la fonction FFT avec le paramètre is_inverse à 1 pour inverser la FFT
    Complex * vec= calculate_fft(vector, n, size);

    // Normalisation par la taille de l'échantillon à la fin de l'IFFT
    for (int i = 0; i < *size; ++i) {
        vec[i].real /= *size;
        vec[i].imaginary /= *size;
    }

    return vec;
}

// Conversion d'un tableau d'entiers en tableau de nombres complexes
Complex* intArrayToComplex(int* tableau, int taille) {
    Complex* tableau_complexes = (Complex*)malloc(taille * sizeof(Complex));
    if (tableau_complexes == NULL) {
        // Gestion de l'échec de l'allocation mémoire
        printf("Erreur d'allocation de mémoire.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < taille; i++) {
        tableau_complexes[i].real = (double)tableau[i];
        tableau_complexes[i].imaginary = 0.0;
    }

    return tableau_complexes;
}

int* multiply_polynomials(int* poly1, int m, int* poly2, int n) {
    // Degré du résultat
    int result_degree = n + m;
    int* result = (int*)calloc(result_degree + 1, sizeof(int));

   
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            result[i + j] += poly1[i] * poly2[j];
        }
    }

    return result;
}



Complex* complex_vector_multiply(Complex* vector1, Complex* vector2, int size) {
    Complex* result = (Complex*)malloc(size * sizeof(Complex));

    if (result == NULL) {
        printf("Erreur d'allocation de mémoire.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        result[i].real = vector1[i].real * vector2[i].real - vector1[i].imaginary * vector2[i].imaginary;
        result[i].imaginary = vector1[i].real * vector2[i].imaginary + vector1[i].imaginary * vector2[i].real;
    }

    return result;
}

Complex* reduce_vector(Complex* vector, int* size) {
    int newSize = *size;
    while (newSize > 0 && (vector[newSize - 1].real == 0 && vector[newSize - 1].imaginary == 0)) {
        newSize--;
    }

    Complex* reducedVector = (Complex*)malloc(newSize * sizeof(Complex));
    if (reducedVector == NULL) {
        printf("Erreur d'allocation de mémoire.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < newSize; i++) {
        reducedVector[i] = vector[i];
    }

    *size = newSize;
    return reducedVector;
}


int* multiply_polynomials_fft(int* poly1, int degree1, int* poly2, int degree2) {
    // Convertir les polynômes en vecteurs complexes
    Complex* complex_poly1 = intArrayToComplex(poly1, degree1 + 1);
    Complex* complex_poly2 = intArrayToComplex(poly2, degree2 + 1);

    
    int result_degree = degree1 + degree2;

    // Taille pour la FFT, laquelle est la prochaine puissance de 2 après le degré du polynôme résultant
    int fft_size = 1;
    while (fft_size <= result_degree) {
        fft_size *= 2;
    }

    Complex* fft_poly1 = resizeToPowerOfTwo(complex_poly1, degree1 + 1, &fft_size);
    Complex* fft_poly2 = resizeToPowerOfTwo(complex_poly2, degree2 + 1, &fft_size);

    fft(fft_poly1, fft_size, 0, &fft_size); // FFT du premier polynôme
    fft(fft_poly2, fft_size, 0, &fft_size); // FFT du deuxième polynôme


    Complex* fft_product = complex_vector_multiply(fft_poly1, fft_poly2, fft_size);
    Complex* ifft_product = ifft(fft_product, fft_size, &fft_size);

    // Réduire le résultat à la taille du polynôme résultant
    Complex* reduced_result = reduce_vector(ifft_product, &fft_size);

    int* result = (int*)calloc(result_degree + 1, sizeof(int));

    
    for (int i = 0; i <= result_degree; i++) {
        result[i] = (int)round(reduced_result[i].real); // Arrondir à l'entier le plus proche
    }

    // Libérer la mémoire
    free(complex_poly1);
    free(complex_poly2);
    free(fft_poly1);
    free(fft_poly2);
    free(fft_product);
    free(ifft_product);
    free(reduced_result);

    return result;
}

int* generate_random_polynomial(int n) {
    // Allouer de la mémoire pour le polynôme
    int* poly = (int*) malloc((n + 1) * sizeof(int));

    // Générer des coefficients aléatoires pour le polynôme
    for (int i = 0; i <= n; ++i) {
        poly[i] = rand() % 100;  // Générer un nombre aléatoire entre 0 et 99
    }

    return poly;
}
void tempsexe(int max_n, int m) {
    // Ouvrir le fichier pour l'écriture
    FILE* file = fopen("times.txt", "w");

    for (int n = 1; n <= max_n; ++n) {
        // Générer des polynômes aléatoires de degré n et m
        int* aa = generate_random_polynomial(n);
        int* bb = generate_random_polynomial(m);

        // Mesurer le temps d'exécution de multiply_polynomials
        clock_t start = clock();
        int* c = multiply_polynomials(aa, n, bb, m);
        clock_t end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("multiply_polynomials avec n=%d et m=%d a pris %f secondes pour s'exécuter.\n", n, m, time_taken);
        fprintf(file, "%f,", time_taken);  // Écrire le temps dans le fichier
        free(c);

        // Mesurer le temps d'exécution de multiply_polynomials_fft
        start = clock();
        c = multiply_polynomials_fft(aa, n, bb, m);
        end = clock();
        time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("multiply_polynomials_fft avec n=%d et m=%d a pris %f secondes pour s'exécuter.\n\n\n", n, m, time_taken);
        fprintf(file, "%f\n", time_taken);  // Écrire le temps dans le fichier
        free(c);

        // Libérer la mémoire des polynômes
        free(aa);
        free(bb);
    }

    // Fermer le fichier
    fclose(file);
}

int main() {
    printf("\n\n\n\t\tEXEMPLES EXECUTIONS DES FONCTIONS \n\n\n");

    printf("Multiplication fft : Exemple 1\n");
    int poly1[] = {1, 1, 1, 0}; // Premier polynôme: x^2+x+1
    int degree1 = sizeof(poly1) / sizeof(poly1[0]) - 1;
    printf("Pôlynôme 1:\n");
    for (int i = 0; i < degree1 + 1; i++) {
        printf("%d ", poly1[i]);
    }
    printf("\n");
    int poly2[] = {2, 5, 0}; // Deuxième polynôme: 10^x+8
    int degree2 = sizeof(poly2) / sizeof(poly2[0]) - 1;
    printf("Pôlynôme 2:\n");
    for (int i = 0; i < degree2 + 1; i++) {
        printf("%d ", poly2[i]);
    }
    printf("\n");

    // Multiplication des polynômes
    int* result = multiply_polynomials_fft(poly1, degree1, poly2, degree2);

    // Affichage du résultat
    printf("Résultat de la multiplication des polynômes :\n");
    for (int i = 0; i < degree1 + degree2 + 1; i++) {
        printf("%d ", result[i]);
    }
    printf("\n\n");

    printf("Multiplication fft : Exemple 2\n");
    int premier_polynome[] = {8, 10, 0}; // Premier polynôme: 10^x+8
    int degre_premier_polynome = sizeof(premier_polynome) / sizeof(premier_polynome[0]) - 1;
    printf("Pôlynôme 1:\n");
    for (int i = 0; i < degre_premier_polynome + 1; i++) {
        printf("%d ", premier_polynome[i]);
    }
    printf("\n");
    int deuxieme_polynome[] = {8, 10, 0}; // Deuxième polynôme: 10^x+8
    int degre_deuxieme_polynome = sizeof(deuxieme_polynome) / sizeof(deuxieme_polynome[0]) - 1;
    printf("Pôlynôme 2:\n");
    for (int i = 0; i < degre_deuxieme_polynome + 1; i++) {
        printf("%d ", deuxieme_polynome[i]);
    }
    printf("\n");
    // Multiplication des polynômes
    int* resultat = multiply_polynomials_fft(premier_polynome, degre_premier_polynome, deuxieme_polynome, degre_deuxieme_polynome);

    // Affichage du résultat
    printf("Résultat de la multiplication des polynômes :\n");
    for (int i = 0; i < degre_premier_polynome + degre_deuxieme_polynome; i++) {
        printf("%d ", resultat[i]);
    }
    printf("\n\n");

    printf("Multiplication fft : Exemple 3\n");
    int premier_polynome_ex3[] = {9, 0}; // Premier polynôme: 10^x+8
    int degre_premier_polynome_ex3 = sizeof(premier_polynome_ex3) / sizeof(premier_polynome_ex3[0]) - 1;
    printf("Pôlynôme 1:\n");
    for (int i = 0; i < degre_premier_polynome_ex3 + 1; i++) {
        printf("%d ", premier_polynome_ex3[i]);
    }
    printf("\n");
    int deuxieme_polynome_ex3[] = {3, 6}; // Deuxième polynôme: 10^x+8
    int degre_deuxieme_polynome_ex3 = sizeof(deuxieme_polynome_ex3) / sizeof(deuxieme_polynome_ex3[0]) - 1;
    printf("Pôlynôme 2:\n");
    for (int i = 0; i < degre_deuxieme_polynome_ex3 + 1; i++) {
        printf("%d ", deuxieme_polynome_ex3[i]);
    }
    printf("\n");
    // Multiplication des polynômes
    int* resultat_ex3 = multiply_polynomials_fft(premier_polynome_ex3, degre_premier_polynome_ex3, deuxieme_polynome_ex3, degre_deuxieme_polynome_ex3);

    // Affichage du résultat
    printf("Résultat de la multiplication des polynômes :\n");
    for (int i = 0; i < degre_premier_polynome_ex3 + degre_deuxieme_polynome_ex3; i++) {
        printf("%d ", resultat_ex3[i]);
    }
    printf("\n\n");

    free(result); // Libérer la mémoire allouée pour le résultat

    printf("\n\n\t\tMULTIPLICATION NAIVE\n\n");

    printf("Multiplication naïve : Exemple 1\n");
    int poly1_naif[] = {6, 3, 10, 0}; // Premier polynôme: x^2 + x + 1
    int degree1_naif = sizeof(poly1_naif) / sizeof(poly1_naif[0]) - 1;
    printf("Pôlynôme 1:\n");
    for (int i = 0; i < degree1_naif + 1; i++) {
        printf("%d ", poly1_naif[i]);
    }
    printf("\n");

    int poly2_naif[] = {4, 5, 0}; // Deuxième polynôme: 5x^2 + 2x
    int degree2_naif = sizeof(poly2_naif) / sizeof(poly2_naif[0]) - 1;
    printf("Pôlynôme 2:\n");
    for (int i = 0; i < degree2_naif + 1; i++) {
        printf("%d ", poly2_naif[i]);
    }
    printf("\n");

    // Multiplication des polynômes de manière naïve
    int* result_naif = multiply_polynomials(poly1_naif, degree1_naif, poly2_naif, degree2_naif);

    // Affichage du résultat
    printf("Résultat de la multiplication des polynômes :\n");
    for (int i = 0; i < degree1_naif + degree2_naif + 1; i++) {
        printf("%d ", result_naif[i]);
    }
    printf("\n\n\n");

    // Libération de la mémoire allouée pour le résultat
    free(result_naif);

    printf("\n\t\t FFT \n");

    Complex complex_numbers[] = {
        {8.0, 0.0}, {10.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}
    };
    int vector_size = sizeof(complex_numbers) / sizeof(complex_numbers[0]);
    int fft_size = vector_size;
    printf("\nVecteur de base :\n");
    for (int i = 0; i < fft_size; ++i) {
        printf("%.2f + %.2fi\n", complex_numbers[i].real, complex_numbers[i].imaginary);
    }

    //fft(complex_numbers, fft_size, 0, &fft_size);

    fft(complex_numbers, fft_size, 0, &fft_size);

    printf("\nRésultat de la FFT :\n");
    for (int i = 0; i < fft_size; ++i) {
        printf("%.2f + %.2fi\n", complex_numbers[i].real, complex_numbers[i].imaginary);
    }

    // Exemple d'utilisation de l'IFFT sur un vecteur de nombres complexes
    Complex numbers[] = {
        {8.0, 0.0}, {10.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}
    };
    int vec_size = sizeof(numbers) / sizeof(numbers[0]);
    int size = vec_size;
    printf("\n\t\t IFFT \n");
    printf("\nVecteur de base :\n");
    for (int i = 0; i < size; ++i) {
        printf("%.2f + %.2fi\n", numbers[i].real, numbers[i].imaginary);
    }

    // Appliquer l'IFFT sur le vecteur
    Complex* ifft_result = ifft(numbers, vec_size, &size);

    printf("\nRésultat de l'IFFT :\n");
    for (int i = 0; i < size; ++i) {
        printf("%.2f + %.2fi\n", ifft_result[i].real, ifft_result[i].imaginary);
    }

    // Libérer la mémoire allouée
    free(ifft_result);

    return 0;
}
