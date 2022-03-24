#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct node {
	int val;
	struct node* next;
} node;

typedef struct list {
	node* head;
} list;

// shared global
static list mylist;

// start flag
volatile int start = 0;

// The thread process
void* thread_routine(void* args)
{
	printf("Worker thread: %lu ready\n", pthread_self());
	
	// wait for start from master thread
	while(start == 0);

	for (int j = 0; j < 1000000; j++)
	{
		node* new_node = malloc(sizeof(node));
		new_node->val = j;
		new_node->next = mylist.head;
		mylist.head = new_node;
	}

	printf("Worker thread: %lu done\n", pthread_self());

	return NULL;
}

// Main process
int main(int argc, char* argv[])
{
	#define THREAD_COUNT 10
	pthread_t thr_ids[THREAD_COUNT];

	mylist.head = NULL;
	
	for(int i = 0; i < THREAD_COUNT; i++) {
		if(pthread_create(&thr_ids[i], NULL, thread_routine, NULL) == -1) {
			printf("COULD NOT CREATE A THREAD\n");
			exit(EXIT_FAILURE);
		}
	}

	start = 1;

	for(int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(thr_ids[i],NULL);
	}

	int length = 0;
	node* itr = mylist.head;
	while(itr != NULL) {
		length++;
		itr = itr->next;
	}
	printf("List contains %d elements\n", length);

	// MEMORY LEAK HERE: TODO FIX

	return EXIT_SUCCESS;
}
