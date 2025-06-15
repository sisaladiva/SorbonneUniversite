#!/bin/bash
#OAR -l walltime=1:00:00
#OAR -n hello_job
#OAR -d /path/to/working/directory
#OAR -M Assia.Mastor@gmail.com
#OAR -m bea

# Messages de début et de fin d'exécution
echo "Début de l'exécution" > debug.txt

# Exécution du programme
./hello > tata.txt 2>> debug.txt

echo "Fin de l'exécution" >> debug.txt

