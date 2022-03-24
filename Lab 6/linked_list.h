/**
  ***********************************************************************************
  * @file   : linked_list.h
  * @brief  : Outline for the linked list (memory structure)
  *         : Lab 6: Memory Manager
  *         : CS-3841/021 
  * @date   : OCT 26 2020
  * @author : Austin Grenert
  ***********************************************************************************
**/
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAILED -1
#define TRUE 1
#define FALSE 0

// LINKED LIST STRUCTURES
//===========================================================================||

typedef struct node {
    int data_size;
    void* data;
    int is_free;
    struct node* next;
} node;

typedef struct linked_list {
    int size;
    node* front;
} linked_list;

//===========================================================================||

// LINKED LIST FUNCTION DECLARATIONS
//===========================================================================||

/*
 * Initializes the linked list by setting all pointers to null
 * @param ll - the linked list to initialize
 */
extern void linked_list_init(linked_list* ll);

/*
 * Adds data pointer to the front of the linked list
 * @param ll   - the linked list to add to
 * @param data - pointer to the data to add to the linked list
 * @param data_size - the size of the data that data is pointing to in bytes
 * @param is_free - indicates if the data in the new node is free or not
 *
 * @return 0 when add succeeds, -1 when it fails
 */
extern int linked_list_add_front(linked_list* ll, void* data, int data_size, int is_free);

/*
 * Adds data pointer to the end of the linked list
 * @param ll - the linked list to add to
 * @param data - pointer to the data to add to the linked list
 * @param data_size - the size of the data that data is pointing to in bytes
 * @param is_free - indicates if the data in the new node is free or not
 */
extern int linked_list_add_end(linked_list* ll, void* data, int data_size, int is_free);

/*
 * Adds data pointer to the specified index of the linked list
 * @param ll - the linked list to add to
 * @param data - the pointer to the data to add to the linked list
 * @param data_size - the size of the data that data is pointing to in bytes
 * @param index - the index to add the data to
 * @param is_free - indicates if the data in the new node is free or not
 *
 * @return 0 when add succeeds, -1 when it fails
 */
extern int linked_list_add(linked_list* ll, void* data, int data_size, int index, int is_free);

/*
 * Removes the front entry of the linked list
 * @param ll - the linked list to remove from
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
extern int linked_list_remove_front(linked_list* ll);

/*
 * Removes the last entry of the linked list
 * @param ll - the linked list to remove from
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
extern int linked_list_remove_end(linked_list* ll);

/*
 * Removes an entry at the specified index of the linked list
 * @param ll - the linked list to remove from
 * @param index - the index to remove the entry at
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
extern int linked_list_remove(linked_list* ll, int index);

/*
 * Returns the size of the linked list
 *
 * @return the size of the linked list
 */
 extern int linked_list_get_size(linked_list* ll);

/*
 * Returns if the list is empty or not
 *
 * @return 1 if the list is empty, 0 if not
 */
 extern int linked_list_is_empty(linked_list *ll);

 /*
  * Destroys the linked list by freeing all nodes
  */
extern void linked_list_destroy(linked_list* ll);

/*
 * Prints out the linked list information for debugging use
 */
 extern void linked_list_print(linked_list* ll);


//===========================================================================||
#endif /* LINKED_LIST_H */