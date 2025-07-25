#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>		
#include <string.h>		
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

/* AUTHOR : Charles Bouillaguet <charles.bouillaguet@lip6.fr>
   USAGE  : compile with -lm (and why not -O3)
            redirect the standard output to a text file
            gcc heatsink.c -O3 -lm -o heatsink
            ./heatsink > steady_state.txt
            then run the indicated python script for graphical rendering

   DISCLAIMER : this code does not claim to an absolute realism.
                this code could be obviously improved, but has been written so as
				to make as clear as possible the physics principle of the simulation.
*/

/* one can change the matter of the heatsink, its size, the power of the CPU, etc. */
#define GOLD
#define CHALLENGE           /* MEDIUM is faster, and FAST is even faster (for debugging) */
#define DUMP_STEADY_STATE
double my_gettimeofday()
{
	struct timeval tmp_time;
	gettimeofday(&tmp_time, NULL);
	return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

const double L = 0.15;      /* length (x) of the heatsink (m) */
const double l = 0.12;      /* height (y) of the heatsink (m) */
const double E = 0.008;     /* width (z) of the heatsink (m) */
const double watercooling_T = 20;   /* temperature of the fluid for water-cooling, (°C) */
const double CPU_TDP = 280; /* power dissipated by the CPU (W) */

/* dl: "spatial step" for simulation (m) */
/* dt: "time step" for simulation (s) */
#ifdef FAST
double dl = 0.004;
double dt = 0.004;
#endif

#ifdef MEDIUM
double dl = 0.002;
double dt = 0.002;
#endif

#ifdef NORMAL
double dl = 0.001;
double dt = 0.001;
#endif

#ifdef CHALLENGE
double dl = 0.0001;
double dt = 0.00001;
#endif

/* sink_heat_capacity: specific heat capacity of the heatsink (J / kg / K) */
/* sink_density: density of the heatsink (kg / m^3) */
/* sink_conductivity: thermal conductivity of the heatsink (W / m / K) */
/* euros_per_kg: price of the matter by kilogram */
#ifdef ALUMINIUM
double sink_heat_capacity = 897;
double sink_density = 2710;
double sink_conductivity = 237;
double euros_per_kg = 1.594;
#endif


#ifdef COPPER
double sink_heat_capacity = 385;
double sink_density = 8960;
double sink_conductivity = 390;
double euros_per_kg = 5.469;
#endif

#ifdef GOLD
double sink_heat_capacity = 128;
double sink_density = 19300;
double sink_conductivity = 317;
double euros_per_kg = 47000;
#endif

#ifdef IRON
double sink_heat_capacity = 444;
double sink_density = 7860;
double sink_conductivity = 80;
double euros_per_kg = 0.083;
#endif

const double Stefan_Boltzmann = 5.6703e-8;  /* (W / m^2 / K^4), radiation of black body */
const double heat_transfer_coefficient = 10;    /* coefficient of thermal convection (W / m^2 / K) */
double CPU_surface;

/* 
 * Return True if the CPU is in contact with the heatsink at the point (x,y).
 * This describes an AMD EPYC "Rome".
 */
static inline bool CPU_shape(double x, double y)
{
	x -= (L - 0.0754) / 2;
	y -= (l - 0.0585) / 2;
	bool small_y_ok = (y > 0.015 && y < 0.025) || (y > 0.0337 && y < 0.0437);
	bool small_x_ok = (x > 0.0113 && x < 0.0186) || (x > 0.0193 && x < 0.0266)
	    || (x > 0.0485 && x < 0.0558) || (x > 0.0566 && x < 0.0639);
	bool big_ok = (x > 0.03 && x < 0.045 && y > 0.0155 && y < 0.0435);
	return big_ok || (small_x_ok && small_y_ok);
}


/* returns the total area of the surface of contact between CPU and heatsink (in m^2) */
double CPU_contact_surface()
{
	double S = 0;
	for (double x = dl / 2; x < L; x += dl)
		for (double y = dl / 2; y < l; y += dl)
			if (CPU_shape(x, y))
				S += dl * dl;
	return S;
}

/* Returns the new temperature of the cell (i, j, k). For this, there is an access to neighbor
 * cells (left, right, top, bottom, front, back), except if (i, j, k) is on the external surface. */
static inline double update_temperature(const double *T, int u, int n, int m, int o, int i, int j, int k)
{
		/* quantity of thermal energy that must be brought to a cell to make it heat up by 1°C */
    const double cell_heat_capacity = sink_heat_capacity * sink_density * dl * dl * dl; /* J.K */
    const double dl2 = dl * dl;
    double thermal_flux = 0;

    if (i > 0)
        thermal_flux += (T[u - 1] - T[u]) * sink_conductivity * dl; /* neighbor x-1 */
    else {
        thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
        thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
    }

    if (i < n - 1)
        thermal_flux += (T[u + 1] - T[u]) * sink_conductivity * dl; /* neighbor x+1 */
    else {
        thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
        thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
    }

    if (j > 0)
        thermal_flux += (T[u - n] - T[u]) * sink_conductivity * dl; /* neighbor y-1 */
    else {
        /* Bottom cell: does it receive it from the CPU ? */
        if (CPU_shape(i * dl, k * dl))
            thermal_flux += CPU_TDP / CPU_surface * dl2;
        else {
            thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
            thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
        }
    }

    if (j < m - 1)
        thermal_flux += (T[u + n] - T[u]) * sink_conductivity * dl; /* neighbor y+1 */
    else {
        thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
        thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
    }

    if (k > 0)
        thermal_flux += (T[u - n * m] - T[u]) * sink_conductivity * dl; /* neighbor z-1 */
    else {
        thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
        thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
    }

    if (k < o - 1)
        thermal_flux += (T[u + n * m] - T[u]) * sink_conductivity * dl; /* neighbor z+1 */
    else {
        thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
        thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
    }

    /* adjust temperature depending on the heat flux */
    return T[u] + thermal_flux * dt / cell_heat_capacity;
}

/* Run the simulation on the k-th xy plane.
 * v is the index of the start of the k-th xy plane in the arrays T and R. */
static inline void do_xy_plane(const double *T, double *R, int v, int n, int m, int o, int k)
{
    if (k == 0)
				// we do not modify the z = 0 plane: it is maintained at constant temperature via water-cooling
        return;

    for (int j = 0; j < m; j++) {   // y
        for (int i = 0; i < n; i++) {   // x
            int u = v + j * n + i;
            R[u] = update_temperature(T, u, n, m, o, i, j, k);
        }
    }
}


int main(int argc, char *argv[])
{
	CPU_surface = CPU_contact_surface();
	double V = L * l * E;
	int n = ceil(L / dl);
	int m = ceil(E / dl);
	int o = ceil(l / dl);
	/* Chronometrage */
	double debut, fin;
	
	int rang;		
	int p;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size (MPI_COMM_WORLD, &p );
	MPI_Comm_rank (MPI_COMM_WORLD,&rang);
	if (rang==0) {
		fprintf(stderr, "DISSIPATEUR\n");
		fprintf(stderr, "\tDimension (cm) [x,y,z] = %.1f x %.1f x %.1f\n", 100 * L, 100 * E, 100 * l);
		fprintf(stderr, "\tVolume = %.1f cm^3\n", V * 1e6);
		fprintf(stderr, "\tMasse = %.2f kg\n", V * sink_density);
		fprintf(stderr, "\tPrix = %.2f €\n", V * sink_density * euros_per_kg);
		fprintf(stderr, "SIMULATION\n");
		fprintf(stderr, "\tGrille (x,y,z) = %d x %d x %d (%.1fMo)\n", n, m, o, 7.6293e-06 * n * m * o);
		fprintf(stderr, "\tdt = %gs\n", dt);
		fprintf(stderr, "CPU\n");
		fprintf(stderr, "\tPuissance = %.0fW\n", CPU_TDP);
		fprintf(stderr, "\tSurface = %.1f cm^2\n", CPU_surface * 10000);
	}
	/* temperature of each cell, in degree Kelvin. */
	double *T = malloc(n * m * o * sizeof(*T));
	double *R = malloc(n * m * o * sizeof(*R));
	if (T == NULL || R == NULL) {
		perror("T or R could not be allocated");
		exit(1);
	}


		
	debut = my_gettimeofday(); 	// chronomètrage

	 /* initially the heatsink is at the temperature of the water-cooling fluid */
	for (int u =((n*m*o)/p)*rang ; u <((n*m*o)/p)*(rang+1); u++){  //on parcourt une tranche
		R[u] = T[u] = watercooling_T + 273.15;}
	/* let's go! we switch the CPU on and launch the simulation until it reaches a stationary state. */
	double t = 0;
	int n_steps = 0;
	int convergence = 0;
	 char filename[20];
  sprintf(filename, "checkpoint_%d.txt", rang);
  FILE *file = fopen(filename, "r");




  if (file != NULL) {
      // Lecture des données depuis le fichier
      for (int u = rang * (o / p) * n * m; u < (rang + 1) * (o / p) * n * m; u++) {
          fscanf(file, "%lf", &T[u]);
      }
      fclose(file);
  }
  int checkpoint_counter = 0; // Déclaration de la variable pour compter les sauvegardes
int checkpoint_interval =100; // Déclaration de l'intervalle pour les sauvegardes 


	/* simulating time steps */
    while (convergence == 0) {

  
MPI_Request request[4] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL};
if (rang < p - 1) MPI_Issend(&T[((rang + 1) * (o / p) * n * m) - n * m], n * m, MPI_DOUBLE, rang + 1, 0, MPI_COMM_WORLD, &request[0]);
if (rang > 0) MPI_Irecv(&T[((rang) * (o / p) * n * m) - n * m], n * m, MPI_DOUBLE, rang - 1, 0, MPI_COMM_WORLD, &request[1]);
if (rang > 0) MPI_Issend(&T[rang * (o / p) * n * m], n * m, MPI_DOUBLE, rang - 1, 0, MPI_COMM_WORLD, &request[2]);
if (rang < p - 1) MPI_Irecv(&T[(rang + 1) * (o / p) * n * m], n * m, MPI_DOUBLE, rang + 1, 0, MPI_COMM_WORLD, &request[3]);
// Le bloc va de rang*(o/p)*n*m à (rang+1)*(o/p)*n*m
       for (int k = (rang*(o/p)+1); k < ((rang+1)*(o/p)-1); k++) {
           int v = k * n * m;
           do_xy_plane(T, R, v, n, m, o, k);
       }
MPI_Waitall(4, request, MPI_STATUSES_IGNORE);


do_xy_plane(T, R, rang*(o/p)*n*m, n, m, o, rang*(o/p));
       do_xy_plane(T, R,  ((rang+1)*(o/p)-1)*n*m, n, m, o,  ((rang+1)*(o/p)-1));

      char filename[20];
      sprintf(filename, "checkpoint_%d.txt", rang);  // Génère un nom de fichier unique pour chaque processus




      if (checkpoint_counter == checkpoint_interval) {
          FILE *file = fopen(filename, "w");
          if (file != NULL) {
              // Écriture des données dans le fichier
              for (int u = rang * (o / p) * n * m; u < (rang + 1) * (o / p) * n * m; u++) {
                  fprintf(file, "%.6f\n", T[u]);
              }
              fclose(file);
          }




          checkpoint_counter = 0;
      } else {
          checkpoint_counter++;
      }
		/* each second, we test the convergence, and print a short progress report */
		if (n_steps % ((int)(1 / dt)) == 0) {
			double delta_T = 0;
			double max = -INFINITY;
			
			for (int u =  n * m * (o/p)*rang; u < n * m * (o/p)*(rang+1); u++) {
				delta_T += (R[u] - T[u]) * (R[u] - T[u]);
				if (R[u] > max)
					max = R[u];
					
			}
			
			MPI_Allreduce(MPI_IN_PLACE,&delta_T,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD); //on obtient une valeur agrégée de delta_T 
			delta_T = sqrt(delta_T) / dt;

			//on obtient la température maximale
			if (rang == 0) {
				MPI_Reduce(MPI_IN_PLACE,&max,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
				fprintf(stderr, "t = %.1fs ; T_max = %.1f°C ; convergence = %g\n", t, max - 273.15, delta_T);
			}
			else{
				MPI_Reduce(&max,&max,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
			}

			
			if (delta_T < 0.1)
				convergence = 1;
		}

		 /* the new temperatures are in R */
		double *tmp = R;
		R = T;
		T = tmp;
		t += dt;
		n_steps += 1;
	}
	fin = my_gettimeofday();
	
	fprintf(stdout, "Le processus %d a mit %g sec à l'éxécution\n",rang, fin - debut);
	if (rang != 0) {
			MPI_Ssend(&T[rang*(o/p)*n*m], (o/p)*n*m, MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	else{
		
		for (size_t i = 0; i < p-1; i++) {
			double *temp = malloc((o/p)*n*m*sizeof(double));
			MPI_Recv(temp,(o/p)*n*m,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			int giver = status.MPI_SOURCE;
			for (size_t ii = 0; ii < (o/p)*n*m; ii++) {
				T[giver*(o/p)*n*m+ii] = temp[ii];
			}
			free(temp);
		}
		FILE *fp;
		fp = fopen("./res.txt", "w+");
	#ifdef DUMP_STEADY_STATE
		printf("###### STEADY STATE; t = %.1f\n", t);
		for (int k = 0; k < o; k++) {	// z
			fprintf(fp,"# z = %g\n", k * dl);
			for (int j = 0; j < m; j++) {	// y
				for (int i = 0; i < n; i++) {	// x
					fprintf(fp,"%.1f ", T[k * n * m + j * n + i] - 273.15);
				}
				fprintf(fp,"\n");
			}
		}
		fprintf(fp,"\n");
		fprintf(stderr, "Rendu graphique : python3 rendu_picture_steady.py [filename.txt] %d %d %d\n", n, m, o);
	#endif
	}
	free(R);
	free(T);
	
	MPI_Finalize();
	exit(EXIT_SUCCESS);
}
