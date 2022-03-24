#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// start flag 
volatile int start;

// flag semaphore
sem_t flag;
int bob = 0;

// The thread process
void* thread_routine(void* args)
{
	// wait for start from master thread
	while(!start);

	for (int j = 0; j < 10000; j++)
	{
		sem_post(&flag);
        bob++;
	}
	return NULL;
}


// Main process
int main(int argc, char* argv[]){

	sem_init(&flag, 0, 0);
	
	pthread_t thr1;
	pthread_t thr2;
	if(pthread_create(&thr1, NULL, thread_routine, NULL) == -1) {
		printf("COULD NOT CREATE A THREAD\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&thr2, NULL, thread_routine, NULL) == -1) {
		printf("COULD NOT CREATE A THREAD\n");
		exit(EXIT_FAILURE);
	}

	start = 1;

	pthread_join(thr1,NULL);
	pthread_join(thr2,NULL);

    int total = 0;
    sem_getvalue(&flag, &total);

	printf("Amount: %d\n", total);
    printf("bob: %d\n", bob);

	sem_destroy(&flag);
	return EXIT_SUCCESS;
}
