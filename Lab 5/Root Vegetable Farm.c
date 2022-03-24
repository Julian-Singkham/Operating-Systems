/**
  ***********************************************************************************
  * @file   : Root_Vegetable_Farm_MultiThreading.c
  * @brief  : Main program body
  *         : Lab 5: Root and Vegetable Farm
  *         : CS-3841/021 
  * @date   : OCT 21 2020
  * @author : Julian Singkham
  ***********************************************************************************
  * @attention
  *
  * This program simulates a farm that produces turnips and radishes. Each turnip/radish
  * field and customer is a thread. When the farm has finished producing all it can for
  * the season, the simulation ends.
  * 
  * See Main comment for instructions.
  *
  ***********************************************************************************
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>

//=====================================Structures=====================================

/*
 * Nodes for a circular linked list
 * Acts like a queue in this lab.
 * It is assumed that whenever a node is referenced, it's the last node.
 * Commented as LLQ
 */
typedef struct Node{
    unsigned long id;
    struct Node *next;
} Node;

//=====================================Variables======================================
sem_t turnip; //Counter for how many turnips are available
sem_t radish; //Counter for how many radishes are available
pthread_mutex_t customer_mutex = PTHREAD_MUTEX_INITIALIZER; //Protects customer LLQ
pthread_mutex_t turnip_mutex = PTHREAD_MUTEX_INITIALIZER;   //Protects total turnip count
pthread_mutex_t radish_mutex = PTHREAD_MUTEX_INITIALIZER;   //Protects total radish count
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;     //Protects initilization of customer LLQ
volatile int start = 0; //Start flag

volatile int total_turnip_consumed = 0;
volatile int total_radish_consumed = 0;
volatile int max_turnip_produced = 0;
volatile int max_radish_produced = 0;
volatile int total_turnip_produced = 0;
volatile int total_radish_produced = 0;
volatile int turnip_field_done = 0; //Used to tell when all the turnip fields are finished. 0 = all done
volatile int radish_field_done = 0; //Used to tell when all the radish fields are finished. 0 = all done

volatile int list_size = 0; //Size of customer LLQ
volatile int customer_ready = 0; //Used to tell when all the customers are ready. 0 = all ready
volatile Node *customer = NULL;  //Used to keep track which customer is last in line

//======================================Methods=======================================
/**
  * @brief Adds a node to an empty LLQ
  * 
  * @param Last: Pointer to the last node in the LLQ. Last (known as customer in Main)
  * is never malloc'd as its only used to keep track of the last in line. Temp is technically
  * where the actual customer is. This was done to greatly simplify LLQ book keeping and prevent
  * memory leaks.
  * @param id: Thread ID of the customer
  * 
  * @retval The last node in the LLQ
  */
Node *add_To_Empty(Node *last, unsigned long id){
	if (list_size != 0){
	    return last;
    }

	struct Node *temp = (Node*)malloc(sizeof(Node)); //The actual customer

	temp->id = id;
	last = temp;
	last->next = last;
    list_size++;
	return last;
}

/**
  * @brief Adds a node to the end of the LLQ
  * 
  * @param Last: Pointer to the last node in the LLQ.
  * @param id: Thread ID of the customer
  * 
  * @retval The last node in the LLQ
  */
Node *add_To_End(Node *last, unsigned long id){
	if (list_size == 0){
        return add_To_Empty(last, id);
    }
	
	Node *temp = (Node*)malloc(sizeof(Node));

	temp->id = id;
	temp->next = last->next; //Temp points to head
	last->next = temp;       //Last becomes 2nd to last node
	last = temp;             //Last is now Temp

    list_size++;
	return last;
}

/**
  * @brief Returns the (thread) ID of the first customer in line
  * 
  * @param Last: Pointer to the last node in the LLQ.
  * 
  * @retval The (thread) ID of the first customer in line
  */
unsigned long get_Head_ID(Node *last){
	Node *temp;

	if (list_size == 0){
		printf("List is empty.\n");
		return -1;
	}

	temp = last -> next; //Point to first Node of the list.

    return(temp->id);
}

/**
  * @brief Frees the customer LLQ
  * 
  * @param Last: Pointer to the last node in the LLQ.
  * 
  * @retval NONE
  */
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

/**
  * @brief Frees the first node
  * 
  * @param Last: Pointer to the last node in the LLQ.
  * 
  * @retval NONE
  */
