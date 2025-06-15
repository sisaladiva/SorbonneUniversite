#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>

int main(void){
    #pragma omp parallel
        #ifdef _OPENMP
            printf("Hello world, thread %d/%d\n",omp_get_thread_num(),omp_get_num_threads());
        #else
            printf("Hello world\n");
        #endif
    
    return 0;
}