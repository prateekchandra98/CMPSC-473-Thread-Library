#include <stdio.h>
#include <thread.h>
#include <syscall.h> /* for PAGE_SIZE */
#include <assert.h>
#include <atomic.h>
#include <sem.h>

// Simple one-time use barrier using semaphores
typedef struct {
    sem_t sem;
    volatile int count;
} bar_t;

int barrier_init(bar_t* bar, int count)
{
    bar->count = count;
    return sem_init(&bar->sem, 0);
}

void barrier_sync(bar_t* bar)
{
    if (atomic_fetch_sub(&bar->count, 1) == 1) {
        sem_signal(&bar->sem);
    } else {
        sem_wait(&bar->sem);
        sem_signal(&bar->sem);
    }
}

#define NUM_THREADS 4
int tids[NUM_THREADS];
int data[NUM_THREADS];
int expected[NUM_THREADS];
bar_t bar1;
bar_t bar2;

void* test_thread(void* arg)
{
    int i = (int)arg;

    // read data
    int left = (i <= 0) ? data[NUM_THREADS-1] : data[i-1];
    int center = data[i];
    int right = (i >= (NUM_THREADS - 1)) ? data[0] : data[i+1];

    barrier_sync(&bar1);

    // write data
    data[i] = left + center + right;

    barrier_sync(&bar2);

    // read data
    left = (i <= 0) ? data[NUM_THREADS-1] : data[i-1];
    center = data[i];
    right = (i >= (NUM_THREADS - 1)) ? data[0] : data[i+1];
        
    return (void*)(left + center + right);
}

void* run(void* arg)
{
    // initialize test
    barrier_init(&bar1, NUM_THREADS);
    barrier_init(&bar2, NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i] = i;
    }
    // calculate expected value
    int temp[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        int left = (i <= 0) ? data[NUM_THREADS-1] : data[i-1];
        int center = data[i];
        int right = (i >= (NUM_THREADS - 1)) ? data[0] : data[i+1];
        temp[i] = left + center + right;
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        int left = (i <= 0) ? temp[NUM_THREADS-1] : temp[i-1];
        int center = temp[i];
        int right = (i >= (NUM_THREADS - 1)) ? temp[0] : temp[i+1];
        expected[i] = left + center + right;
    }
    // start threads
    for (int i = 0; i < NUM_THREADS; i++) {
        tids[i] = thr_create(test_thread, (void*)i);
    }
    // wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        int status = 0;
        thr_join(tids[i], (void**)&status);
        assert(status == expected[i]);
    }
    sem_signal((sem_t*)arg);
    return (void*)7;
}

int main()
{
    sem_t test_sem;
    thr_init(16 * PAGE_SIZE);
    sem_init(&test_sem, 3); // test semaphore count > 1

    int child = thr_create(run, &test_sem);

    sem_wait(&test_sem);
    sem_wait(&test_sem);
    sem_wait(&test_sem);
    sem_wait(&test_sem);
    int status = 0;
    thr_join(child, (void**)&status);
    assert(status == 7);
    printf("semaphore_test: SUCCESS\n");
    return 0;
}
