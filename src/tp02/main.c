#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "header.h"
#include <time.h>
#include <pthread.h>//part 2
#include <semaphore.h>

#define FILES_NUM 3
#define THREAD_NUM 3

int program_finalize = 0;

//definição de semaforos
sem_t lock1; 
sem_t lock2; 

//dados que vão ser acedidos pelas threads
Coord* PointCloud1;//buffer 1
Coord* PointCloud2;//buffer 2
Coord* PointCloud3;//buffer 3
Analise Stats1;
Analise Stats2;
Analise Stats3;
int file_num = 0;
const char* filenames[3] = {"point_cloud1.txt","point_cloud2.txt","point_cloud3.txt"}; 
struct timespec periodo;
struct timespec start, end;



void * thread1_function(void* args){

	//int count = 0;
	periodo.tv_sec = 0.1;
	//periodo.tv_nsec= 100000000;
	
	//while(count < 3){ 
		
		//loop para iterar entre todos os ficehiros
		for(int i=0; i<FILES_NUM; i++){

			//timing
            		clock_gettime(CLOCK_MONOTONIC, &end);
			double elapsed = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
			if(i != 0){
			    printf("Consecutive Run of the first task: %lf\n", elapsed);
			}
			    clock_gettime(CLOCK_MONOTONIC, &start);


			//ler dados para as estruturas
			readPointCloud(filenames[i], &(Stats1), &PointCloud1);
			Stats2.point_count = Stats1.point_count;
		    	if (PointCloud1 != NULL) {
				printf("Ficheiro %d. Leitura com sucesso pela thread %d!\n",file_num, *(int*)args);
				printf("Temos agora %d dados\n", Stats1.point_count);
				//printf("Depois readPointCloud:   PointCloud1->%p\n", PointCloud1);
			}
				
			//Periodo de 100ms
		    clock_nanosleep(CLOCK_MONOTONIC, 0, &periodo, NULL);
			
			sem_post(&lock1); 	
			
		    	
		}
		//count++;
	//}
	program_finalize = 1;
		
}
    
void * thread2_function(void* args){
	//wait for the signal 
	while(program_finalize != 1){
		
		if(sem_wait(&lock1)==0){
			//ler pre processar os dados da estrutura 1, e escrever para a 2
			int point_count = Stats2.point_count;
			preProcessPointCloud(&PointCloud1,&PointCloud2, &point_count);
			Stats3.point_count = point_count;
			printf("Ficheiro %d. Cloud preprocessada pela thread %d!!\n",file_num,*(int*)args);
		    	printf("Temos agora %d dados\n", point_count);
			//printf("Depois de preProcessPointCloud:   PointCloud1->%p  |  PointCloud2->%p\n",PointCloud1,PointCloud2);
		    	
		    	//ativação do semaforo
		    	sem_post(&lock2); 
	    	}
	
	}
	
}

void * thread3_function(void* args){
	while(program_finalize != 1){
		
		if(sem_wait(&lock2)==0){
			//ler e processar os dados da estrutura 2, e escrever para a 3
			int point_count = Stats3.point_count;
			identifyDrivableArea(&PointCloud2, &PointCloud3, &point_count);
			printf("Ficheiro %d. Drivable Area encontrada pela thread %d!!\n",file_num,*(int*)args);
		    	printf("Temos agora %d dados\n", point_count);
			//printf("Depois de DrivableArea:   PointCloud2->%p  |  PointCloud3->%p\n", PointCloud2,PointCloud3);

			//Atualizar o numero do ficheiro
			file_num++;
			if(file_num == 3) {
				file_num =0;
			}

	    	}
	
    }
}

int main(int argc, char*argv[]) {
 	
 	PointCloud1 = malloc(1*sizeof(Coord));
 	PointCloud2 = malloc(1*sizeof(Coord));
 	PointCloud3 = malloc(1*sizeof(Coord));


 	if(PointCloud1 == NULL || PointCloud1 == NULL || PointCloud1 == NULL){
 		perror("Falha na alocação inicial!\n");
 		return 0;
	}
		    
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	int arg1 = 1, arg2 = 2, arg3 = 3;
	
	sem_init(&lock1, 0, 0); 
	sem_init(&lock2, 0, 0); 
	
	pthread_create(&thread1, NULL, thread1_function, (void *)&arg1);
	pthread_create(&thread2, NULL, thread2_function, (void *)&arg2);
	pthread_create(&thread3, NULL, thread3_function, (void *)&arg3);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	
	if (PointCloud1 != NULL) {
    free(PointCloud1);
    PointCloud1 = NULL;
	}
	if (PointCloud2 != NULL) {
    free(PointCloud2);
    PointCloud2 = NULL;
	}
	if (PointCloud2 != NULL) {
    free(PointCloud2);
    PointCloud2 = NULL;
	}

	sem_destroy(&lock1);
	sem_destroy(&lock2);

    return 0;
}





