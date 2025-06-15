#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>

#define ZERO_CELSIUS_IN_KELVIN 273.15

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
#define FAST         /* MEDIUM is faster, and FAST is even faster (for debugging) */
#define ALUMINIUM
#define DUMP_STEADY_STATE

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
static inline double update_temperature(const double *T, int u, int n, int m, int o, int i, int j, int k, int p, int my_rank, double * echange)
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
        if (CPU_shape(i * dl, (my_rank * (o / p) + k) * dl))
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
        /* k == 0 */
        /* That means we're dealing with the bottom line of our subsection */
        if (my_rank == 0) {
            /* I am the first process, no one is below me */
            thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
            thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
        } else {
            /* Another process is below me, need to read the data it sent to me */
            if (echange == NULL) fprintf(stderr, "Wtf 1 are you stupid (k=%d, o=%d, my_rank=%d, p=%d)\n", k, o/p, my_rank, p);
            // printf("#%d echange[j * n + i] = echange[%d * %d + %d] = %.2lf, T[u]=%.2lf\n", my_rank, j, n, i, echange[j * n + i], T[u]);
            thermal_flux += (echange[j * n + i] - T[u]) * sink_conductivity * dl; /* neighbor z-1 */
        }
    }

    if (k < (o/p) - 1)
        thermal_flux += (T[u + n * m] - T[u]) * sink_conductivity * dl; /* neighbor z+1 */
    else {
        /* k == o - 1 */
        /* That means we're dealing with the top line of our subsection */
        if (my_rank == p - 1) {
            /* I am the last process, no one is above me APART FROM GOD HIMSELF */
            thermal_flux -= Stefan_Boltzmann * dl2 * pow(T[u], 4);
            thermal_flux -= heat_transfer_coefficient * dl2 * (T[u] - watercooling_T);
        } else {
            /* I am NOT the last process, another process is above me and I must read the data it sent to me */
            if (echange == NULL) fprintf(stderr, "Wtf 2 are you stupid (k=%d, o=%d, my_rank=%d, p=%d)\n", k, o/p, my_rank, p);
            // printf("#%d echange[j * n + i + n * m] = echange[%d * %d + %d + %d * %d] = %.2lf, T[u]=%.2lf\n", my_rank, j, n, i, n, m, echange[j * n + i + n * m], T[u]);
            thermal_flux += (echange[j * n + i + n * m] - T[u]) * sink_conductivity * dl; /* neighbor z+1 */
        }
    }

    /* adjust temperature depending on the heat flux */
    return T[u] + thermal_flux * dt / cell_heat_capacity;
}

/* Run the simulation on the k-th xy plane.
 * v is the index of the start of the k-th xy plane in the arrays T and R. */
static inline void do_xy_plane(const double *T, double *R, int v, int n, int m, int o, int k, int p, int my_rank)
{
    if ((my_rank == 0) && (k == 0))  // we do not modify the z = 0 plane: it is maintained at constant temperature via water-cooling
        return;

    for (int j = 0; j < m; j++) {   // y
        for (int i = 0; i < n; i++) {   // x
            int u = v + j * n + i;
            R[u] = update_temperature(T, u, n, m, o, i, j, k, p, my_rank, NULL);
            if (R[u] < 290)
                printf("Invalid value: %.2lf (v=%d n=%d m=%d o=%d k=%d my_rank=%d)\n", R[u], v, n, m, o, k, my_rank);
        }
    }
}

static inline void do_first_plane(const double * T, double * R, int n, int m, int o, int k, int p, int my_rank, double * echange)
{
    if ((my_rank == 0) && (k == 0))  // we do not modify the z = 0 plane: it is maintained at constant temperature via water-cooling
        return;

    for (int j = 0; j < m; j++) {   // y
        for (int i = 0; i < n; i++) {   // x
            int u = j * n + i;
            R[u] = update_temperature(T, u, n, m, o, i, j, k, p, my_rank, echange);
            if (R[u] < 290)
                printf("Invalid value: %.2lf (u=%d n=%d m=%d o=%d k=%d my_rank=%d)\n", R[u], u, n, m, o, k, my_rank);
        }
    }
}

