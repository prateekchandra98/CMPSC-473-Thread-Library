/** @file sem_type.h
 *  @brief This file defines the type for semaphores.
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H
#include <cond_type.h>
#include <mutex_type.h>
#include <mutex.h>
#include <cond_type.h>
#include <cond.h>
typedef struct sem_s {
    /* IMPLEMENT THIS */
    cond_t semCond;
    mutex_t semMutex;
    int initializeFlag;
    int semaphore;
} sem_t;

#endif /* _SEM_TYPE_H */
