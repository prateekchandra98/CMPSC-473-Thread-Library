/*
###############################################################################################################
     ______        ______              ___         ____________     _________     _________     __     __
    |   __  `-    |   __  `-          /   \       |____    ____|   |   ______|   |  _______|   |  |  /  /
    |  |   \  \   |  |   \  \        /  ^  \           |  |        |  |          |  |          |  | /  /
    |  |__ /  /   |  |__ /  /       /  /_\  \          |  |        |  |____      |  |____      |  |/  /
    |   ___ ~`    |      _~`       /   ___   \         |  |        |   ____|     |   ____|     |     <
    |  |          |  |\  \        /  /     \  \        |  |        |  |          |  |          |  |\  \
    |  |          |  | \  \      /  /       \  \       |  |        |  |______    |  |______    |  | \  \
    |__|          |__|  \__\    /__/         \__\      |__|        |_________|   |_________|   |__|  \__\

###############################################################################################################
DESIGN STRATEGIES                               linked_list.c                                   27th April 2017
###############################################################################################################

LIST_NODE_T:

    *next       : Pointing to the next node in the list. This is how you iterate through the list
    *prev       : Pointing to the previous node in the list. This is used to update the tail. Why do I need the 
                  tail? It is explained in cond.c file. 
    *thread_data: It is a void pointer because it can point to any struct type. The reason i use two seperate
                  structs for conditional variable and threads is to save memory. thread_cond_t needs only two 
                  int where as thread_t needs a lot more than that. 
    *threadID   : This is used to make the search easier.

LIST_CREATE:    It just sets the initial values of the lists which makes things easier later. 

LIST_DESTROY:   This function frees all the memory related to the list. Every node and every thread_data. 

LIST_INSERT:    This function inserts a node into the list. The node is placed at the head of the list, and 
                the entire list is shifted. Advatage of doing this method is that you do not have to loop,
                through the entire list. Only disadvantage is that it mallocs a new node which takes away
                heap space from the user. 

LIST_DEQUEUE:   Removes the last thread in the list (AKA the thread that is waiting the longest). In order to
                do so trivially, i would have to loop through the entire list to get the last node. But instead,
                there is a tail in my list which is a pointer pointing to the last node. So it just removes the 
                tail and updates the tail to the previous node. 

LIST_REMOVE:    This is a generic remove. It removes a particular node from the list and updates the pointers. 

LIST_FIND:      It finds the data regarding the thread from the linked list. The only way to do it is to loop 
                through the entire list which takes O(n) time. A better implementation would be using a red 
                black tree which takes log(n) time. This function is used by the thread library. 
###############################################################################################################
*/
#include "linked_list.h"
#include <assert.h>
#include <stdio.h>

/* 
--------------------------------------
STRUCT DEFINITION (JUST FOR REFERENCE)

    typedef struct list_node {
        struct list_node* next;
        struct list_node* prev;
        void *thread_data;
        int threadID;
    } list_node_t;

    typedef struct {
        list_node_t* head;
        list_node_t* tail;
        size_t count;
    } list_t;
--------------------------------------
*/

// Create and return a new list
void list_create(list_t* my_doubly_linked_list)
{
    my_doubly_linked_list->head = NULL;
    my_doubly_linked_list->tail = NULL;    
    my_doubly_linked_list->count = 0;

    return;
}

// Destroy a list
void list_destroy(list_t* list)
{
    list_node_t* node;
    node = list->head;

    list_node_t* temp;
    temp = node;

    while (node!=NULL)
    {
        temp = node;
        free (node->thread_data);
        node->thread_data = NULL;
        free (node);
        node = NULL;
        node = temp->next;
    }
    list->head = NULL;
    list->tail= NULL;
}

// Find the first node in the list with the given threadID
// Returns NULL if threadID could not be found
list_node_t* list_find(list_t* list, int threadID)
{
    list_node_t* node;
    node = list->head;
    {
        while ((node)!=NULL && (node)->threadID != threadID) 
        {
            node = (node)->next;
        }
        assert(node != NULL);
        return node;
    }
}

// Insert a new node in the list with the given threadID 
int list_insert(list_t* list, void* thread_data, int threadID)
{
    /* IMPLEMENT THIS IF YOU WANT TO USE LINKED LISTS */
    list_node_t* node;
    node = list->head;

    if (node == NULL) // BASE CASE
    {
        (list->head) = (list_node_t*)malloc(sizeof(list_node_t));
        if (list->head == NULL)
        {
        	return -1;
        }
        (list->tail) = list->head;
        (list->head)->thread_data = thread_data;
        (list->head)->threadID = threadID;
        //next and the previous is null since it is the first and only element
        (list->head)->next = NULL;
        (list->head)->prev = NULL;
        list->count++;
        return 0;
    }
    else
    {
        //allocate the size of the element
        (node->prev) = (list_node_t*)malloc(sizeof(list_node_t));
        if (node->prev == NULL)
		{
			return -1;
		}
        //set the previous pointer of the newly added element 
        (node->prev)->next = node;
        (node->prev)->prev = NULL;
        //store the threadID
        (node->prev)->thread_data = thread_data;
        (node->prev)->threadID = threadID;
        //since it is the first element, the head is pointing to the previous node
        (list->head) = node->prev;
        //increment the count
        list->count++;
        return 0;
    }
}
int list_dequeue (list_t* list, void **data)
{
    
    if (list->count == 0 || list == NULL)
    {
        return -1;
    }
    else
    {
        list_node_t* node;
        
        node = list->tail;

        *data = node->thread_data;

		if (node ->prev != NULL)
		{
			node->prev->next = node->next;
		}
		if(list->head == node)
		{
			list->head = node->next;
        }
        if(list->tail == node)
        {
            list->tail = node->prev;
        }
        //decrement the size
        list ->count --;
        //free the node entirely
        free (node);
        node = NULL;
        return 0;
    }
}

// Remove a node from the list and free the node resources
void list_remove(list_t* list, list_node_t* node)
{
    /* IMPLEMENT THIS IF YOU WANT TO USE LINKED LISTS */
    //find the exact node that needs to be removed
    
    if(node->next != NULL)
    {
        (node->next)->prev = node->prev;
    }
    if (node ->prev != NULL)
    {
        node->prev->next = node->next;
    }
    if(list->head == node)
    {
        list->head = node->next;
    }
    //Update the tail if the given node is the last node
    if(list->tail == node)
    {
        list->tail = node->prev;
    }


    list->count--;
    free(node->thread_data);
    free(node);
    node = NULL; 
}