/**
  ***********************************************************************************
  * @file   : memory_manager.c
  * @brief  : Main program body
  *         : Lab 6: Memory Manager
  *         : CS-3841/021 
  * @date   : OCT 27 2020
  * @author : Austin Grenert & Julian Singkham
  * 		: Outline By Prof Lembke
  ***********************************************************************************
  * @attention
  *
  * This program handles memory allocation and removal.
  *
  ***********************************************************************************
**/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "memory_manager.h"
#include "linked_list.h"

// CONSTANTS
#define FREE 1
#define NOT_FREE 0

/* 
 * Using static causes the compiler to
 * limit visibility of the varibles to this file only
 * This can be used to simulate 'private' variables in c
 */
static int allocation_count = 0;

/*
 * Keeps track of the amount of fragments in the memory manager
 */
static int fragment_count = 0;

/*
 * Keeps track of the amount of bytes currently allocated
 */
static int allocated_space = 0;

/*
 * Holds the value for the maximum amount of memory (in bytes)
 * that can be allocated
 * Is set in the init function for the memory manager
 */
static int max_size = 0;

/*
 * Holds the status for whether or not the list is initialized and ready
 */
static int mem_status = 0;

/*
 * Protects memory functions.
 */
static pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Creates the linked list that will be used to manage memory
 * Linked list is static and should not be seen by code that uses
 * the memory manager
 */
static linked_list memList;

/* TODO Define additional structure definitions here */

/* mmInit()
 *     Initialize the memory manager to "manage" the given location
 *     in memory with the specified size.
 *         Parameters: start - the start of the memory to manage
 *                     size - the size of the memory to manage
 *         Returns: void
 */ 
void mmInit(void* start, int size)  {
	allocation_count = 0;
	max_size = size;
	fragment_count = 1;

	// Initialize the linked list
	linked_list_init(&memList);

	// Create free memory of the size specified
	linked_list_add_front(&memList, start, size, FREE);

	// update manager status
	mem_status = 1;
}

/* mmDestroy()
 *     Cleans up any storage used by the memory manager
 *     After a call to mmDestroy:
 *         all allocated spaces become invalid
 *         future allocation attempts fail
 *         future frees result in segmentation faults
 *     NOTE: after a call to mmDestroy a call to mmInit
 *           reinitializes the memory manager to allow allocations
 *           and frees
 *         Parameters: None
 *         Returns: void
 */ 
void mmDestroy() {
	// destroys all of the remaining entried of the linked list holding the memory
	linked_list_destroy(&memList);

	// Update variables and status
	mem_status = 0;
	max_size = 0;
	allocated_space = 0;
	allocation_count = 0;
	fragment_count = 0;
}

