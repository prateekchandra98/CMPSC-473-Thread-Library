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
DESIGN STRATEGIES                                       sem.c                                   27th April 2017
###############################################################################################################
I am using the mutex and conditional variable for the implementation of semaphores. Everything else is self
explanatory.  
###############################################################################################################
*/
#include <sem.h>
#include <cond.h>
#include <sem_type.h>
#include <cond_type.h>
#include <mutex_type.h>
#include <mutex.h>
#include <assert.h>

// Uncomment the following line to disable printf calls
//#define _REMOVE_PRINTF_CALLS_
#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif // _REMOVE_PRINTF_CALLS_

/*
 *This function should initialize the semaphore pointed to by sem to the value count. 
 *Effects of using a semaphore before it has been initialized may be undelned. 
 *This function returns zero on success and a number less than zero on error.
 */
int sem_init(sem_t* sem, int count)
{
    /* IMPLEMENT THIS */
    if (sem == NULL)
    {
        return -1;
    }
    if (mutex_init(&sem->semMutex)<0)
    {
        return -1;
    }
    if (cond_init(&sem->semCond)<0)
    {
        return -1;
    }
    sem->initializeFlag = 1;
    sem->semaphore = count;
    return 0;
}

/*
 *This function should \deactivate" the semaphore pointed to by sem. 
 *Effects of using a semaphore after it has been destroyed may be undened.
 *It is illegal for an application to use a semaphore after it has been destroyed (unless and until it is later re-initialized). 
 *It is illegal for an application to invoke sem destroy() on a semaphore while threads are waiting on it.
 */
void sem_destroy(sem_t* sem)
{
    /* IMPLEMENT THIS */
	mutex_lock(&sem->semMutex);
	if (sem == NULL || sem -> initializeFlag != 1)
    {
		mutex_unlock(&sem->semMutex);
		assert(0);
    }
	else
	{
		sem -> initializeFlag = 0;
		cond_destroy(&sem->semCond);
		mutex_unlock(&sem->semMutex);
		mutex_destroy(&sem->semMutex);
	}   
}

/*
 *The semaphore wait function allows a thread to decrement a semaphore value, and may cause it to block indenitely until it is legal to perform the decrement.
 */
void sem_wait(sem_t* sem)
{
    mutex_lock(&sem->semMutex);
    if (sem == NULL || sem -> initializeFlag != 1)
    {
        mutex_unlock(&sem->semMutex);
        assert(0);
    }
    //decrement the semaphore
    sem->semaphore--;
    //check if the semaphore is less than 0
    if (sem->semaphore < 0)
    {
        cond_wait (&sem->semCond, &sem->semMutex);
    }
    mutex_unlock(&sem->semMutex);
}

/*
 *This function should wake up a thread waiting on the semaphore pointed to by sem, if one exists, and should update the semaphore value regardless.
 */
void sem_signal(sem_t* sem)
{
    /* IMPLEMENT THIS */
    mutex_lock(&sem->semMutex);
    if (sem == NULL || sem -> initializeFlag != 1)
    {
        mutex_unlock(&sem->semMutex);
        assert(0);
    }
    //check if the semaphore is less than 0
    if (sem->semaphore < 0)
    {
        cond_signal (&sem->semCond);
    }
    //increment the semaphore
    sem->semaphore++;
    
    mutex_unlock(&sem->semMutex);
}
