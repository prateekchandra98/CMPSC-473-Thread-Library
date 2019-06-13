/*  @file thr_internals.h
 *  
 *  IMPORTANT:
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

/* ADD OTHER LIBTHREAD-RELATED HEADER CONTENT HERE */
#include <mutex.h>
#include <cond.h>
typedef struct thread_s{
    //ID of the thread
    int threadID;
    //status: used in thr_join to update the status
    void *status;
    //base address of the stack for that particular thread
    void *baseAddress;
    //exitFlag: to check weather the thread has exited or not
    int exitFlag;
    //joinedFlag: used to ensure that multiple threads cannot join the same thread at the same time
    int joinedFlag;
    //stack_size: allocated memory for the stack of a particular thread
    unsigned int stack_size;
    //threadCond: used to signal the joined thread about the current thread's status
    cond_t threadCond;
}thread_t;

//this is for the conditional variable
typedef struct thread_cond_s{
    int waitFlag;
    int threadID;
}thread_cond_t;

extern mutex_t malloc_mutex;

#endif /* THR_INTERNALS_H */
