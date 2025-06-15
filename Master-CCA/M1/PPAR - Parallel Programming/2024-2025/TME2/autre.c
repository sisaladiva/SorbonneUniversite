#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "rasterfile.h"

double wallclock_time() {
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6);
}

unsigned char xy2color(double a, double b, int depth) {
    double x = 0, y = 0;
    for (int i = 0; i < depth; i++) {
        double temp = x;
        double x2 = x * x;
        double y2 = y * y;
        x = x2 - y2 + a;
        y = 2 * temp * y + b;
        if (x2 + y2 > 4.0) return (i % 255);  // Divergence
    }
    return 255;  // Non divergence
}

void save_rasterfile(char *name, int largeur, int hauteur, unsigned char *p) {
    FILE *fd = fopen(name, "w");
    if (fd == NULL) {
        perror("Error while opening output file.");
        exit(1);
    }
    struct rasterfile file;
    file.ras_magic = htonl(RAS_MAGIC);
    file.ras_width = htonl(largeur);
    file.ras_height = htonl(hauteur);
    file.ras_depth = htonl(8);
    file.ras_length = htonl(largeur * hauteur);
    file.ras_type = htonl(RT_STANDARD);
    file.ras_maptype = htonl(RMT_EQUAL_RGB);
    file.ras_maplength = htonl(256 * 3);
    fwrite(&file, sizeof(struct rasterfile), 1, fd);

    for (int i = 255; i >= 0; i--) {
        unsigned char o = cos_component(i, 13.0);
        fwrite(&o, sizeof(unsigned char), 1, fd);
    }
    for (int i = 255; i >= 0; i--) {
        unsigned char o = cos_component(i, 5.0);
        fwrite(&o, sizeof(unsigned char), 1, fd);
    }
    for (int i = 255; i >= 0; i--) {
        unsigned char o = cos_component(i + 10, 7.0);
        fwrite(&o, sizeof(unsigned char), 1, fd);
    }

    fwrite(p, largeur * hauteur, sizeof(unsigned char), fd);
    fclose(fd);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Récupérer le rang
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Récupérer le nombre de processus

    double xmin = -2, ymin = -2, xmax = 2, ymax = 2;
    int w = 3840, h = 3840, depth = 10000;

    if (argc > 1) w = atoi(argv[1]);
    if (argc > 2) h = atoi(argv[2]);
    if (argc > 3) xmin = atof(argv[3]);
    if (argc > 4) ymin = atof(argv[4]);
    if (argc > 5) xmax = atof(argv[5]);
    if (argc > 6) ymax = atof(argv[6]);
    if (argc > 7) depth = atoi(argv[7]);

    double xinc = (xmax - xmin) / (w - 1);
    double yinc = (ymax - ymin) / (h - 1);

    // Chaque processus calcule une portion de l'image
    int lines_per_process = h / size;
    int start_line = rank * lines_per_process;
    int end_line = (rank == size - 1) ? h : start_line + lines_per_process;

    unsigned char *local_image = malloc(w * (end_line - start_line));

    double y = ymin + start_line * yinc;
    for (int i = start_line; i < end_line; i++) {
        double x = xmin;
        for (int j = 0; j < w; j++) {
            local_image[(i - start_line) * w + j] = xy2color(x, y, depth);
            x += xinc;
        }
        y += yinc;
    }

    unsigned char *global_image = NULL;
    if (rank == 0) global_image = malloc(w * h);

    // Rassembler les parties calculées par chaque processus
    MPI_Gather(local_image, w * (end_line - start_line), MPI_UNSIGNED_CHAR,
               global_image, w * (end_line - start_line), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // Sauvegarde de l'image par le processus maître
    if (rank == 0) {
        save_rasterfile("mandel_mpi.ras", w, h, global_image);
        free(global_image);
    }

    free(local_image);

    MPI_Finalize();
    return 0;
}
