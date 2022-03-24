/**
  ***********************************************************************************
  * @file   : mem_ff_test.c
  * @brief  : Test file for first fit algorithm
  *         : Lab 6: Memory Manager
  *         : CS-3841/021 
  * @date   : NOV 02 2020
  * @author : Julian Singkham
  ***********************************************************************************
  * @attention
  *
  * This tests the memory manager with first fit algorithm. First fit is where the
  * requested memory block is placed in the next available free block.
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
  * @brief Memory manager using first fit algorithm.
  * 
  * @param NONE
  * 
  * @retval NONE
  */
int main() {

    char my_heap[MY_HEAP_SIZE];
    printf("TESTING MEMORY INITIALIZATION\n");
    printf("Starting Address: %p\n\n", &my_heap);
    mmInit(my_heap, MY_HEAP_SIZE);
    mem_print_stats();
    printf("---------------------------------------------\n");

    printf("TESTING FILLED MEMORY\n");
    char* ptr1; char* ptr2; char* ptr3; char* ptr4; char* ptr5; char* test;
    ptr1 = mymalloc_ff(10);
    ptr2 = mymalloc_ff(15);
    ptr3 = mymalloc_ff(40);
    ptr4 = mymalloc_ff(30);
    ptr5 = mymalloc_ff(5);

    mem_print_stats();
    printf("Should be: (0-0-5-100)\n");
    mem_print();
    printf("Node Sizes should be: 10, 15, 40, 30, 5\n");
    printf("---------------------------------------------\n");


    printf("TESTING REMOVAL OF MEMORY NODES\n");
    myfree(ptr1);
    myfree(ptr3);
    myfree(ptr5);

    mem_print_stats();
    printf("Should be: (55-3-5-45)\n");
    mem_print();
    printf("Node Sizes should be: 10, 15, 40, 30, 5 and Node 0, 2, 4 should be free\n");
    printf("---------------------------------------------\n");


    printf("TESTING FIRST FIT MEMORY REQUEST\n");
    test = mymalloc_ff(5);

    mem_print_stats();
    printf("Should be: (50-3-6-50)\n");
    mem_print();
    printf("Node Sizes should be: 5, 5, 15, 40, 30, 5 and Node 1, 3, 5 should be free\n");
    printf("---------------------------------------------\n");


    printf("TESTING CONGREGATION OF EMPTY MEMORY NODES\n");
    myfree(test);
    myfree(ptr2);

    mem_print_stats();
    printf("Should be: (70-2-6-30)\n");
    mem_print();
    printf("Node Sizes should be: 65, 30, 5 and Node 0, 2 should be free\n");
    printf("---------------------------------------------\n");
    
    printf("TESTING OF CLEARED MEMORY\n");
    myfree(ptr4);

    mem_print_stats();
    printf("Should be: (100-1-6-0)\n");
    printf("---------------------------------------------\n");


    printf("TESTING OF TOO LARGE OF A MEMORY REQUEST\n");
    ptr1 = mymalloc_ff(101);
    mem_print_stats();
    printf("Should be same: Malloc Fail due to size limit\n");
    printf("---------------------------------------------\n");


    printf("TESTING OF DOUBLE CLEARING MEMORY\n");
    printf("Should cause Segmentation Fault:\n");
    myfree(ptr1);

    return 0;
}