/* mymalloc_ff()
 *     Requests a block of memory be allocated using 
 *         first fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_ff(int nbytes) {
	// If the memory manager is not initialized, return NULL pointer
	pthread_mutex_lock(&mem_mutex);
	if (mem_status == 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// Create pointer for new allocation
	void* allocation;
	
	// Keeps track of index with largest free block available that can satisfy request
	int firstIndex = -1;

	// Keeps a reference to the lowest node
	node* firstNode;

	// Keeps track of first fit number
	int firstNumber = 0;

	// creates an iterator to iterate through entire list
	int currentIndex = 0;
	node* iter = memList.front;

	// iterates through entire list
	while (iter != NULL && !firstNumber) {
		// If block is a free block
		if (iter->is_free) {
			// If free block is big enough
			if (iter->data_size >= nbytes) {
				firstNumber = iter->data_size;
				firstNode = iter;
				firstIndex = currentIndex;
			}
		}
		currentIndex++;
		iter = iter->next;
	} 

	// If no such node is found, return nothing
	if (firstIndex < 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// Else allocate new block of memory (currently not using the heap)
	// allocation = malloc(nbytes);

	// set data address to beginning of previously free block
	allocation = firstNode->data;

	// Insert new node at index found
	linked_list_add(&memList, allocation, nbytes, firstIndex, NOT_FREE);

	// If the free space exactly equals the number of bytes, remove the free block
	if (firstNode->data_size == nbytes) {
		linked_list_remove(&memList, firstIndex + 1);
		fragment_count--;
	}

	// Otherwise reduce the amount of free space in the block
	else {
		firstNode->data_size -= nbytes;
		firstNode->data = allocation + nbytes;
	}

	// Update stats
	allocation_count++;
	allocated_space += nbytes;

	if (get_remaining_space() == 0) {
		fragment_count = 0;
	}

	// return the pointer to the allocated memory
	pthread_mutex_unlock(&mem_mutex);
	return allocation;
}

/* mymalloc_wf()
 *     Requests a block of memory be allocated using 
 *         worst fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_wf(int nbytes) {
	// If the memory manager is not initialized, return NULL pointer
	pthread_mutex_lock(&mem_mutex);
	if (mem_status == 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// Create pointer for new allocation
	void* allocation;
	
	// Navigate to the node with the largest free block that is larger or equal to nbytes
	
	// Keeps track of index with largest free block available that can satisfy request
	int largestIndex = -1;

	// Keeps a reference to the largest node
	node* worstNode;

	// Keeps track of worst fit number
	int largestNumber = 0;

	// keeps track of how many free blocks can satisfy request
	int numFreeBlocks = 0;

	// creates an iterator to iterate through entire list
	int currentIndex = 0;
	node* iter = memList.front;

	// iterates through entire list
	while (iter != NULL) {
		// If block is a free block
		if (iter->is_free) {
			// If free block is big enough
			if (iter->data_size >= nbytes) {
				// If first free block found
				if (numFreeBlocks == 0) {
					// Initialize smallest index
					largestNumber = iter->data_size;
					largestIndex = currentIndex;
					worstNode = iter;
				} 
				
				// If not the first free block found
				else {
					// Compare with largest index
					if (iter->data_size > largestNumber) {
						largestNumber = iter->data_size;
						largestIndex = currentIndex;
						worstNode = iter;
					}
				}
				numFreeBlocks++;
			}
		}
		currentIndex++;
		iter = iter->next;
	} 

	// If no such node is found, return nothing
	if (largestIndex < 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// set data address to beginning of previously free block
	allocation = worstNode->data;

	// Insert new node at index found
	linked_list_add(&memList, allocation, nbytes, largestIndex, NOT_FREE);

	// If the free space exactly equals the number of bytes, remove the free block
	if (worstNode->data_size == nbytes) {
		linked_list_remove(&memList, largestIndex + 1);
		fragment_count--;
	}

	// Otherwise reduce the amount of free space in the block
	else {
		worstNode->data_size -= nbytes;
		worstNode->data = allocation + nbytes;
	}

	// Update stats
	allocation_count++;
	allocated_space += nbytes;

	if (get_remaining_space() == 0) {
		fragment_count = 0;
	}

	// return the pointer to the allocated memory
	pthread_mutex_unlock(&mem_mutex);
	return allocation;
}

/* mymalloc_bf()
 *     Requests a block of memory be allocated using 
 *         best fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_bf(int nbytes) {
	// If the memory manager is not initialized, return NULL pointer
	pthread_mutex_lock(&mem_mutex);
	if (mem_status == 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// Create pointer for new allocation
	void* allocation;
	
	// Navigate to the node with the smallest free block that is larger or equal to nbytes
	
	// Keeps track of index with smallest free block available that can satisfy request
	int smallestIndex = -1;

	// Keeps a reference to the lowest node
	node* bestNode;

	// Keeps track of best fit number
	int lowestNumber = 0;

	// keeps track of how many free blocks can satisfy request
	int numFreeBlocks = 0;

	// creates an iterator to iterate through entire list
	int currentIndex = 0;
	node* iter = memList.front;

	// iterates through entire list
	while (iter != NULL) {
		// If block is a free block
		if (iter->is_free) {
			// If free block is big enough
			if (iter->data_size >= nbytes) {
				// If first free block found
				if (numFreeBlocks == 0) {
					// Initialize smallest index
					lowestNumber = iter->data_size;
					smallestIndex = currentIndex;
					bestNode = iter;
				} 
				
				// If not the first free block found
				else {
					// Compare with smallest index
					if (iter->data_size < lowestNumber) {
						lowestNumber = iter->data_size;
						smallestIndex = currentIndex;
						bestNode = iter;
					}
				}
				numFreeBlocks++;
			}
		}
		currentIndex++;
		iter = iter->next;
	} 

	// If no such node is found, return nothing
	if (smallestIndex < 0) {
		pthread_mutex_unlock(&mem_mutex);
		return NULL;
	}

	// Else allocate new block of memory (currently not using the heap)
	// allocation = malloc(nbytes);

	// set data address to beginning of previously free block
	allocation = bestNode->data;

	// Insert new node at index found
	linked_list_add(&memList, allocation, nbytes, smallestIndex, NOT_FREE);

	// If the free space exactly equals the number of bytes, remove the free block
	if (bestNode->data_size == nbytes) {
		linked_list_remove(&memList, smallestIndex + 1);
		fragment_count--;
	}

	// Otherwise reduce the amount of free space in the block
	else {
		bestNode->data_size -= nbytes;
		bestNode->data = allocation + nbytes;
	}

	// Update stats
	allocation_count++;
	allocated_space += nbytes;

	if (get_remaining_space() == 0) {
		fragment_count = 0;
	}

	// return the pointer to the allocated memory
	pthread_mutex_unlock(&mem_mutex);
	return allocation;
}

/* myfree()
 *     Requests a block of memory be freed and the storage made
 *         available for future allocations
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: ptr - a pointer to the start of the space to be freed
 *         Returns: void
 *         Signals a SIGSEGV if a free is not valid
 *             - memory manager is not initialized
 *             - memory manager has been destroyed
 *             - ptr is not allocated (e.g. double free)
 */
