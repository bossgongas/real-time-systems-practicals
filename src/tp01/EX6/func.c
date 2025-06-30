#define _GNU_SOURCE

// Inclusão de bibliotecas
#include <stdio.h>
#include <sys/mman.h> //POSIX real time extension
#include <sched.h>
#include "func.h"

// Assinatura de funções (opcional)
// Declarações de variáveis globais (opcional)

// Função principal (main)
int main(int argc, char * argv[]) {
	
	//variaveis 
	struct timespec start_time, end_time;
	double elapsed_time;
	
	//set the processor to run in a single processor
	cpu_set_t cpuset;
    	CPU_ZERO(&cpuset);
    	CPU_SET(0, &cpuset); // Set affinity to CPU core 0
    	sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    	
    	for(int i = 1; i<=3; i++){
    		// Measure start time
    		clock_gettime(CLOCK_MONOTONIC, &start_time);
    		
    		if (i == 1){
    			// Call the function you want to measure
    			f1(1,5);	
    		}
    		else if(i==2){
    			f2(1,5);
    		}
    		else if(i==3){
    			f3(1,5);
    		}
    		
    		// Measure end time
    		clock_gettime(CLOCK_MONOTONIC, &end_time);
    		
    		// Calculate the elapsed time in milliseconds
    		elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + 
                         (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
        	
        	// Print the computation time
    		printf("Computation time of f%d: %.2f milliseconds\n", i,elapsed_time);
    	}
    	    	
    	return 0; // Opcional: retorna um valor para o sistema operacional
}



