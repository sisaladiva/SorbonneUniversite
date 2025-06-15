#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Question 4 : On pourrait utiliser une approche dynamique et faire en sorte que a chaque fois qu'un processus finit de faire son travail, on lui accorde une autre tâche afin d'alleger les tahces d'un processus qui mettrait plus de temps. On gagnerait ainsi du temps d'execution total.
// j'ai remis ici ma fonction précedente
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

    int nbmaxtaches = 100;  
    if (rang != 0) {
        MPI_Send(&rang, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        int taches = taille; 
        while (taches > 0 && taches < nbmaxtaches) {
            int proc;
            MPI_Recv(&proc, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (tempsfin < longeur) {
                int nvxdebu = tempsfin;
                int nvxfin = tempsfin + (longeur / taille);
                MPI_Send(&nvxdebu, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
                MPI_Send(&nvxfin, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
                tempsfin = nvxfin;
            } else {
                int dummy = -1; // Aucune tâche supplémentaire disponible
                MPI_Send(&dummy, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
                taches--; // Réduit le nb de tâches attribuées
            }
        }

        // condition d'arrêt explicite
        if (taches >= nbmaxtaches) {
            printf("Stooop nombre maximal de tâches atteint \n");
        }
    }

    printf("lalallaa");
    MPI_Finalize(); 
    printf("HHHHHHHHHHHHHHH"); 
    return 0;
}
