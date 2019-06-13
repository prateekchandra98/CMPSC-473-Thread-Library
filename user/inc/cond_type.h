/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

#include <linked_list.h>
#include <stdlib.h> 
#include <mutex_type.h>
#include <mutex.h> 

typedef struct cond_s {
    /* IMPLEMENT THIS */
    int initializeFlag; //If it has been initialized, it is one, otherwise it is zero
    mutex_t condMutex; //mutex required for atomicity
    list_t list; //list to store the list of the waiting threads
} cond_t;

#endif /* _COND_TYPE_H */
