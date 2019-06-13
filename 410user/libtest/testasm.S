/** @file 410user/libtest/testasm.S
 *  @brief Test library.
 *  @author elly1 U2009
 */

#include <simics.h>
#include <syscall_int.h>

.global exhaustion

#define EX_BASE     0x20000000
#define EX_SIZE     1024 * 1024 * 1024

/* %edi - continuation
 * %esi - syscall arg frame
 * %ebx - argument
 */
exhaustion:
    mov 4(%esp), %edi
    mov 8(%esp), %ebx
    lea -8(%esp), %esi  /* argument frame space */
    mov $EX_BASE, %ecx
    mov $EX_SIZE, %edx
    mov $4, %esp        /* don't tempt the user to think that their stack is
                         * still live */
loop:
    cmp $4096, %edx
    jb 1f
    mov %ecx, (%esi)
    mov %edx, 4(%esi)
    int $NEW_PAGES_INT
    test %eax, %eax
    jnz smaller
    add %edx, %ecx
    jmp loop
smaller:
    shr $1, %edx
    jmp loop
1:
    jmp *%edi

.global fork_and_exit

fork_and_exit:
    mov %ebx, %ecx
    int $FORK_INT
    test %eax, %eax
    jz 1f
    INLINE_SIMCALL(SIM_PUTS, %ecx, $0)
1:
    mov $0, %esi
    int $SET_STATUS_INT
    int $VANISH_INT

.global exit_success

exit_success:
    mov %ebx, %ecx
    INLINE_SIMCALL(SIM_PUTS, %ecx, $0)
    INLINE_SIMCALL(SIM_TEST_REPORT, %ecx, $1)
    mov $0, %esi
    int $SET_STATUS_INT
    int $VANISH_INT

.global illegal

illegal:
    xor %eax, %eax
    mov %eax, %cr0
    ret

/* Don't code like this:  "authorized personnel only". */
.global assuredly_misbehave
assuredly_misbehave:
    pushl %ebp
    movl %esp, %ebp
    pusha
    movl 8(%ebp), %esi
    int $MISBEHAVE_INT
    popa
    leave
    ret

/** @ brief Checks if callee save registers are somehow corrupted.
 *    Also checks if stack is somehow corrupted or if invalid parts of the stack are used.
 *    int register_stack_check(int canary, void** ret, void* func, int num_params, params...)
 *
 *  @param canary random data to place in registers/stack
 *  @param ret location to output return of func
 *  @param func function pointer to call
 *  @param num_params number of parameters
 *  @param ... parameters for func
 *  @return 0 on success, -1 on failure
 */
        .global register_stack_check
register_stack_check:
        push %ebp
        mov %esp, %ebp
        push %ebx                               /* save callee-save */
        push %edi
        push %esi

        /* align stack */
        sub $0x8, %esp                          /* ensure at least 8 bytes of stack space */
        and $0xffffff00, %esp                   /* align to 256 bytes */
        mov %ebp, 0x4(%esp)                     /* store ebp in case it is corrupted */

        /* setup stack canary */
        mov 0x8(%ebp), %ecx                     /* get canary */
        mov %ecx, (%esp)                        /* store canary to check for stack corruption */

        /* setup call params */
        push %ecx                               /* push something to get the alignment code to work */
        mov 0x14(%ebp), %edx                    /* get num_params */
        test %edx, %edx                         /* while (num_params > 0) { */
        jle register_stack_check_loop_end
register_stack_check_loop_begin:
        dec %edx                                /* num_params-- */
        mov 0x18(%ebp, %edx, 4), %eax           /* copy param for calling func */
        push %eax
        test %edx, %edx
        jg register_stack_check_loop_begin      /* } */
register_stack_check_loop_end:
        /* fill registers with canary */
        mov 0x10(%ebp), %edx                    /* get func */
        mov %ecx, %ebp                          /* fill callee-save registers with canary */
        mov %ecx, %ebx                          /* fill callee-save registers with canary */
        mov %ecx, %edi                          /* fill callee-save registers with canary */
        mov %ecx, %esi                          /* fill callee-save registers with canary */
        mov %ecx, %eax                          /* fill eax with canary for fun */

        /* call function */
        call *%edx                              /* call function */

        /* remove parameters by aligning stack */
        and $0xffffff00, %esp                   /* align to 256 bytes */
        add $0x100, %esp                        /* add 256 bytes to remove parameters */
        mov 0x4(%esp), %ebp                     /* restore ebp */

        /* trash the stack */
        mov (%esp), %ecx                        /* get canary */
        push %ecx                               /* trash stack by pushing 256 bytes of canary */
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx
        push %ecx

        /* store return */
        mov 0xc(%ebp), %edx                     /* get ret */
        mov %eax, (%edx)                        /* store return value */

        /* check for corruption */
        cmp %ecx, 0x8(%ebp)
        jne register_stack_check_fail
        cmp %ecx, %ebx
        jne register_stack_check_fail
        cmp %ecx, %edi
        jne register_stack_check_fail
        cmp %ecx, %esi
        jne register_stack_check_fail

        xor %eax, %eax                          /* return success */
        jmp register_stack_check_exit
register_stack_check_fail:
        mov $-1, %eax                           /* return failure */
register_stack_check_exit:
        lea -0xc(%ebp), %esp                    /* restore stack pointer */
        pop %esi                                /* restore callee-save */
        pop %edi
        pop %ebx
        pop %ebp
        ret
