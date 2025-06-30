#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>
#include "func.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <time.h>


/*
*	Exercicio 4: COmutação das prioridades durante a execução das tasks
* 	Leonardo GOnçalves e Gonçalo Bastos 	UC-MEEC-STR
*/


#define S_TO_NS			1E9
#define MS_TO_NS		1E6
#define S_TO_MS         	1E3


#define T1 100000000L //100ms 
#define T2 200000000L //200ms 
#define T3 300000000L //300ms

#define Class 	1
#define Group 	5

#define WAIT_TIME 		1 //s -> Assegur que todas as threads vão começar a executar no mesmo instante
#define EXECUTION_TIME		6 //s  


#define N_Threads		3

#define POLICY		SCHED_FIFO

struct threadArgs{

	unsigned int f; 
	struct timespec period; 
	struct timespec start; 
	struct timespec finish; 
	int policy; 
};


struct thread_Val{
	
	unsigned int 	execution_expected; 
	unsigned int 	execution_done; 
	int 	*miss_deadline; 
	long double 	max_response_time; 
	long double 	min_response_time;
	long double 	response_time_jitter; 
	int 	n_miss_deadline; 
};


struct timespec timeSum(struct timespec a, struct timespec b); 
struct timespec timespecSet(int seconds, int nanoseconds); 
long double timespecInMs(struct timespec a);
int time_lower_cmp(struct timespec a, struct timespec b);
int time_lower_equal(struct timespec a, struct timespec b); 
int time_equal(struct timespec a, struct timespec b); 
struct timespec timeDiff(struct timespec start, struct timespec end) ; 


pthread_t threadID[N_Threads]; 
void* thread(void *arg); 
 

int main(int argc, char *argv[]){



	//Only use 1 CPU  
	cpu_set_t cpu; 
	CPU_ZERO(&cpu); 
	CPU_SET(0, &cpu); 
	
	if(sched_setaffinity(0, sizeof(cpu), &cpu)==-1){
		perror("CPU_ERROR"); 
		exit(EXIT_FAILURE);
	}
	if(mlockall(MCL_CURRENT | MCL_FUTURE) == -1 ){
		perror("mlock_error"); 
		exit(EXIT_FAILURE); 
	}
	
	struct threadArgs	    args[N_Threads];
	struct sched_param 	    param[N_Threads];
	pthread_attr_t		    attr[N_Threads];
	struct thread_Val         *retval[N_Threads];
	struct timespec             startTime, finishTime;
	
	int period[N_Threads]   = {T1, T2, T3};
	int i; 
	
	
	/* 
		Instante de inicio e de fim iguais para todas as thread
		definimos o tempo de inicio com o WAIT_TIME
		Vamos assegurar que todas as threads estao prontas a executar quando for o momento de executar 
	*/
	
	if(clock_gettime(CLOCK_MONOTONIC, &startTime) == -1){
		perror("clock_gettime error"); 
		exit(EXIT_FAILURE);
	} 
	
	startTime = timeSum(startTime, timespecSet((int)WAIT_TIME, 0)); 
	finishTime = timeSum(startTime, timespecSet((int)EXECUTION_TIME, 0)); 
	
	/* Precisamos de configurar as threads antes destas entrarem em execução
		Utilizamos os parametros de atributos das threads
	*/
	
	for(i=0; i < N_Threads; i++){
	
		//Parametros das threads 
		if(pthread_attr_init(&attr[i])!=0)
		{
		 	perror("pthread_attr_init"); 
		 	exit(EXIT_FAILURE);
		}
		if (pthread_attr_setaffinity_np(&(attr[i]), sizeof(cpu_set_t), &cpu) != 0){
			perror("pthread_attr_setaffinity_np"); 
		 	exit(EXIT_FAILURE); 
		}
		if (pthread_attr_setschedpolicy(&(attr[i]), POLICY) != 0)
		{
			perror("main->pthread_attr_setschedpolicy");
			exit(EXIT_FAILURE);
		}
		
		if (pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED) != 0)
		{
			perror("main->pthread_attr_setschedpolicy");
			exit(EXIT_FAILURE);
		}
		
		/* 
			Vamos definir a prrioridade de cada thread, indo a prioridade máxima permitida para a politica subtraindo o valor de cada thread permitindo diferentes prioridades  
		*/
		
		memset(&(param[i]), 0, sizeof(struct sched_param));
		
		if ((param[i].sched_priority = sched_get_priority_max(POLICY)) == -1)
		{
			perror("sched_priority");
			exit(EXIT_FAILURE);  
		}
		
		param[i].sched_priority -= i;	// subtrair o valor do i do ciclo 
		
		printf("Prioridade de f%d: %d\t\t\n", i+1, (int) param[i].sched_priority);
		
		
		if (pthread_attr_setschedparam(&(attr[i]), &(param[i])) != 0)
		{
			perror("main->pthread_attr_setschedparam");
			exit(EXIT_FAILURE); 
		}
		
		// Argumentos da thread 
		
		args[i].f       = i;
		args[i].period  = timespecSet(0, period[i]);
		args[i].start   = startTime;
		args[i].finish  = finishTime;
		args[i].policy	= (int)POLICY;
	} 
	
	
	
	for (i = 0; i < N_Threads; i++){
		
		if (pthread_create(&(threadID[i]), &(attr[i]), thread, &(args[i])) != 0)
			{
				perror("pthread_create");
				exit(EXIT_FAILURE); 
			}
	}
	
	for (i = 0; i < N_Threads; i++){
	
		if (pthread_join(threadID[i], (void *)&(retval[i])) != 0)
		{
			perror("main->pthread_join");
		}
		if (pthread_attr_destroy(&(attr[i])) != 0)
		{
			perror("main->pthread_attr_destroy");
			exit(EXIT_FAILURE);
		}
	}
	
	for(i = 0; i < N_Threads; i++){
	
		printf("Execução de  f%d:\n", i+1);
		if (retval[i] != NULL)
		{
			printf("Esperado: %d\tExecutado: %d\n", retval[i]->execution_expected, retval[i]->execution_done);
			printf("Maximo tempo de resposta:  %Lf(ms)\t Minimo tempo de resposta:  %Lf(ms)\t\n  Jitter do tempo de Resposta: %Lf(ms)\n ", retval[i]->max_response_time,retval[i]->min_response_time, retval[i]->response_time_jitter);
			printf("N de Metas falhadas: %d\n", retval[i]->n_miss_deadline); 
			printf("=================== RMPO | RMPO inverso ===================\n");
			for (int j = 0; j < retval[i]->execution_done || j < retval[i]->execution_expected; j++)
			{
				
				printf("No. %2d: D = %3d ms -> ", (int)j, period[i]/(int)MS_TO_NS);
				
				if (retval[i]->miss_deadline[j])
				{
					printf("Cumpriu a Meta");
				}
				else
				{
					printf("Nao cumpriu a Meta");
				}
				printf("\n"); 
			}
			
			free(retval[i]->miss_deadline);
			free(retval[i]);
		}
		
	}
	
	munlockall();
	exit(EXIT_SUCCESS);	
}