static inline void do_last_plane(const double * T, double * R, int n, int m, int o, int k, int p, int my_rank, double * echange)
{
    unsigned int v = n * m * (o / p) - n * m;

    for (int j = 0; j < m; j++) {   // y
        for (int i = 0; i < n; i++) {   // x
            int u = v + j * n + i;
            R[u] = update_temperature(T, u, n, m, o, i, j, k, p, my_rank, echange);
            if (R[u] < 290)
                printf("Invalid value: %.2lf (v=%d u=%d n=%d m=%d o=%d k=%d my_rank=%d)\n", R[u], v, u, n, m, o, k, my_rank);
            // printf("%.2lf\n", t);
        }
    }
}

int main()
{
    int p, my_rank;

    //MPI initialisation
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
 
    CPU_surface = CPU_contact_surface();
    double V = L * l * E;
    int n = ceil(L / dl);
    int m = ceil(E / dl);
    int o = ceil(l / dl);

    if (my_rank == 0) {
        fprintf(stderr, "HEATSINK\n");
        fprintf(stderr, "\tDimension (cm) [x,y,z] = %.1f x %.1f x %.1f\n", 100 * L, 100 * E, 100 * l);
        fprintf(stderr, "\tVolume = %.1f cm^3\n", V * 1e6);
        fprintf(stderr, "\tWeight = %.2f kg\n", V * sink_density);
        fprintf(stderr, "\tPrice = %.2f €\n", V * sink_density * euros_per_kg);
        fprintf(stderr, "SIMULATION\n");
        fprintf(stderr, "\tGrid (x,y,z) = %d x %d x %d (%.1fMo)\n", n, m, o, 7.6293e-06 * n * m * o);
        fprintf(stderr, "\tdt = %gs\n", dt);
        fprintf(stderr, "CPU\n");
        fprintf(stderr, "\tPower = %.0fW\n", CPU_TDP);
        fprintf(stderr, "\tArea = %.1f cm^2\n", CPU_surface * 10000);
    }

    MPI_Barrier(MPI_COMM_WORLD);



    int sectionSize = n * m * (o / p); // Taille d'une section en 1D
    int taille_surface_echange = n * m;

    double * T = (double *) malloc(sizeof(double) * sectionSize);  // Everyone allocates the same work array
    double * R = (double *) malloc(sizeof(double) * sectionSize);
    double * echange = (double *) malloc(sizeof(double) * taille_surface_echange * 2);

    if (T == NULL || R == NULL) {
        perror("T or R could not be allocated");
        exit(1);
    }

    /* initially the heatsink is at the temperature of the water-cooling fluid */
    for (unsigned int u = 0; u < sectionSize; u++)
        R[u] = T[u] = watercooling_T + ZERO_CELSIUS_IN_KELVIN;

    for (unsigned int u = 0; u < taille_surface_echange * 2; u++)
        echange[u] = watercooling_T + ZERO_CELSIUS_IN_KELVIN;


    /* let's go! we switch the CPU on and launch the simulation until it reaches a stationary state. */
    double t = 0;
    unsigned int n_steps = 0;
    char has_converged = 0;

    MPI_Request request_recv[2];
    MPI_Request request_send[2];

    /* simulating time steps */
    while (has_converged == 0) {

        // On reçoit les parties des autres pour notre calcul :
        if (my_rank == 0)
            MPI_Irecv(echange + taille_surface_echange, taille_surface_echange, MPI_DOUBLE, my_rank + 1, 
                      0, MPI_COMM_WORLD, &request_recv[1]);

        else if (my_rank == p - 1)
            MPI_Irecv(echange, taille_surface_echange, MPI_DOUBLE, my_rank - 1,
                      0, MPI_COMM_WORLD, &request_recv[0]);

        else {
            // On reçoit les données de la tranche du dessus
            MPI_Irecv(echange + taille_surface_echange, taille_surface_echange, MPI_DOUBLE, my_rank + 1,
                      0, MPI_COMM_WORLD, &request_recv[1]);

            // On reçoit les données de la tranche du dessous
            MPI_Irecv(echange, taille_surface_echange, MPI_DOUBLE, my_rank - 1,
                      0, MPI_COMM_WORLD, &request_recv[0]);
        }

        /* We calculate the inner part of our own slice while waiting for the data */
        if (my_rank == 0) {
            for (unsigned int k = 0; k < (o / p) - 1; k++) {   // z
                unsigned int v = k * n * m;
                do_xy_plane(T, R, v, n, m, o, k, p, my_rank);
            }
        } else if (my_rank == p - 1) {
            for (unsigned int k = 1; k < (o / p); k++) {       // z
                unsigned int v = k * n * m;
                do_xy_plane(T, R, v, n, m, o, k, p, my_rank);
            }
        } else {
            for (unsigned int k = 1; k < (o / p) - 1; k++) {   // z
                unsigned int v = k * n * m;
                do_xy_plane(T, R, v, n, m, o, k, p, my_rank);
            }
        }

        
        if (my_rank == 0)
            MPI_Isend(T + sectionSize - taille_surface_echange, taille_surface_echange, MPI_DOUBLE, my_rank + 1,
                      0, MPI_COMM_WORLD, &request_send[1]);

        else if (my_rank == p - 1)
            MPI_Isend(T, taille_surface_echange, MPI_DOUBLE, my_rank - 1,
                      0, MPI_COMM_WORLD, &request_send[0]);

        else {
            MPI_Isend(T + sectionSize - taille_surface_echange, taille_surface_echange, MPI_DOUBLE, my_rank + 1,
                      0, MPI_COMM_WORLD, &request_send[1]);

            MPI_Isend(T, taille_surface_echange, MPI_DOUBLE, my_rank - 1,
                      0, MPI_COMM_WORLD, &request_send[0]);
        }

        // On attend d'avoir reçu les données de ses voisins pour faire les calculs
        if (my_rank == 0)
            MPI_Wait(&request_recv[1], MPI_STATUS_IGNORE);

        else if (my_rank == p - 1)
            MPI_Wait(&request_recv[0], MPI_STATUS_IGNORE);

        else
            MPI_Waitall(2, request_recv, MPI_STATUSES_IGNORE);


        /* Update all cells. xy planes are processed, for the first and last plane. */
        if (my_rank == 0) {
            do_last_plane(T, R, n, m, o, (o / p) - 1, p, my_rank, echange);
        } else if (my_rank == p - 1) {
            do_first_plane(T, R, n, m, o, 0, p, my_rank, echange);
        } else {
            do_last_plane(T, R, n, m, o, (o / p) - 1, p, my_rank, echange);
            do_first_plane(T, R, n, m, o, 0, p, my_rank, echange);
        }

        /* each second, we test the convergence, and print a short progress report */
        if (n_steps % ((int) (1 / dt)) == 0) {
            double delta_T = 0;
            double max = -INFINITY;

            for (unsigned int u = 0; u < sectionSize; u++) {
                delta_T += (R[u] - T[u]) * (R[u] - T[u]);

                if (R[u] > max)
                    max = R[u];
            }

            // -- Synchronisation ici --
            MPI_Allreduce(MPI_IN_PLACE, &delta_T, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allreduce(MPI_IN_PLACE,     &max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
            // -------------------------

            delta_T = sqrt(delta_T) / dt;

            if (my_rank == 0)
                fprintf(stderr, "t = %.1fs ; T_max = %.1f°C ; delta_T = %g my_rank = %d\n", t, max - ZERO_CELSIUS_IN_KELVIN, delta_T, my_rank);

            if (delta_T < 0.1)
                has_converged = 1;

        }

        /* the new temperatures are in R */
        double * tmp = R;
        R = T;
        T = tmp;

        t += dt;
        n_steps += 1;

        // On vérifie qu'on a envoyé les données à nos voisins
        if (my_rank == 0)
            MPI_Wait(&request_send[1], MPI_STATUS_IGNORE);

        else if (my_rank == p - 1)
            MPI_Wait(&request_send[0], MPI_STATUS_IGNORE);

        else
            MPI_Waitall(2, request_send, MPI_STATUSES_IGNORE);
    }

#ifdef DUMP_STEADY_STATE

    double *final = NULL;
    if (my_rank == 0)
        final = (double *) malloc(sizeof(double) * p * sectionSize);

    MPI_Gather(T, sectionSize, MPI_DOUBLE, final, sectionSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("###### STEADY STATE; t = %.1f\n", t);
        for (int k = 0; k < o; k++) {   // z
            printf("# z = %g k = %d\n", k * dl, k);
            for (int j = 0; j < m; j++) {   // y
                for (int i = 0; i < n; i++) {   // x
                    printf("%.1f ", final[k * n * m + j * n + i] - ZERO_CELSIUS_IN_KELVIN);
                }
                printf("\n");
            }
        }
        printf("\n");
        fprintf(stderr, "For graphical rendering: python3 rendu_picture_steady.py [filename.txt] %d %d %d\n", n, m, o);
    }

#endif
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
