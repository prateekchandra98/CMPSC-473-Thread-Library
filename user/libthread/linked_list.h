#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
//#include <stddef.h>

/*
 *Programmer: Prateek Chandra
 *Date: 16 March 2018
 *Course: CMPSC 473
 *File: linked_list.h
*/

typedef struct list_node {
    struct list_node* next;
    struct list_node* prev;
    void* thread_data;
    int threadID;
} list_node_t;

typedef struct {
    list_node_t* head;
    list_node_t* tail;
    size_t count;
} list_t;

// Create and return a new list
void list_create(list_t* my_doubly_linked_list);

// Destroy a list
void list_destroy(list_t* list);

// Find the first node in the list with the given threadID
// Returns NULL if threadID could not be found
list_node_t* list_find(list_t* list, int threadID);

// Insert a new node in the list with the given threadID
int list_insert(list_t* list, void* thread_data, int threadID);

// Removes a specific node from the list and free the node resources
void list_remove(list_t* list, list_node_t* node);

//remove the last element from the list (FIFO)
int list_dequeue (list_t* list, void **data);

#endif
