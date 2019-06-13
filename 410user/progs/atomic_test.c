// Tests atomic functions for correct functionality. Does NOT test atomicity or thread-safety.

#include <atomic.h>
#include <stdio.h>
#include <test.h>

#define CANARY 0xdeadbeef

int test_atomic_exchange()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_exchange, 2, &var, 3);
    if (check != 0) {
        printf("ERROR: atomic_exchange corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 3) {
        printf("ERROR: atomic_exchange failed to update memory\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_exchange failed to return original value\n");
        return 2;
    }
    return 0;
}

int test_atomic_compare_swap_equal()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_compare_swap, 3, &var, 7, 3);
    if (check != 0) {
        printf("ERROR: atomic_compare_swap corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 3) {
        printf("ERROR: atomic_compare_swap failed to update memory\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_compare_swap failed to return original value\n");
        return 2;
    }
    return 0;
}

int test_atomic_compare_swap_not_equal()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_compare_swap, 3, &var, 9, 3);
    if (check != 0) {
        printf("ERROR: atomic_compare_swap corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 7) {
        printf("ERROR: atomic_compare_swap updated memory when it shouldn't have\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_compare_swap failed to return original value\n");
        return 2;
    }
    return 0;
}

int test_atomic_fetch_add()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_fetch_add, 2, &var, 3);
    if (check != 0) {
        printf("ERROR: atomic_fetch_add corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 10) {
        printf("ERROR: atomic_fetch_add failed to update memory\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_fetch_add failed to return original value\n");
        return 2;
    }
    return 0;
}

int test_atomic_fetch_sub()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_fetch_sub, 2, &var, 3);
    if (check != 0) {
        printf("ERROR: atomic_fetch_sub corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 4) {
        printf("ERROR: atomic_fetch_sub failed to update memory\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_fetch_sub failed to return original value\n");
        return 2;
    }
    return 0;
}

int test_atomic_load()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_load, 1, &var);
    if (check != 0) {
        printf("ERROR: atomic_load corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 7) {
        printf("ERROR: atomic_load updated memory when it shouldn't have\n");
        return 1;
    }
    if (ret != 7) {
        printf("ERROR: atomic_load failed to return the value\n");
        return 2;
    }
    return 0;
}

int test_atomic_store()
{
    int ret = 5;
    volatile int var = 7;

    int check = register_stack_check(CANARY, (void**)&ret, atomic_store, 2, &var, 3);
    if (check != 0) {
        printf("ERROR: atomic_store corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (var != 3) {
        printf("ERROR: atomic_store failed to update memory\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    int ret;
    ret = test_atomic_exchange();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_compare_swap_equal();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_compare_swap_not_equal();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_fetch_add();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_fetch_sub();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_load();
    if (ret != 0) {
        return ret;
    }
    ret = test_atomic_store();
    if (ret != 0) {
        return ret;
    }
    printf("SUCCESS: Your atomic library passes basic functionality tests, but has not been tested for atomicity/thread-safety.\n");
    return 0;
}
