/**************************************************************
Lokman A. Abbas-Turki code

Those who re-use this code should mention in their code
the name of the author above.
***************************************************************/
#include "rng.h"
#include <time.h>


// Set the new RNG seed
void CMRG_set(int *a0, int *a1, int *a2, int *a3, int *a4, 
			         int *a5, int *CMRG_Out){
	CMRG_Out[0] = *a0;
	CMRG_Out[1] = *a1;
	CMRG_Out[2] = *a2;
	CMRG_Out[3] = *a3;
	CMRG_Out[4] = *a4;
	CMRG_Out[5] = *a5;
}

// Get the RNG Seed
void CMRG_get(int *a0, int *a1, int *a2, int *a3, int *a4, 
			         int *a5, int *CMRG_In){
	*a0 = CMRG_In[0];
	*a1 = CMRG_In[1];
	*a2 = CMRG_In[2];
	*a3 = CMRG_In[3];
	*a4 = CMRG_In[4];
	*a5 = CMRG_In[5];
}

// Generate uniformly distributed random variables
void CMRGg(int *a0, int *a1, int *a2, int *a3, int *a4, 
			     int *a5, float *g0, float *g1, int nb){

 const int m1 = 2147483647;// Requested for the simulation
 const int m2 = 2145483479;// Requested for the simulation
 int h, p12, p13, p21, p23, k, loc;// Requested local parameters

 for(k=0; k<nb; k++){
	 // First Component 
	 h = *a0/q13; 
	 p13 = a13*(h*q13-*a0)-h*r13;
	 h = *a1/q12; 
	 p12 = a12*(*a1-h*q12)-h*r12;

	 if (p13 < 0) {
	   p13 = p13 + m1;
	 }
	 if (p12 < 0) {
	   p12 = p12 + m1;
	 }
	 *a0 = *a1;
	 *a1 = *a2;
	 if( (p12 - p13) < 0){
	   *a2 = p12 - p13 + m1;  
	 } else {
	   *a2 = p12 - p13;
	 }
  
	 // Second Component 
	 h = *a3/q23; 
	 p23 = a23*(h*q23-*a3)-h*r23;
	 h = *a5/q21; 
	 p21 = a21*(*a5-h*q21)-h*r21;

	 if (p23 < 0){
	   p23 = p23 + m2;
	 }
	 if (p12 < 0){
	   p21 = p21 + m2;
	 }
	 *a3 = *a4;
	 *a4 = *a5;
	 if ( (p21 - p23) < 0) {
	   *a5 = p21 - p23 + m2;  
	 } else {
	   *a5 = p21 - p23;
	 }

	 // Combines the two MRGs
	 if(*a2 < *a5){
		loc = *a2 - *a5 + m1;
	 }else{loc = *a2 - *a5;} 

	 if(k){
		if(loc == 0){
			*g1 = Invmp*m1;
		}else{*g1 = Invmp*loc;}
	 }else{
		*g1 = 0.0f; 
		if(loc == 0){
			*g0 = Invmp*m1;
		}else{*g0 = Invmp*loc;}
	 }
  }
}


// Calling RNG
void RNGcall(int * a0, int * a1, int * a2, int * a3, int * a4, int * a5){

 float u0, u1;

 for (int i = 0; i < 100; i++) {
	 //CMRGg(a0, a1, a2, a3, a4, a5, &u0, &u1, 1); // one uniform 
	 CMRGg(a0, a1, a2, a3, a4, a5, &u0, &u1, 2); // two uniforms
 } 
	 
}


int main()
{

	int Ntraj = 1024*1024;

	PostInitDataCMRG();	
	
	float Tim;											// CPU timer instructions
	clock_t start, stop;								// CPU timer instructions
	
	int a0, a1, a2, a3, a4, a5;
	CMRG_get(&a0, &a1, &a2, &a3, &a4, &a5, CMRGp[0][0]);

	start = clock();
	
	for(int imc = 0; imc < Ntraj; imc++){
		RNGcall(&a0, &a1, &a2, &a3, &a4, &a5);
	}
	
	stop = clock();
	Tim = ((float)stop-(float)start)/(float)CLOCKS_PER_SEC;// CPU timer instructions

	printf("Execution time %f s\n", Tim);
		
	return 0;
}



