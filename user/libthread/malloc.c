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
DESIGN STRATEGIES                                   malloc.c                                   27th April 2017
###############################################################################################################

Use a global mutex to make everything atomic. The mutex is initialized in thread_init since thread_init is the
first thing the user calls. 

###############################################################################################################
*/
#include <malloc.h>
#include <stdlib.h>
#include <types.h>
#include <stddef.h>
#include <mutex.h>
#include <thr_internals.h>

// Uncomment the following line to disable printf calls
//#define _REMOVE_PRINTF_CALLS_
#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif // _REMOVE_PRINTF_CALLS_

mutex_t malloc_mutex;

void* malloc(size_t size)
{
    /* IMPLEMENT THIS : MAKE THIS THREAD-SAFE */
    mutex_lock(&malloc_mutex);
    void *ptr = _malloc(size);
    mutex_unlock(&malloc_mutex);
    return ptr;
}

void* calloc(size_t nelt, size_t eltsize)
{
    /* IMPLEMENT THIS : MAKE THIS THREAD-SAFE */
    mutex_lock(&malloc_mutex);
    void *ptr =_calloc(nelt, eltsize);
    mutex_unlock(&malloc_mutex);
    return ptr;
}

void* realloc(void* buf, size_t new_size)
{
    /* IMPLEMENT THIS : MAKE THIS THREAD-SAFE */
    mutex_lock(&malloc_mutex);
    void *ptr = _realloc(buf, new_size);
    mutex_unlock(&malloc_mutex);
    return ptr;
}

void free(void* buf)
{
    /* IMPLEMENT THIS : MAKE THIS THREAD-SAFE */
    mutex_lock(&malloc_mutex);
    _free(buf);
    mutex_unlock(&malloc_mutex);
}
