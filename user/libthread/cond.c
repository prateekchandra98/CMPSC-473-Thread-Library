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
DESIGN STRATEGIES                                     cond.c                                    27th April 2017
###############################################################################################################

COND_T: This is the data stored for the conditional variable. It contains the following:

        *initializeFlag : To check weather the conditional variable has been initialized
        *condMutex      : mutex required for atomicity
        *list           : list to store the list of the waiting threads

THREAD_COND_T: This is the data stored in each node of the list. It contains the following:

        *waitFlag   : To indicate weather the thread is waiting
        *threadID   : To diffrenciate between threads

Important flags check:

	*a waiting conditional v. cannot call cond_wait
	*a waiting conditional v. threads cannot call cond_destroy
	*an un-initialized conditional v cannot call cond_destroy
	*an un-initialized conditional v cannot call cond_wait
	*an un-initialized conditional v cannot call cond_signal
	*an non-waiting conditional v can call cond_signal
	*only an non-waiting conditional v cannot call cond_wait


COND_INIT:      Initialize the conditional variable that is pointed by the function's argument cond. This
                This function initializes the mutex and the list in the conditional variable

COND_DESTROY:   This function destroys whatever i initialize in cond_init. Destroys the list and the mutex and
                checks if the conditional variable is initialized.

COND_WAIT:      I malloc every thread_cond_t struct (which goes into my list) instead of creating a local variable 
                so that I have more control when i can free it from the memory instead of relying on the function 
                return. It makes it easier in my opinion. The disadvantage with this method is that it is using 
                the heap space which is taking away memory from the user.  I use a linked list to keep track of 
                the threads that are waiting. Every time cond wait is called i insert it at the front of the list,
                so that i do not have to loop through the entire list. There is a specific race condition between 
                mutex unlock and descedule. To avoid the race condition, i put in a while loop and check for the
                wait flag just in case cond_signal is called before the deschedule and after the unlock. 
                
COND_SIGNAL:    This function wakes up a thread  waiting on the condition variable pointed to by cond, if one 
                exists. In order to do so i have a doubly linked list which keeps track of the waiting thread. 
                I remove the last thread in the list (AKA the thread that is waiting the longest). In order to
                do so trivially, i would have to loop through the entire list to get the last node. But instead,
                there is a tail in my list which is a pointer pointing to the last node. Now the question arises,
                how to update the tail to the previous node when the last node is removed. Thats where the doubly
                linked list comes into play. Each node has a pointer pointing to its previous node. With this
                implementation i do not have to loop when removing or inserting a node. Hence it is fast. The 
                only disadvantage is that it is taking up more memory as each node has an extra pointer pointing 
                to its previous node which means it is taking more memory from the user since it is being malloced.

COND_BROADCAST: This function does the exact same thing as con_signal, except it does not just wake up one thread,
                from the list, it wakes up all of them. You have to loop through each node. There is no way around
                that. 

###############################################################################################################
*/
#include <cond.h>
#include <syscall.h>
#include <atomic.h>
#include <assert.h>
#include <stdlib.h>
#include <mutex.h>
#include <linked_list.h>
#include <thr_internals.h>

// Uncomment the following line to disable printf calls
#define _REMOVE_PRINTF_CALLS_
#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif //_REMOVE_PRINTF_CALLS_

/*
 *This function should initialize the condition variable pointed to by cond. 
 *It is illegal for an application to use a condition variable before it hasbeen initialized or to initialize one when it is already initialized and in use. 
 *This function returns zero on success, and a negative number on error.
 */

int cond_init(cond_t* cond)
{
    /* IMPLEMENT THIS */
    if (cond == NULL)
    {
        return -1;
    }
    cond -> initializeFlag = 1;
    //initializing and checking for failure of mutex initialization
	if ( mutex_init(&(cond->condMutex)) < 0)
	{
		return -1;
	}

    //create the list
    list_create(&cond->list);
    //checking if malloc failed
    if (&cond->list == NULL)
    {
        //printf("Malloc Failed\n");
        return -1;
    }
    
    return 0;
}

