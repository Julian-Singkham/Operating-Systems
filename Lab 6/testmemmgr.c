/**
  ***********************************************************************************
  * @file   : testmemmgr.c
  * @brief  : Test file for multi-threading
  *         : Lab 6: Memory Manager
  *         : CS-3841/021 
  * @date   : OCT 22 2020
  * @author : Prof Lembke
  ***********************************************************************************
  * @attention
  *
  * This tests the memory manager with best fit algorithm. Best fit is where the
  * requested memory block is placed into the smallest available free block.
  * 
  * NOTE: Segmentation is to be expected in this test.
  * 
  ***********************************************************************************
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memory_manager.h"

#define MY_HEAP_SIZE 100

/**
  * @brief Memory manager using best fit algorithm.
  * 
  * @param NONE
  * 
  * @retval NONE
  */
int main()
{
	char my_heap[MY_HEAP_SIZE];
	printf("Heap Address: %p\n", &my_heap);
	mmInit(my_heap, MY_HEAP_SIZE);
	
	printf("1 -- Available Memory: %d, Fragment Count: %d\n", get_remaining_space(), get_fragment_count());

	// Allocate 10 bytes
	//   shouldn't fail
	char* ptr1 = mymalloc_bf(10);
	if(ptr1 == NULL) {
		printf("ptr1 - mymalloc_bf(10) failed\n");
		exit(EXIT_FAILURE);
	}
	strncpy(ptr1, "HELLO", 10);
	printf("ptr1 is %s\n", ptr1);
	
	printf("2 -- Available Memory: %d, Fragment Count: %d\n", get_remaining_space(), get_fragment_count());

	// Allocate 45 bytes
	//   shouldn't fail
	char* ptr2 = mymalloc_bf(45);
	if(ptr2 == NULL) {
		printf("ptr2 - mymalloc_bf(45) failed\n");
		exit(EXIT_FAILURE);
	}
	strncpy(ptr2, "GOODBYE", 45);
	printf("ptr2 is %s\n", ptr2);	
		
	printf("3 -- Available Memory: %d, Fragment Count: %d\n", get_remaining_space(), get_fragment_count());
	
	// Attempt to allocate 50 bytes
	//   should fail
	char* ptr3 = mymalloc_bf(50);
	if(ptr3 == NULL) {
		printf("ptr3 - mymalloc_bf(50) failed\n");
	}
	printf("4 -- Available Memory: %d, Fragment Count: %d\n", get_remaining_space(), get_fragment_count());
	
	// Free the first two pointers
	myfree(ptr2);

	mem_print();
	
	myfree(ptr1);
	printf("5 -- Available Memory: %d, Fragment Count: %d\n", get_remaining_space(), get_fragment_count());
	
	printf("Total successful mallocs: %d\n", get_mymalloc_count());
	
	// Double free, should cause a segmentation fault (IT DOES)
	myfree(ptr2);
	
	mmDestroy();

	printf("Complete\n");
	
	return 0;
}