void myfree(void* ptr) {
	pthread_mutex_lock(&mem_mutex);
	if (mem_status == 0) {
		// Signal segmentation fault if the memory is not allocated
		pthread_mutex_unlock(&mem_mutex);
		pthread_mutex_destroy(&mem_mutex);
		kill(0, SIGSEGV);
	}

	// create iterator
	node* prevNode = NULL;
	node* currentNode = NULL;
	node* nextNode = NULL;

	// Set iterator to front node of list
	currentNode = memList.front;
	nextNode = currentNode->next;

	// Navigate to the end of the list
	int index = 0;
	while (currentNode != NULL) {
		// If the data in the current block is equal to the data to be freed
		if (currentNode->data == ptr && !currentNode->is_free) {
			// Set the pointer to NULL
			ptr = NULL;

			// Reduce total allocated space by the size of the data freed
			allocated_space -= currentNode->data_size;
			
			// free the data (unused - took away using the heap)
			// free(ptr);

			// Set the data pointer to NULL (indicates a free block)
			currentNode->is_free = FREE;

			// Update fragment count
			fragment_count++;

			// If previous block is also free
			if (prevNode != NULL && prevNode->is_free) {
				// Combine the sizes
				prevNode->data_size += currentNode->data_size;
				
				// Remove the second block
				linked_list_remove(&memList, index);

				// Retract currentNode
				currentNode = prevNode;

				// Update fragment count
				fragment_count--;

				// Update index (one entry was removed)
				index--;
			}

			// If next block is also free
			if (nextNode != NULL && nextNode->is_free) {
				// Combine the sizes
				currentNode->data_size += nextNode->data_size;

				// Remove the second block
				linked_list_remove(&memList, index + 1);

				// Update fragment count
				fragment_count--;
			}

			// return 
			pthread_mutex_unlock(&mem_mutex);
			return;
		}
		// iterate to next element
		prevNode = currentNode;
		currentNode = currentNode->next;
		nextNode = currentNode->next;
		index++;
	}

	// end is reached and ptr is not found -> signal segmentation fault
	pthread_mutex_unlock(&mem_mutex);
	pthread_mutex_destroy(&mem_mutex);
	kill(0, SIGSEGV);
}

/* get_allocated_space()
 *     Retrieve the current amount of space allocated by the memory manager (in bytes)
 *         Parameters: None
 *         Returns: int - the current number of allocated bytes 
 */
int get_allocated_space() {
	return allocated_space;
}

/* get_remaining_space()
 *     Retrieve the current amount of available space in the memory manager (in bytes)
 *         (e.g. sum of all free blocks)
 *         Parameters: None
 *         Returns: int - the current number of free bytes 
 */
int get_remaining_space() {
	return max_size - allocated_space;
}

/* get_fragment_count()
 *     Retrieve the current amount of free blocks (i.e. the count of all the block, not the size)
 *         Parameters: None
 *         Returns: int - the current number of free blocks
 */
int get_fragment_count() {
	return fragment_count;
}

/* get_mymalloc_count()
 *     Retrieve the number of successfull malloc calls (for all placement types)
 *         Parameters: None
 *         Returns: int - the total number of successfull mallocs
 */
int get_mymalloc_count() {
	return allocation_count;
}

 /*
 * Prints out all of the entries in the memory manager
 * Calls linked_list_print() function
 */
void mem_print() {
	printf("\n");
	linked_list_print(&memList);
}

/*
 * Prints all of the stats from the memory manager
 *  prints remaining space
 *  prints fragment count
 *  prints allocation count
 *  prints currently allocated space 
 */
void mem_print_stats() {
	printf("Av. Mem: %d, Fragment Count: %d, ", get_remaining_space(), get_fragment_count());
	printf("Alloc. Count: %d, Mem. Used: %d\n", get_mymalloc_count(), get_allocated_space());
}