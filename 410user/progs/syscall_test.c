// Tests that syscall functions do not crash or corrupt registers or the stack. Does NOT extensively test the functionality of the syscall. Refer to other tests like cat or nibbles that utilize the syscalls for some functionality.

#include <syscall.h>
#include <stdio.h>
#include <string.h>
#include <test.h>

#define CANARY 0xdeadbeef

int test_print()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, print, 2, 5, "test\n");
    if (check != 0) {
        printf("ERROR: print corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: print returned with value %d\n", ret);
    return 0;
}

int test_fork()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, fork, 0);
    if (check != 0) {
        printf("ERROR: fork corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: fork returned with value %d\n", ret);
    return 0;
}

int test_exec_success()
{
    printf("SUCCESS: exec succeeded\n");
    return 0;
}

int test_exec()
{
    char* argv[3];
    argv[0] = "syscall_test";
    argv[1] = "test_exec_success";
    argv[2] = NULL;
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, exec, 2, argv[0], argv);
    if (check != 0) {
        printf("ERROR: exec corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("ERROR: exec returned with value %d\n", ret);
    return ret;
}

int test_set_status()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, set_status, 1, 7);
    if (check != 0) {
        printf("ERROR: set_status corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: set_status succeeded\n");
    return 0;
}

int test_wait()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, fork, 0);
    if (check != 0) {
        printf("ERROR: fork corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret == 0) {
        return 9;
    } else {
        int status = 0;
        check = register_stack_check(CANARY, (void**)&ret, wait, 1, &status);
        if (check != 0) {
            printf("ERROR: wait corrupted callee-save registers and/or the stack\n");
            return check;
        }
        if (status != 9) {
            printf("ERROR: wait got an incorrect status %d\n", status);
            return -1;
        }
        printf("SUCCESS: wait returned with value %d\n", ret);
        return 0;
    }
}

int test_vanish()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, fork, 0);
    if (check != 0) {
        printf("ERROR: fork corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret == 0) {
        register_stack_check(CANARY, (void**)&ret, vanish, 0);
        printf("ERROR: vanish returned\n");
        return -1;
    } else {
        int status = 0;
        check = register_stack_check(CANARY, (void**)&ret, wait, 1, &status);
        if (check != 0) {
            printf("ERROR: wait corrupted callee-save registers and/or the stack\n");
            return check;
        }
        printf("SUCCESS: vanish succeeded\n");
        return 0;
    }
}

int test_task_vanish()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, fork, 0);
    if (check != 0) {
        printf("ERROR: fork corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret == 0) {
        register_stack_check(CANARY, (void**)&ret, task_vanish, 1, 7);
        printf("ERROR: task_vanish returned\n");
        return -1;
    } else {
        int status = 0;
        check = register_stack_check(CANARY, (void**)&ret, wait, 1, &status);
        if (check != 0) {
            printf("ERROR: wait corrupted callee-save registers and/or the stack\n");
            return check;
        }
        if (status != 7) {
            printf("ERROR: wait got an incorrect status %d\n", status);
            return -1;
        }
        printf("SUCCESS: task_vanish succeeded\n");
        return 0;
    }
}

int test_gettid()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, gettid, 0);
    if (check != 0) {
        printf("ERROR: gettid corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: gettid returned with value %d\n", ret);
    return 0;
}

int test_yield()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, yield, 1, -1);
    if (check != 0) {
        printf("ERROR: yield corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret != 0) {
        printf("ERROR: yield returned an error %d\n", ret);
        return ret;
    }
    printf("SUCCESS: yield returned with value %d\n", ret);
    return 0;
}

int test_deschedule()
{
    int reject = 1;
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, deschedule, 1, &reject);
    if (check != 0) {
        printf("ERROR: deschedule corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret != 0) {
        printf("ERROR: deschedule returned an error %d\n", ret);
        return ret;
    }
    printf("SUCCESS: deschedule returned with value %d\n", ret);
    return 0;
}

int test_make_runnable()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, make_runnable, 1, 0);
    if (check != 0) {
        printf("ERROR: make_runnable corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: make_runnable returned with value %d\n", ret);
    return 0;
}

int test_get_ticks()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, get_ticks, 0);
    if (check != 0) {
        printf("ERROR: get_ticks corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: get_ticks returned with value %d\n", ret);
    return 0;
}

int test_sleep()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, sleep, 1, 0);
    if (check != 0) {
        printf("ERROR: sleep corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret != 0) {
        printf("ERROR: sleep returned an error %d\n", ret);
        return ret;
    }
    printf("SUCCESS: sleep returned with value %d\n", ret);
    return 0;
}

int test_new_pages()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, new_pages, 2, 0x80000000, PAGE_SIZE);
    if (check != 0) {
        printf("ERROR: new_pages corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: new_pages returned with value %d\n", ret);
    return 0;
}

int test_remove_pages()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, new_pages, 2, 0x80000000, PAGE_SIZE);
    if (check != 0) {
        printf("ERROR: new_pages corrupted callee-save registers and/or the stack\n");
        return check;
    }
    check = register_stack_check(CANARY, (void**)&ret, remove_pages, 1, 0x80000000);
    if (check != 0) {
        printf("ERROR: remove_pages corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: remove_pages returned with value %d\n", ret);
    return 0;
}

int test_readline()
{
    printf("Press the enter key to continue...\n");
    char buf[128];
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, readline, 2, 128, buf);
    if (check != 0) {
        printf("ERROR: readline corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: readline returned with value %d\n", ret);
    return 0;
}

