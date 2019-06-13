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
DESIGN STRATEGIES                                   mutex.c                                   27th April 2017
###############################################################################################################
I am using the atomicexchange and yield syscall to check weather the lock flag has been updated. Everything else
is self explanatory. Important points to note:

	*a locked mutex cannot call mutex_destroy
	*a locked mutex cannot call mutex_lock
	*an un-initialized mutex cannot call mutex_destroy
	*an un-initialized mutex cannot call mutex_lock
	*an un-initialized mutex cannot call mutex_unlock
	*an un-locked mutex cannot call mutex_unlock
	*only an un-locked mutex cannot call mutex_lock

###############################################################################################################
*/
#include <mutex.h>
#include <syscall.h>
#include <atomic.h>
#include <assert.h>
#include <stdlib.h>
//#include <410_tests.h>
// Uncomment the following line to disable printf calls
//#define _REMOVE_PRINTF_CALLS_

#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif // _REMOVE_PRINTF_CALLS_

/*
 * This function should initialize the mutex pointer to by mutex
 * It is  illegal for an application to use a mutex before it has been initialized or to initialize one when it is alreay initialized and in use
 * Returns 0 on success and -1 on error
 * Mutex_t* mutex as a parameter
 */
int mutex_init(mutex_t* mutex )
{
    if ( mutex == NULL)
    {
        return -1;
    }

    //initializing the mutex
    mutex -> initializeFlag = 1;
    mutex -> lockFlag = 0;
    //successful return
    return 0;
}
/*
 * This function should "deactivate" the mutex pointer to by mutex
 * It is illegal for an application to use a mutex after it has been destroyed (unless and until it is later re-initialized).
 * Void return which means it is fail proof
 * Mutex_t* mutex as a parameter
 */
void mutex_destroy(mutex_t* mutex)
{
    /* IMPLEMENT THIS */
    if (mutex -> initializeFlag == 1 && mutex -> lockFlag != 1)
    {
        mutex->initializeFlag = 0;
    }
    else
    {
        assert(0);
    }
}
/*
 * This function ensures the mutual exclusion in the region between itself and a call to mutex_unlock
 * Thread calling this function while another thread is in an iterfering critical section must notproceed until it is able to claim the lock
 * Void return which means it is fail proof
 * Mutex_t* mutex as a parameter
 */
void mutex_lock(mutex_t* mutex)
{
    if ( mutex -> initializeFlag == 1 )
    {
        while ((atomic_exchange(&mutex->lockFlag, 1)) == 1)
        {
            yield(gettid());
        }
    }
    else
    {
        assert(0);
    } 
}
/*
 * Signals the end of a region of mutual exclusion.
 * The calling thread gives up its claim to the lock. It is illegal for an application to unlock a
 * mutex that is not locked.
 * Void return which means it is fail proof
 * Mutex_t* mutex as a parameter
 */
void mutex_unlock(mutex_t* mutex)
{
    if ( mutex ->initializeFlag == 1 && mutex->lockFlag == 1)
    {
        atomic_exchange(&mutex->lockFlag, 0);
    }
    else
    {
        assert(0);
    }
}