void free_head(Node *last){
    if(list_size == 0){
        printf("The list is empty.\n");
        return;
    }
    Node *temp;
    temp = last->next; //Temp becomes head
    last->next = temp->next; //Last links to 2nd in line
    list_size--;
    free(temp);
}

/**
  * @brief Turnip Producer Routine (thread)
  * 
  * @param args: Pointer to the given arguements
  * 
  * @retval NONE
  */
void* turnip_thread(void* args){
    int *input = args;
    int temp = 0;
	while(!start); //Wait for start from master thread

	for(int i = 0; i < input[1]; i++) {
        pthread_mutex_lock(&turnip_mutex);
		sem_post(&turnip);
        sem_getvalue(&turnip, &temp);
        if(temp > max_turnip_produced){
            max_turnip_produced = temp;
        }
        total_turnip_produced++;
        pthread_mutex_unlock(&turnip_mutex);

		usleep(input[0]);
	}
    pthread_mutex_lock(&turnip_mutex);
    turnip_field_done--; //Notify that this thread is done
    pthread_mutex_unlock(&turnip_mutex);
	return NULL;
}

/**
  * @brief Radish Producer Routine (thread)
  * 
  * @param args: Pointer to the given arguements
  * 
  * @retval NONE
  */
void* radish_thread(void* args){
    int *input = args;
    int temp = 0;
	while(!start); //Wait for start from master thread

	for(int i = 0; i < input[1]; i++) {
        pthread_mutex_lock(&radish_mutex);
		sem_post(&radish);
        sem_getvalue(&radish, &temp);
        if(temp > max_radish_produced){
            max_radish_produced = temp;
        }
        total_radish_produced++;
        pthread_mutex_unlock(&radish_mutex);

		usleep(input[0]);
	}
    pthread_mutex_lock(&radish_mutex);
    radish_field_done--; //Notify that this thread is done
    pthread_mutex_unlock(&radish_mutex);
	return NULL;
}

/**
  * @brief Customer Routine (thread)
  * 
  * @param args: Pointer to the given arguements
  *     args[0] = # of turnips to buy
  *     args[1] = # of radishes to buy
  *     args[2] = cooldown after completeing order   
  * 
  * @retval How many orders were fullfilled
  */
void* customer_thread(void* args){
    int *input = args;
    int amount_turnip; //Amount ready to buy
    int amount_radish; //Amount ready to buy
    int order_filled = 0;
    pthread_mutex_lock(&init_mutex);
    customer = add_To_End(customer, pthread_self());
    customer_ready++;
    pthread_mutex_unlock(&init_mutex);
	
    //Wait for start from master thread
	while(!start); 
    //Wait until all customers are ready
    while(turnip_field_done != 0 || radish_field_done != 0){ 
        pthread_mutex_lock(&customer_mutex);
        //Wait until its the customer's turn
        if(pthread_self() == get_Head_ID(customer)){ 
            sem_getvalue(&turnip, &amount_turnip);
            sem_getvalue(&radish, &amount_radish);
            //Wait until entire order can be fullfilled
            if(amount_turnip >= input[0] && amount_radish >= input[1]){
                int i = 0;
                while(i < input[0]){
                    sem_wait(&turnip);
                    total_turnip_consumed++;
                    i++;
                }
                int j = 0;
                while(j < input[1]){
                    sem_wait(&radish);
                    total_radish_consumed++;
                    j++;
                }
                order_filled++;
                free_head(customer);
                pthread_mutex_unlock(&customer_mutex);

                usleep(input[1]);

                pthread_mutex_lock(&customer_mutex);
                customer = add_To_End(customer, pthread_self());
            }
        }
        pthread_mutex_unlock(&customer_mutex);
        sem_getvalue(&turnip, &amount_turnip);
        sem_getvalue(&radish, &amount_radish);
    }
    pthread_exit((void *) order_filled);
}

/**
  * @brief Simulates a root and vegetable farm with customers
  * 
  * @param args: Pointer to the given arguements
  *     args[1] = Name of settings file
  * @param Settings_File_Format 
  *     Line 1: (# of turnip fields) (Time to grow) (# to be produced for the season)
  *     Line 2: (# of radish fields) (Time to grow) (# to be produced for the season)
  *     Line 3: (# of customers)
  *     Line x: (# Turnips to buy) (# Radishes to buy) (Cooldown between order fullfillment)
  * 
  * @retval NONE
  */