int test_getchar()
{
    int row = 4;
    int col = 8;
    get_cursor_pos(&row, &col);
    printf("Press the enter key to continue...\n");
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, getchar, 0);
    if (check != 0) {
        printf("ERROR: getchar corrupted callee-save registers and/or the stack\n");
        printf("Press the enter key to continue...\n");
        getchar();
        return check;
    }
    printf("SUCCESS: getchar returned with value %d\n", ret);
    printf("Press the enter key to continue...\n");
    getchar();
    return 0;
}

int test_set_term_color()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, set_term_color, 1, FGND_BLUE);
    if (check != 0) {
        printf("ERROR: set_term_color corrupted callee-save registers and/or the stack\n");
        printf("Press the enter key to continue...\n");
        getchar();
        return check;
    }
    if (ret != 0) {
        printf("ERROR: set_term_color returned an error %d\n", ret);
        printf("Press the enter key to continue...\n");
        getchar();
        return ret;
    }
    printf("SUCCESS: set_term_color returned with value %d\n", ret);
    printf("Press the enter key to continue...\n");
    getchar();
    return 0;
}

int test_get_cursor_pos()
{
    int row = 4;
    int col = 8;
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, get_cursor_pos, 2, &row, &col);
    if (check != 0) {
        printf("ERROR: get_cursor_pos corrupted callee-save registers and/or the stack\n");
        printf("Press the enter key to continue...\n");
        getchar();
        return check;
    }
    if (ret != 0) {
        printf("ERROR: get_cursor_pos returned an error %d\n", ret);
        printf("Press the enter key to continue...\n");
        getchar();
        return ret;
    }
    printf("SUCCESS: get_cursor_pos returned with value %d\n", ret);
    printf("Press the enter key to continue...\n");
    getchar();
    return 0;
}

int test_set_cursor_pos()
{
    int row = 4;
    int col = 8;
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, get_cursor_pos, 2, &row, &col);
    if (check != 0) {
        printf("ERROR: get_cursor_pos corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret != 0) {
        printf("ERROR: get_cursor_pos returned an error %d\n", ret);
        return ret;
    }
    check = register_stack_check(CANARY, (void**)&ret, set_cursor_pos, 2, row, col);
    if (check != 0) {
        printf("ERROR: set_cursor_pos corrupted callee-save registers and/or the stack\n");
        printf("Press the enter key to continue...\n");
        getchar();
        return check;
    }
    if (ret != 0) {
        printf("ERROR: set_cursor_pos returned an error %d\n", ret);
        printf("Press the enter key to continue...\n");
        getchar();
        return ret;
    }
    printf("SUCCESS: set_cursor_pos returned with value %d\n", ret);
    printf("Press the enter key to continue...\n");
    getchar();
    return 0;
}

int test_halt()
{
    printf("Halt test will terminate, so there's no way to determine success\n");
    int ret = 5;
    register_stack_check(CANARY, (void**)&ret, halt, 0);
    printf("ERROR: halt returned\n");
    return -1;
}

int test_readfile()
{
    char buf[32];
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, readfile, 4, "syscall_test", buf, 32, 0);
    if (check != 0) {
        printf("ERROR: readfile corrupted callee-save registers and/or the stack\n");
        return check;
    }
    if (ret != 32) {
        printf("ERROR: readfile returned an error %d\n", ret);
        return ret;
    }
    printf("SUCCESS: readfile returned with value %d\n", ret);
    return 0;
}

int test_misbehave()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, misbehave, 1, 0);
    if (check != 0) {
        printf("ERROR: misbehave corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: misbehave succeeded\n");
    return 0;
}

int test_swexn()
{
    int ret = 5;
    int check = register_stack_check(CANARY, (void**)&ret, swexn, 4, NULL, NULL, NULL, NULL);
    if (check != 0) {
        printf("ERROR: swexn corrupted callee-save registers and/or the stack\n");
        return check;
    }
    printf("SUCCESS: swexn returned with value %d\n", ret);
    return 0;
}

int string_equal(const char* str1, const char* str2)
{
    if ((str1 == NULL) && (str2 == NULL)) {
        return 1;
    }
    if ((str1 == NULL) || (str2 == NULL)) {
        return 0;
    }
    return (strcmp(str1, str2) == 0);
}

typedef int (*test_fn_t)();
typedef struct {
    char* name;
    test_fn_t test;
} test_t;

test_t tests[] = {
    {"test_print", test_print},
    {"test_fork", test_fork},
    {"test_exec_success", test_exec_success},
    {"test_exec", test_exec},
    {"test_set_status", test_set_status},
    {"test_wait", test_wait},
    {"test_vanish", test_vanish},
    {"test_task_vanish", test_task_vanish},
    {"test_gettid", test_gettid},
    {"test_yield", test_yield},
    {"test_deschedule", test_deschedule},
    {"test_make_runnable", test_make_runnable},
    {"test_get_ticks", test_get_ticks},
    {"test_sleep", test_sleep},
    {"test_new_pages", test_new_pages},
    {"test_remove_pages", test_remove_pages},
    {"test_readline", test_readline},
    {"test_getchar", test_getchar},
    {"test_set_term_color", test_set_term_color},
    {"test_get_cursor_pos", test_get_cursor_pos},
    {"test_set_cursor_pos", test_set_cursor_pos},
    {"test_halt", test_halt},
    {"test_readfile", test_readfile},
    {"test_misbehave", test_misbehave},
    {"test_swexn", test_swexn},
};
size_t num_tests = sizeof(tests)/sizeof(tests[0]);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s test_name\n", argv[0]);
        return 0;
    }

    for (size_t i = 0; i < num_tests; i++) {
        if (string_equal(argv[1], tests[i].name)) {
            return tests[i].test();
        }
    }
    printf("Did not find test\n");
    return 0;
}