/*
 *This function should deactivate" the condition variable pointed to by cond.
 *It is illegal for an application to use a condition variable after it has been destroyed (unless and until it is later re-initialized). 
 *It is illegal for an application to invoke cond destroy() on a condition variable while threads are blocked waiting on it.
 */
void cond_destroy(cond_t* cond)
{
    /* IMPLEMENT THIS */
    mutex_lock(&cond->condMutex);

    if (cond == NULL || cond -> initializeFlag == 0 || cond->list.count > 0)
    {
        mutex_unlock(&cond->condMutex);
        assert(0);
    }
    //ask professor how to check for waiting threads
    cond -> initializeFlag = 0;
    list_destroy(&cond->list);
    mutex_unlock(&cond->condMutex);

    mutex_destroy(&cond->condMutex);
}

/*
 *The condition-wait function allows a thread to wait for a condition and release the associated mutex that it needs to hold to check that condition. 
 *The calling thread blocks, waiting to be signaled. 
 *The blocked thread may be awakened by a cond signal() or a cond broadcast(). 
 *Upon return from cond wait(), *mutex has been re-acquired on behalf of the calling thread.
 */
void cond_wait(cond_t* cond, mutex_t* mutex)
{
    /* IMPLEMENT THIS */
    mutex_lock (&(cond -> condMutex));

    if (cond == NULL || cond -> initializeFlag == 0 || mutex == NULL)
    {
        mutex_unlock (&(cond -> condMutex));
        assert(0);
    }

    //malloc to store it in the heap
    thread_cond_t* conditionalData = malloc(sizeof(thread_cond_t));
    int tid = gettid();
    conditionalData->threadID = tid;
    conditionalData->waitFlag = 0;
    
    //check if malloc fails
    if ((list_insert (&cond->list, (void *)conditionalData, tid)) < 0)
    {
    	mutex_unlock (&(cond -> condMutex));
    	assert(0);
    }

    mutex_unlock (mutex);
    mutex_unlock (&(cond->condMutex));
    
    while (!(conditionalData->waitFlag))
    {
        deschedule(&conditionalData->waitFlag);
    }

    free(conditionalData);

    mutex_lock(mutex);
}

/*
 *This function should wake up a thread waiting on the condition variable pointed to by cond, if one exists.
 *Removing the thread that is waiting the longest in order to decrease the number of waiting threads at a give time
 */
void cond_signal(cond_t* cond)
{
    /* IMPLEMENT THIS */
    mutex_lock(&cond->condMutex);
    if (cond == NULL || cond -> initializeFlag != 1  )
    {
        mutex_unlock(&cond->condMutex);
        assert(0);
    }

    thread_cond_t *conditionalData;
    int count  = cond->list.count;

    if (count != 0)
    {
        if (list_dequeue(&cond->list ,(void **)(&conditionalData))<0)
        {
            mutex_unlock(&cond->condMutex);            
            assert(0);
        }
        int tid = conditionalData->threadID;
        conditionalData->waitFlag = 1;
        
        make_runnable(tid);
    }
    mutex_unlock(&cond->condMutex);
}

/*
 *This function should wake up all threads waiting on the condition variable pointed to by cond.
 *Note that cond broadcast() should not awaken threads which may invoke cond wait(cond) \after" this call to cond broadcast() has begun execution.
*/ 
void cond_broadcast(cond_t* cond)
{
    /* IMPLEMENT THIS */
    mutex_lock(&cond->condMutex);
    if (cond == NULL || cond -> initializeFlag == 0 )
    {
        mutex_unlock(&cond->condMutex);
        assert(0);
    }
    
    thread_cond_t *conditionalData;

    int count  = cond->list.count;

    if (count != 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (list_dequeue(&cond->list ,(void **)(&conditionalData))<0)
            {
                mutex_unlock(&cond->condMutex);                
                assert(0);
            }
            int tid = conditionalData->threadID;
            conditionalData->waitFlag = 1;
            make_runnable(tid);
        }
    }
    mutex_unlock(&cond->condMutex);
}
