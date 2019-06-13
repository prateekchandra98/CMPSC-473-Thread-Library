/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct mutex_s {
    /* IMPLEMENT THIS */
    int lockFlag;       //if it is locked, it is one, otherwise it is zero
    int initializeFlag; //if it has been initialized, it is one, otherwise it is zero
    
} mutex_t;

#endif /* _MUTEX_TYPE_H */
