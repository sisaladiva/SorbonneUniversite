

Multiplication de Polynômes et Implémentation FFT/IFFT

Ce programme en C implémente la multiplication de polynômes à l'aide de l'approche traditionnelle et de l'algorithme de Transformée de Fourier Rapide (FFT). Il démontre également les opérations FFT et IFFT sur des vecteurs complexes.

Compilation

Pour compiler le programme, utilisez un compilateur C tel que `gcc`. Ouvrez un terminal et naviguez jusqu'au répertoire contenant le fichier source (`fft.c`). Ensuite, exécutez la commande suivante :
gcc projetmodel.c -o projetmodel -lm
Oui bien simplement Make.
Exécution du Programme
Après la compilation, exécutez le programme avec la commande suivante :
./projetmodel

Notre code inclut une fonction (tempsexe) pour mesurer les temps d'exécution de la multiplication de polynômes avec des degrés variables. Les résultats sont écrits dans un fichier nommé times.txt.
