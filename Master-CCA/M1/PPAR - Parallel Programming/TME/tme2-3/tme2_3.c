#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//Question 1: On peut paralléliser l'algorithme présent dans la section 4 en décomposant l'image en plusieurs parties qui seront traitées chacunes par un processeur différent. Une fois que tous les processeurs terminent les calculs des pixels qui leur ont été attribués, on réunti l'image à nouveau en rassemblant les résultats.


int mandelbrot(double x, double y, int max_iter) {
    int i = 0; // compteur d'itérations
    double zx = 0.0, zy = 0.0; 
    double zx2, zy2; 
    
    // on boucle jusqu'à divergence ou nombre maximum d'itérations atteintes
    while (i < max_iter && (zx * zx + zy * zy) < 4.0) {
        zx2 = zx * zx - zy * zy + x; 
        zy2 = 2.0 * zx * zy + y;     
        zx = zx2;
        zy = zy2;
        i++; 
    }
    
    return i; // Renvoie le nombre d'itérations faites avant la divergence
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 
    
    int rang;
    int taille;
    double tempsdebut;
    double tempsfin;
    int larg = 800;
    int longeur = 800; 
    int max_iter = 100; 

    MPI_Comm_rank(MPI_COMM_WORLD, &rang); // rang processeur
    MPI_Comm_size(MPI_COMM_WORLD, &taille); // nb total de processeurs
    

    // calcul des pixels pour chaque processeur
    int debut = rang * (longeur / taille); 
    int fin = (rang + 1) * (longeur / taille); 
    
    unsigned char *image = malloc(larg * longeur * sizeof(unsigned char)); // alloue de la mémoire pour l'image
    
    tempsdebut = MPI_Wtime(); 

    for (int j = debut; j < fin; j++) {
        for (int i = 0; i < larg; i++) {
            double x = (i - larg/2.0) * 4.0 / larg; // partie réelle de c
            double y = (j - longeur/2.0) * 4.0 / longeur; // partie imaginaire de c
            
            int iter = mandelbrot(x, y, max_iter); // calcule l'ensemble de Mandelbrot
            
            image[j * larg + i] = iter % 255; // enregistre le résultat dans l'image
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD); 

    tempsfin = MPI_Wtime(); 

    printf("Processus numero %d: temps d'execution = %f secondes\n", rang, tempsfin - tempsdebut); 

    // on recupere les resultats 
    unsigned char *result_image = NULL;
    if (rang == 0) {
        result_image = malloc(larg * longeur * sizeof(unsigned char)); // alloue de la mémoire pour l'image finale
    }

    MPI_Gather(image, larg * longeur / taille, MPI_UNSIGNED_CHAR, result_image, larg * longeur / taille, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD); // Rassemble les résultats

    free(image); // libere la memoire
    if (rang == 0) {
        free(result_image); // libere la memoire de l'image finale
    }

    MPI_Finalize(); 
    return 0;
}
