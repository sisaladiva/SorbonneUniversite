all : main main_with_gsl projet_GPU

main : main.c
	gcc -Wall -o main main.c

main_with_gsl : main_with_gsl.c
	gcc -Wall -o main_with_gsl main_with_gsl.c -lm -lgsl -lgslcblas

projet_CPU : projet_CPU.c
	gcc -Wall -o projet_CPU projet_CPU.c

projet_GPU : projet_GPU.cpp
	c++ -o projet_GPU projet_GPU.cpp -I/Users/samyhorchani/Desktop/Informatique/projet_CCA/libtorch/include -I/Users/samyhorchani/Desktop/Informatique/projet_CCA/libtorch/include/torch/csrc/api/include -L/Users/samyhorchani/Desktop/Informatique/projet_CCA/libtorch/lib -ltorch -ltorch_cpu -lc10 -Wl,-rpath,/Users/samyhorchani/Desktop/Informatique/projet_CCA/libtorch/lib


clean : 
	rm main main_with_gsl