void* thread(void *arg)
{
	int                     i;
	struct timespec         nextTime, current;
	struct timespec 	max_time = {0}; 
	struct timespec		min_time = {0}; 
	struct sched_param 	    param;

	struct threadArgs       *args = (struct threadArgs *)arg;
	struct thread_Val     *retval = (struct thread_Val *)malloc(1*sizeof(struct thread_Val));
	
	struct timespec 	aux = {0}; 
	struct timespec 		current_aux = {0};
	
	long double current_time = 0; 

	if (retval == NULL)
	{
		perror("thread->malloc");
		exit(EXIT_FAILURE);
	}

	
	retval->execution_expected = (int)((float)EXECUTION_TIME / ((float)timespecInMs(args->period) / (float)S_TO_MS) + 0.5);
	retval->miss_deadline = (int *)malloc(retval->execution_expected * sizeof(int));
	
	if (retval->miss_deadline == NULL)
	{
		perror("thread->malloc");
		exit(EXIT_FAILURE); 
	}

	/*
	*  O primeiro tempo de execução é dado pela main para que todas as tarefas executem no mesmo instante
	*/
	nextTime = args->start;

	i = 0;
	while (time_lower_cmp(nextTime, args->finish))
	{
		/*
		* O sistema chama o "clock_nanosleep" que vai fazer com que a thread suspenda ate chegar o novo tempo de execução
		*/
		if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &nextTime, NULL) != 0)
		{
			perror("thread_f4->clock_nanosleep");
			exit(EXIT_FAILURE);
		}

		/*
		* 	Excuta a tareda, medindo o tempo de fim. 
		* 	Se for concluido antes do proximo periodo, então a meta foi cumprida! 
		*	Guardamos o resultado numa tabela booleana 
		*/
		if (args->f == 0)
		{
			f1(Class, Group);
		}
		else if (args->f == 1)
		{
			f2(Class, Group);
		}
		else if (args->f == 2)
		{
			f3(Class, Group);
		}
		if (clock_gettime(CLOCK_MONOTONIC, &current) == -1)
		{
			perror("thread->clock_gettime");
			exit(EXIT_FAILURE);
		}
		
		
		
		//calcular o maximo response time
		aux = timeDiff(nextTime, current); 
		if(time_lower_cmp(max_time, aux)){
			max_time = aux; 
		} 
		
		//calcular o minimo response time 
		if(i==0){
			min_time = aux;
		}
		if (time_lower_cmp(aux, min_time)) {
		    	min_time = aux;
		
		}
		
		nextTime = timeSum(nextTime, args->period);


		if (i < retval->execution_expected)
		{
			retval->miss_deadline[i] = time_lower_equal(current, nextTime);
			if(retval->miss_deadline[i] == 0){
				retval->n_miss_deadline += 1; 
			}
		}
		
		
		
		//Verificar Inversão das prioridades 
		
		aux = args->start; 
		current_aux = timeDiff(current, aux); 
		current_time = fabsl(timespecInMs(current_aux));
		//printf("%Lf\n", current_time); 
		if (current_time >= 1950.0 && current_time < 2000.0){
		
			//Alterar as prioridades 
			memset(&param, 0, sizeof(struct sched_param));
			if ((param.sched_priority = sched_get_priority_max(args->policy)) == -1)
			{
				perror("sched_get_priority_max");
				exit(EXIT_FAILURE);
			}
			
			if (pthread_setschedparam(threadID[2], args->policy, &param) != 0)
			{
				perror("pthread_setschedparam");
				exit(EXIT_FAILURE);
			}
			param.sched_priority-=2;
			if (pthread_setschedparam(threadID[0], args->policy, &param) != 0)
			{
				perror("pthread_setschedparam");
				exit(EXIT_FAILURE);
			}
		}else if(current_time >= 3950.0 && current_time < 4000.0){
		
			//Alterar as prioridades 
			memset(&param, 0, sizeof(struct sched_param));
			if ((param.sched_priority = sched_get_priority_max(args->policy)) == -1)
			{
				perror("sched_get_priority_max");
				exit(EXIT_FAILURE);
			}
			
			if (pthread_setschedparam(threadID[0], args->policy, &param) != 0)
			{
				perror("pthread_setschedparam");
				exit(EXIT_FAILURE);
			}
			param.sched_priority-=2;
			if (pthread_setschedparam(threadID[2], args->policy, &param) != 0)
			{
				perror("pthread_setschedparam");
				exit(EXIT_FAILURE);
			}
		}
		
		
		i++;

		}
		
	//Guardar o Maximo e o Minimo
	retval->max_response_time = timespecInMs(max_time);
	retval->min_response_time = timespecInMs(min_time); 
	
	
	//Calcular o response time jitter
	retval->response_time_jitter = fabsl(timespecInMs(timeDiff(max_time, min_time)));
	 

	retval->execution_done = i;

	pthread_exit((void *)retval);	
	
	
}



