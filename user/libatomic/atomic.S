/* int atomic_exchange(volatile int* obj, int desired); */
    .global atomic_exchange
atomic_exchange:
    /* IMPLEMENT THIS */
    mov 0x4(%esp), %ecx
    mov 0x8(%esp), %eax
    lock xchg (%ecx), %eax
    ret

/* int atomic_compare_swap(volatile int* obj, int expected, int desired); */
    .global atomic_compare_swap
atomic_compare_swap:
    /* IMPLEMENT THIS */
    mov 0x4(%esp), %ecx
    mov 0x8(%esp), %eax
    mov 0xc(%esp), %edx
    lock cmpxchg %edx, (%ecx)
    ret

/* int atomic_fetch_add(volatile int* obj, int arg); */
    .global atomic_fetch_add
atomic_fetch_add:
    mov 0x4(%esp), %ecx
    mov 0x8(%esp), %edx
    lock xadd %edx, (%ecx)
    mov %edx, %eax
    ret

/* int atomic_fetch_sub(volatile int* obj, int arg); */
    .global atomic_fetch_sub
atomic_fetch_sub:
    mov 0x4(%esp), %ecx
    mov 0x8(%esp), %edx
    neg %edx
    lock xadd %edx, (%ecx)
    mov %edx, %eax
    ret

/* int atomic_load(volatile int* obj); */
    .global atomic_load
atomic_load:
    mov 0x4(%esp), %ecx
    mov $0, %eax
    lock xadd %eax, (%ecx)
    ret

/* void atomic_store(volatile int* obj, int desired); */
    .global atomic_store
atomic_store:
    mov 0x4(%esp), %ecx
    mov 0x8(%esp), %edx
    lock xchg (%ecx), %edx
    ret
