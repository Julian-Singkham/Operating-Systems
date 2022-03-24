/**
  ***********************************************************************************
  * @file   : linked_list.c
  * @brief  : The memory data structure is a linked list.
  *         : Lab 6: Memory Manager
  *         : CS-3841/021 
  * @date   : OCT 27 2020
  * @author : Austin Grenert
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
#include "linked_list.h"

// LINKED LIST FUNCTION DEFINITIONS
//===========================================================================||

/*
 * Initializes the linked list by setting all pointers to null
 * @param ll - the linked list to initialize
 */
void linked_list_init(linked_list* ll) {
    // Set size to begginning value of 0
    ll->size = 0;

    // Set the pointer to the first node to NULL (list starts empty)
    ll->front = NULL;
}

/*
 * Adds data pointer to the front of the linked list
 * @param ll   - the linked list to add to
 * @param data - pointer to the data to add to the linked list
 * @param data_size - the size of the data that data is pointing to in bytes
 * @param is_free - indicates if the data in the new node is free or not
 *
 * @return 0 when add succeeds, -1 when it fails
 */
int linked_list_add_front(linked_list* ll, void* data, int data_size, int is_free) {
    // Attempts to allocate the space for the new node
    node* newNode = malloc(sizeof(node));

    // If allocation fails, return with FAILED status
    if (newNode == NULL) {
        return FAILED;
    }

    // Set data of the new node to the data provided in arguments
    newNode->data = data;
    newNode->data_size = data_size;
    newNode->is_free = is_free;

    // If the linked list is empty, create first entry
    if (linked_list_is_empty(ll)) {
        ll->front = newNode;
        newNode->next = NULL;
    } 
    
    // If the linked list is not empty, add entry to the front
    else {
        newNode->next = ll->front;
        ll->front = newNode;
    }

    // Increment size due to entry being added
    ll->size++;

    // Return SUCCESS status
    return SUCCESS;
}

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
int linked_list_add(linked_list* ll, void* data, int data_size, int index, int is_free) {
    // If index is out of range, return a FAILED status
    if(index > ll->size || index < 0) {
        return FAILED;
    }

    // If the index is the end of the list, call other function that adds to the end
    // and return SUCCESS status
    if (index == ll->size) {
        linked_list_add_end(ll, data, data_size, is_free);
        return SUCCESS;
    }
    
    // If the index is the begginning of the list, call other function that adds to the
    // front and return SUCCESS status
    if (index == 0) {
        linked_list_add_front(ll, data, data_size, is_free);
        return SUCCESS;
    }
        
    // Attempts to allocate the space for the new node
    node* newNode = malloc(sizeof(node));

    // If allocation fails, return with FAILED status
    if (newNode == NULL) {
        return FAILED;
    }
    
    // Set data of the new node to the data provided in arguments
    newNode->data = data;
    newNode->data_size = data_size;
    newNode->is_free = is_free;

    // Create pointers to current and previous nodes
    node* prev;
    node* iter = ll->front;

    // Navigate current and previous node so that current node points to the node
    // at index specified
    for (int i = 0; i < index; i++) {
        prev = iter;
        iter = iter->next;
    }
    
    // Set the current node at the index specified to the next node after that index
    newNode->next = iter;

    // Place the new node in at the index specified
    prev->next = newNode;

    // Increase the size of the list
    ll->size++;

    // Return SUCCESS status
    return SUCCESS;
}

/*
 * Adds data pointer to the end of the linked list
 * @param ll - the linked list to add to
 * @param data - pointer to the data to add to the linked list
 * @param data_size - the size of the data that data is pointing to in bytes
 * @param is_free - indicates if the data in the new node is free or not
 */
int linked_list_add_end(linked_list* ll, void* data, int data_size, int is_free) {
    // Attempts to allocate the space for the new node
    node* newNode = malloc(sizeof(node));

    // If allocation fails, return with FAILED status
    if (newNode == NULL) {
        return FAILED;
    }

    // Set data of the new node to the data provided in arguments
    newNode->data = data;
    newNode->data_size = data_size;
    newNode->is_free = is_free;
    newNode->next = NULL;

    // If the list is empty, add to the front and return with SUCCESS status
    if (linked_list_is_empty(ll)) {
        ll->front = newNode;
        ll->size++;
        return SUCCESS;
    }

    // Set iterator to front node
    node* iter =  ll->front;

    // Navigate to the end of the list
    while(iter->next != NULL) {
        iter = iter->next;
    }

    // Place new node at the end of the list
    iter->next = newNode;

    // Increase the size of the list
    ll->size++;

    // Return with SUCCESS status
    return SUCCESS;
}