long double timespecInMs(struct timespec a) //return ms
{
	return (long double)(a.tv_sec * (long double)S_TO_MS + a.tv_nsec / (long double)MS_TO_NS);
}


int time_lower_cmp(struct timespec a, struct timespec b)
{
	/*Verificar se é menor*/
	if (a.tv_sec < b.tv_sec)
	{
		return 1;
	}
	else{
		if (a.tv_sec == b.tv_sec)
		{
			if (a.tv_nsec < b.tv_nsec)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
}


int time_lower_equal(struct timespec a, struct timespec b)
{
	/*Verificar se é menor ou igual*/
	return time_lower_cmp(a, b) || time_equal(a, b);
}

struct timespec timeSum(struct timespec a, struct timespec b)
{
	struct timespec result;

	result.tv_nsec = a.tv_nsec + b.tv_nsec;
	if (result.tv_nsec > (int)S_TO_NS-1)
	{
		result.tv_nsec = result.tv_nsec % (int)S_TO_NS;
		result.tv_sec = 1 + a.tv_sec + b.tv_sec;
	}
	else
	{
		result.tv_sec = a.tv_sec + b.tv_sec;
	}
	return result;
}

struct timespec timeDiff(struct timespec start, struct timespec end) {

    struct timespec result;

    if (end.tv_nsec < start.tv_nsec) {
        result.tv_sec = end.tv_sec - start.tv_sec - 1;
        result.tv_nsec = (int)S_TO_NS + end.tv_nsec - start.tv_nsec;
    } else {
        result.tv_sec = end.tv_sec - start.tv_sec;
        result.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return result;
}



struct timespec timespecSet(int seconds, int nanoseconds)
{
	struct timespec result;

	result.tv_sec = (time_t)(seconds + (int)(nanoseconds / (int)S_TO_NS));
	result.tv_nsec = (long)(nanoseconds % (int)S_TO_NS);

	return result;
}

int time_equal(struct timespec a, struct timespec b)
{
	/*verificar se é igual*/
	return a.tv_sec == b.tv_sec && a.tv_nsec == b.tv_nsec;
}

