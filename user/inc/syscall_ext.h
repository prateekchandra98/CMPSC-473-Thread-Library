#ifndef _SYSCALL_EXT_H
#define _SYSCALL_EXT_H

/* ADD OTHER LIBSYSCALL-RELATED HEADER CONTENT HERE */
int thread_fork(void* (*func)(void*), void* arg, void* new_esp);

#endif /* _SYSCALL_EXT_H */