int main(int argc, char* argv[]){
    FILE *file = fopen(argv[1], "r");
    if (file==NULL){ 
        printf("No such file.\n"); 
        return 0; 
    }
    printf("The Root & Vegetable Farm\n");

    sem_init(&turnip, 0, 0);
    sem_init(&radish, 0, 0);

    int turnip_field_amount;
    int turnip_parameters[2];
    fscanf(file, "%d ", &turnip_field_amount);
    fscanf(file, "%d %d", &turnip_parameters[0], &turnip_parameters[1]);
    printf("Turnip Fields: %d Time to grow: %d Total production: %d\n", turnip_field_amount, turnip_parameters[0], turnip_parameters[1]);
    turnip_field_done = turnip_field_amount;

    //Turnip producer initilization
    pthread_t tur[turnip_field_amount];
    for(int i = 0; i < turnip_field_amount; i++) {
        if(pthread_create(&tur[i], NULL, turnip_thread, (void*)turnip_parameters) == -1) {
            printf("COULD NOT CREATE TURNIP FIELD\n");
            exit(EXIT_FAILURE);
        }
	}

    int radish_field_amount;
    int radish_parameters[2];
    fscanf(file, "%d ", &radish_field_amount);
    fscanf(file, "%d %d", &radish_parameters[0], &radish_parameters[1]);
    printf("Radish Fields: %d Time to grow: %d Total production: %d\n", radish_field_amount, radish_parameters[0], radish_parameters[1]);
    radish_field_done = radish_field_amount;

    //Radish producer initilization
    pthread_t rad[radish_field_amount];
    for(int i = 0; i < radish_field_amount; i++) {
        if(pthread_create(&rad[i], NULL, radish_thread, (void*)radish_parameters) == -1) {
            printf("COULD NOT CREATE RADISH FIELD\n");
            exit(EXIT_FAILURE);
        }
	}

    int customer_amount = 0;
    fscanf(file, "%d ", &customer_amount);
    void *customer_return_value[customer_amount]; //Each element is a return value from customer_thread
    int customer_parameters[customer_amount][3]; //Each row is a customer

    //Customer initilization
    pthread_t cons[customer_amount];
    for(int i = 0; i < customer_amount; i++) {
        fscanf(file, "%d %d %d", &customer_parameters[i][0], &customer_parameters[i][1], &customer_parameters[i][2]);
        printf("Customer %d Turnips: %d Radishes: %d Wait: %d\n", i, customer_parameters[i][0], customer_parameters[i][1], customer_parameters[i][2]);
        if(pthread_create(&cons[i], NULL, customer_thread, (void*)customer_parameters[i]) == -1) {
            printf("COULD NOT CREATE CUSTOMER\n");
            exit(EXIT_FAILURE);
        }
	}
    
    //Wait until all customers are ready
    while(customer_ready != customer_amount); 
    start = 1;

    for(int i = 0; i < turnip_field_amount; i++) {
		pthread_join(tur[i], NULL);
	}
    for(int i = 0; i < radish_field_amount; i++) {
		pthread_join(rad[i], NULL);
	}
    printf("\nSimulation Finished\n");
    printf("Max turnip in the bin: %d\n", max_turnip_produced);
    printf("Max radish in the bin: %d\n", max_radish_produced);
    for(int i = 0; i < customer_amount; i++) {
		pthread_join(cons[i], &customer_return_value[i]);
        printf("Customer %d got their order filled %d times\n", i, (int *)customer_return_value[i]);
	}

    int turnip_left;
    int radish_left;
    sem_getvalue(&turnip, &turnip_left);
    sem_getvalue(&radish, &radish_left);

    printf("Turnips  - Produced: %d Bought: %d Remaining: %d \n", total_turnip_produced, total_turnip_consumed,turnip_left);
    printf("Radishes - Produced: %d Bought: %d Remaining: %d \n", total_radish_produced, total_radish_consumed,radish_left);

    free_list(customer);
    sem_destroy(&turnip);
    sem_destroy(&radish);
    pthread_mutex_destroy(&customer_mutex);
    pthread_mutex_destroy(&turnip_mutex);
    pthread_mutex_destroy(&radish_mutex);
	return 0;
}