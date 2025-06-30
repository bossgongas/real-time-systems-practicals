#include "func2.h"

#define S_TO_NS			1E9

/*
*	Exercício 5: Criação da função objeto similar à função disponibilizada. Tivemos em conta o tempo de calculos determinados no exercício 1
*	Leonardo Gonçalves e Gonçalo Bastos UC-MEEC-STR
*/

int waitTime(); 
int time_lower(struct timespec a, struct timespec b);
struct timespec timesum(struct timespec a, struct timespec b);
struct timespec timespecset(int seconds, int nanoseconds);

void f1(int a, int b)
{
	while((waitTime(TIME_EXECUTION1))!=0); 
}

void f2(int a, int b)
{
	while((waitTime(TIME_EXECUTION2))!=0); 
}

void f3(int a, int b)
{
	while((waitTime(TIME_EXECUTION3))!=0); 
	
}

/*
*	This function makes busy waiting to emulate a task of real computation with
*	the same time of computation of such task.
*/
int waitTime(int time_ns)
{
	struct timespec current, finish;
	finish = timespecset(0, time_ns);
	if (clock_gettime(CLOCK_MONOTONIC, &current) == -1)
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	finish = timesum(current, finish);
	while (time_lower(current, finish))
	{
		if (clock_gettime(CLOCK_MONOTONIC, &current) == -1)
		{
			perror("waitTime->clock_gettime");
			exit(EXIT_FAILURE);
		}
	}
	
	return 0; 
}

int time_lower(struct timespec a, struct timespec b)
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

struct timespec timesum(struct timespec a, struct timespec b)
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

struct timespec timespecset(int seconds, int nanoseconds)
{
	struct timespec result;

	result.tv_sec = (time_t)(seconds + (int)(nanoseconds / (int)S_TO_NS));
	result.tv_nsec = (long)(nanoseconds % (int)S_TO_NS);

	return result;
}
