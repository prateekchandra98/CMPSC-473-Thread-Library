#include <rwlock.h>

// Uncomment the following line to disable printf calls
//#define _REMOVE_PRINTF_CALLS_
#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif // _REMOVE_PRINTF_CALLS_

int rwlock_init(rwlock_t* rw)
{
    /* IMPLEMENT THIS */
    return 0;
}

void rwlock_destroy(rwlock_t* rw)
{
    /* IMPLEMENT THIS */
}

void rwlock_lock(rwlock_t* rw, enum rwlock_type type)
{
    /* IMPLEMENT THIS */
}

void rwlock_unlock(rwlock_t* rw)
{
    /* IMPLEMENT THIS */
}

void rwlock_downgrade(rwlock_t* rw)
{
    /* IMPLEMENT THIS */
}