/*
 * Removes the front entry of the linked list
 * @param ll - the linked list to remove from
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
int linked_list_remove_front(linked_list* ll) {
    // If the list is empty, return with FAILED status
    if (linked_list_is_empty(ll)) {
        return FAILED;
    }

    // If the list is not empty, navigate to front node
    node* frontNode = ll->front;

    // Set the new front node to the second node
    ll->front = ll->front->next;

    // Free the front node
    free(frontNode);

    // Reduce the size of the list
    ll->size--;

    // Return with SUCCESS status
    return SUCCESS;
}

/*
 * Removes the last entry of the linked list
 * @param ll - the linked list to remove from
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
int linked_list_remove_end(linked_list* ll) {
    // If the list is empty, return with FAILED status
    if (linked_list_is_empty(ll)) {
        return FAILED;
    }

    // If the list is not empty, start at front node
    node* iter =  ll->front;

    // Create prev pointer
    node* prev;

    // Navigate to the last node in the list, where prev points to the
    // second to last node
    while (iter->next != NULL) {
        prev = iter;
        iter = iter->next;
    }

    // Make prev node the new last node
    prev->next = NULL;

    // Free the previous last node
    free(iter);

    // Reduce size of the linked list
    ll->size--;

    // Return with SUCCESS status
    return SUCCESS;
}

/*
 * Removes an entry at the specified index of the linked list
 * @param ll - the linked list to remove from
 * @param index - the index to remove the entry at
 *
 * @return 0 when remove succeeds, -1 when it fails
 */
int linked_list_remove(linked_list* ll, int index) {
    // If index is out of the range of the list, return with FAILED status
    if (index > ll->size - 1|| index < 0) {
        return FAILED;
    }

    // If the index is the end of the list, call other function to remove from the end
    // of the list 
    if (index == ll->size - 1) {
        linked_list_remove_end(ll);
        return SUCCESS;
    }

    // If the index is the front of the list, call other function to remove from the
    // front of the list
    if (index == 0) {
        linked_list_remove_front(ll);
        return SUCCESS;
    }

    // Create pointers to current and previous nodes
    node* prev;
    node* iter = ll->front;

    // Navigate to the index specified, where prev points to the previous node
    for(int i = 0; i < index; i++) {
        prev = iter;
        iter = iter->next;
    }

    // Remove the node at that index
    prev->next = iter->next;

    // Free the node that was previously at that index
    free(iter);

    // Reduce the size of the list
    ll->size--;

    // Return with SUCCESS status
    return SUCCESS;
}

/*
 * Returns the size of the linked list
 *
 * @return the size of the linked list
 */
int linked_list_get_size(linked_list* ll) {
    // Returns the size of the linked list
    return ll->size;
 }

/*
 * Returns if the list is empty or not
 *
 * @return 1 if the list is empty, 0 if not
 */
int linked_list_is_empty(linked_list *ll) {
     // If the front node is NULL, the list is empty
     if (ll->front == NULL) {
         return TRUE;
     } 

     // If the front node points to an entry, the list is not empty
     else {
         return FALSE;
     }
}

/*
 * Destroys the linked list by freeing all nodes
 */
void linked_list_destroy(linked_list* ll) {
    // Creates pointers to current and next node
    node* nodeToFree;
    node* nextNode;

    // If the list is empty, return and do nothing
    if (linked_list_is_empty(ll)) {
        return;
    } 
    
    // If the list is not empty, set current node to the first node
    else {
        nodeToFree = ll->front;
    }

    // Iterate through each node in the list
    while(nodeToFree != NULL) {
        // Set the next node pointer
        nextNode = nodeToFree->next;

        // Free the current node
        free(nodeToFree);

        // Reduce size of list
        ll->size--;

        // Set current node to the next node
        nodeToFree = nextNode;
    }

    ll->front = NULL;
}

/*
 * Prints out the linked list information for debugging use
 */
 void linked_list_print(linked_list* ll) {
     // Create iterator to iterate through entire list
     node* iter = ll->front;

     // Print size
     printf("Size: %d\n", ll->size);

     // Iterate through the list
     int index = 0;
     while (iter != NULL) {
         printf("Node at %p:\n", iter);
         printf("Node Index    : %d\n", index);
         printf("Node Size     : %d\n", iter->data_size);
         printf("Node Ptr      : %p\n", iter->data);
         printf("Is Node Free? : ");
         if (iter->is_free) { printf("YES\n\n"); }
         else { printf("NO\n\n"); }
         index++;
         iter = iter->next;
     }
 }

//===========================================================================||