#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

typedef struct Node{
    unsigned long id;
    int time;
    int want;
    struct Node *next;
} Node;

sem_t carrot;
pthread_mutex_t element_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t customer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int start = 0; // Start flag

int amount_carrot;
int list_size = 0;
int total_consumed = 0;
int customer_ready = 0;
Node *customer = NULL;
int season_flag = 1;

Node *add_To_Empty(Node *last, unsigned long id, int want, int time){
	//Check to see if the list is empty
	if (list_size != 0){
	    return last;
    }

	struct Node *temp = (Node*)malloc(sizeof(Node));

	temp->id = id;
    temp->want = want;
    temp->time = time;
	last = temp;
	last->next = last;
    list_size++;
	return last;
}

Node *add_To_End(Node *last, unsigned long id, int want, int time){
    //Checks to see if list has been initialized
	if (list_size == 0){
        return add_To_Empty(last, id, want, time);
    }
	
	Node *temp = (Node*)malloc(sizeof(Node));

	temp->id = id;
    temp->want = want;
    temp->time = time;
	temp->next = last->next;
	last->next = temp;
	last = temp;

    list_size++;
	return last;
}

unsigned long get_Head_ID(Node *last){
	Node *temp;

	if (list_size == 0){
		printf("List is empty.\n");
		return -1;
	}

	temp = last -> next; //Point to first Node of the list.

    return(temp->id);
}

void free_list(Node *last){
    Node *temp;
    if(list_size == 0){
        printf("The list is empty.\n");
        return;
    }
    while(list_size != 0){
        temp = last->next; //Temp becomes head
        last->next = temp->next; //Last links to 2nd in line
        free(temp);
        list_size--;
    }
}

void free_head(Node *last){
    if(list_size == 0){
        printf("The list is empty.\n");
        return;
    }
    //printf("Head list size: %d", list_size);
    Node *temp;
    temp = last->next; //Temp becomes head
    last->next = temp->next; //Last links to 2nd in line
    list_size--;
    free(temp);
}

// customer Routine
void* customer_thread(void* args){
    int *input = args;
    int consumed = 0;
    pthread_mutex_lock(&init_mutex);
    customer = add_To_End(customer, pthread_self(), input[0], input[1]);
    customer_ready++;
    pthread_mutex_unlock(&init_mutex);
	
	while(!start); // wait for start from master thread
    while(season_flag){ //replace with season flag
        pthread_mutex_lock(&customer_mutex);
        //printf("Head ID: %lu\n", get_Head_ID(customer));
        if(pthread_self() == get_Head_ID(customer)){
            sem_getvalue(&carrot, &amount_carrot);
            if(amount_carrot >= input[0]){
                int i = 0;
                while(i < input[0]){
                    sem_wait(&carrot);
                    total_consumed++;
                    consumed++;
                    i++;
                    //printf("%d\n", i);
                }
                printf("%lu just bought %d\n", pthread_self(), i);
                free_head(customer);
                pthread_mutex_unlock(&customer_mutex);

                usleep(input[1]);

                pthread_mutex_lock(&customer_mutex);
                customer = add_To_End(customer, pthread_self(), input[0], input[1]);
            }
            else{
                season_flag = 0;
            }
        }
        pthread_mutex_unlock(&customer_mutex);
        sem_getvalue(&carrot, &amount_carrot);
    }
    //printf("Carrots left: %d\n", amount_carrot);
    //printf("%lu bought %d\n", pthread_self(), consumed);
    pthread_exit((void *) consumed);
}


int main(int argc, char* argv[]){
    FILE *file = fopen(argv[1], "r");
    if (file==NULL){ 
        printf("No such file.\n"); 
        return 0; 
    }

    sem_init(&carrot, 0, 100);
    sem_getvalue(&carrot, &amount_carrot);
    printf("Starting carrot amount: %d\n", amount_carrot); // delete

    int customer_amount = 0;
    fscanf(file, "%d ", &customer_amount); //How many customers
    void *return_value[customer_amount];
    int customer_parameters[customer_amount][2]; //Each row is a customer

    //customer initilization
    pthread_t cons[customer_amount];
    for(int i = 0; i < customer_amount; i++) {
        fscanf(file, "%d %d", &customer_parameters[i][0], &customer_parameters[i][1]);
        if(pthread_create(&cons[i], NULL, customer_thread, (void*)customer_parameters[i]) == -1) {
            printf("COULD NOT CREATE customer\n");
            exit(EXIT_FAILURE);
        }
	}
    
    while(customer_ready != customer_amount);
    start = 1;
    for(int i = 0; i < customer_amount; i++) {
		pthread_join(cons[i], &return_value[i]);
        printf("Customer %d Bought: %d\n", i, (int *)return_value[i]);
	}
    printf("Total Consumed: %d\n", total_consumed);

    free_list(customer);
    sem_destroy(&carrot);
    pthread_mutex_destroy(&element_mutex);
    pthread_mutex_destroy(&customer_mutex);
	return 0;
}