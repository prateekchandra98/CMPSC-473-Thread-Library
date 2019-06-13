#ifndef ATOMIC_H
#define ATOMIC_H

int atomic_exchange(volatile int* obj, int desired);
int atomic_compare_swap(volatile int* obj, int expected, int desired);
int atomic_fetch_add(volatile int* obj, int arg);
int atomic_fetch_sub(volatile int* obj, int arg);
int atomic_load(volatile int* obj);
void atomic_store(volatile int* obj, int desired);

#endif // ATOMIC_H
