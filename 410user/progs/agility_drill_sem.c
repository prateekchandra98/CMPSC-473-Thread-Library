/** @file agility_drill_sem.c
 *
 * **> Public: Yes
 * **> Covers: sem_wait, sem_signal
 * **> For: P2
 * **> NeedsWork: Yes
 * **> Authors: geoffl
 *
 *  @brief Test program for semaphore wait/signal. Identical to agility_drill,
 *  except with binary semaphores instead of mutexes.
 *
 *  Tests: Aggressively tests acquire/release of lots of semaphores with the
 *  occasional sleep or yield call in the middle.
 *
 *  Agility drill takes three arguments (all of which have default values - if
 *  you give only zero, one or two arguments the defaults are used for the
 *  un-set arguments).
 *  First parameter: number of semaphores
 *  Second parameter: number of chasethreads (default 20)
 *  Third parameter: footprint (default 2)
 *
 *  Each 'chase' thread will attempt to acquire up to 'footprint' locks in an 
 *  array of semaphores, in strict increasing order starting from zero. Once it
 *  acquires that many locks, the thread will proceed by releasing the lowest-
 *  numbered lock before proceeding to acquire the next highest lock. So a
 *  thread with a footprint of 3 and n_semaphores set to 10 would have the
 *  following behavior:
 * 
 *  acquire 0, 1, 2
 *  release 0
 *  acquire 3
 *  release 1
 *  acquire 4
 *  ....
 *  acquire 10
 *  release 8
 *  release 9
 *  release 10
 *  
 * If the threads implementation is working, this should simply finish. If this
 * test gets stuck for reasonable-sized parameters, then there might be
 * something wrong with the threads implementation. Using huge numbers (e.g. an
 * order of magnitude more than the default paramters) may run extremely slowly
 * on Simics, of course.
 * 
 * There is a stupid magic constant (defined as 4) that determines which threads
 * will call yield in the middle of their lock acquire/release loop. Currently
 * thread with "creation_number" divisible by 4 will call yield on the iteration
 * of the main loop after it releases a lock with index divisible by 4. This is
 * essentially idiotic behavior but it stops the kernel from just running each
 * thread to completion as soon as it's completed (that'd be too easy, now,
 * wouldn't it). The stupid magic constant also determines how many threads are
 * required (at a minimum) to make this test meaningful).
 *
 *  @author Geoff Langdale (geoffl), Spring 2005
 *
 *  @bug None known
 **/

/*

   Maybe the "periodically" is really periodic; maybe the period is
   a function of the low-order bits of the thread id; maybe the sleep()
   and yield() calls are alternated strictly; probably it doesn't matter
   all *that* much because a simple application of either one will tend
   to pile threads up behind you.
*/


#include <thread.h>
#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>
#include <sem.h>
#include <assert.h>
#include <thrgrp.h>
#include "410_tests.h"
DEF_TEST_NAME ("agility_drill_sem:");

#define STACK_SIZE 4096

#define STUPID_MAGIC_CONSTANT 4

int n_semaphores = 200; // number of semaphores in array - must be at least 1 and
                     // strictly larger than footprint
int n_chasethreads = 20; // total number of chase threads
int footprint = 2; // maximum number of semaphores held simultaneously by each
                   // thread - must be at least 2
sem_t * sems; // semaphore array
thrgrp_group_t tg;

// note: this test may not bother with status after all - it's too tedious to
// release all the locks on a failed thread, so any kind of failure will gum
// things up for good

void * chase(void * arg) {
    int next_semaphore_to_acquire = 0;
    int highest_semaphore_released = -1;
    // we don't want behavior to be determined by thread id policy, use
    // 0..n_chasethreads-1 here (passed in as argument)
    int my_creation_number = (int)arg;
    char buf[80];

    snprintf(buf, sizeof (buf), "Starting chase thread with creation number %d",
             my_creation_number);
    REPORT_MISC(buf);

    while (highest_semaphore_released < n_semaphores - 1) {

        // if we haven't acquired semaphores all the way to the end of the array,
        // acquire one more
        if (next_semaphore_to_acquire < n_semaphores) {
            sem_wait(&(sems[next_semaphore_to_acquire++]));
        }

        // TODO: here's the point that we 'periodically do something' such as
        // sleep() or yield()
        if (((my_creation_number % STUPID_MAGIC_CONSTANT) == 0)
            && ((highest_semaphore_released % STUPID_MAGIC_CONSTANT) == 0)) {
            yield(-1);    
        }
        // we only start releasing semaphores when we've acquired a total of
        // "footprint" semaphores
        if (next_semaphore_to_acquire >= footprint) {
            sem_signal(&(sems[++highest_semaphore_released]));
        }
    }

    snprintf(buf, sizeof (buf),
             "Successful finish for thread with creation number %d",
             my_creation_number);
    REPORT_MISC(buf);

    return (void *)1;
}

int main( int argc, char *argv[] ) {
    int i;
    void * status_temporary; // not really used anymore

    thr_init(STACK_SIZE);
    REPORT_START_CMPLT;

    /* handle options - number of semaphores , number of threads, "footprint" */
    switch(argc) {
        case 4:
            footprint = atoi(argv[3]);
            /* fall through */
        case 3:
            n_chasethreads = atoi(argv[2]);
            /* fall through */
        case 2:
            n_semaphores = atoi(argv[1]);
            /* fall through */
        default:
            break;
    }
    
    // the requirement for n_chasethreads is obscure but not entirely stupid
    // what we're trying to make sure is that there are some threads that
    // get 'caught behind' a 'yielder' thread under most normal scheduling
    // choices

    if ( (argc > 4) ||
         (footprint < 1) ||
         (n_chasethreads < (2 * STUPID_MAGIC_CONSTANT - 1)) ||
         (n_semaphores < 3) ||
         (n_semaphores <= footprint)
         ) {
        printf("Usage: %s [number of semaphores] [number of chase threads] "
               "[footprint]\n", argv[0]);
        printf("Number of semaphores must be at least 3 and strictly greater than "
               "footprint\n");
        printf("Number of chasethreads must be at least (2 * "
               "STUPID_MAGIC_CONSTANT - 1) = %d\n",
               (2 * STUPID_MAGIC_CONSTANT - 1));
        printf("Footprint (number of locks acquired at the same time) must be "
               "at least 1\n");
        /* don't REPORT anything: abort, attract attention of TA */
        thr_exit((void *)1);
    }
    thrgrp_init_group(&tg);
    sems = (sem_t *)calloc(n_semaphores, sizeof(sem_t));
    for (i = 0; i < n_semaphores; i++) {
        assert(sem_init(&(sems[i]), 1) == 0);
    }
    for (i = 0; i < n_chasethreads; i++) {
        assert(thrgrp_create(&tg, chase, (void *)i) >= 0);
    }
    for (i = 0; i < n_chasethreads; i++) {
        assert(thrgrp_join(&tg, &status_temporary) == 0);
    }
    free(sems);
    REPORT_END_SUCCESS; 
    thr_exit((void *)0);
    exit(99);
}
