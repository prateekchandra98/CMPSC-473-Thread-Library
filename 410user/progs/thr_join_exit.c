#include <stdio.h>
#include <thread.h>
#include <syscall.h> /* for PAGE_SIZE */
#include <assert.h>
#include "410_tests.h"
#include <report.h>
#include <test.h>

DEF_TEST_NAME("thr_join_exit:");

#define ERR REPORT_FAILOUT_ON_ERR

void *
waiter(void *p)
{
  thr_exit((void *)'!');

  while(1)
	continue; /* placate compiler portably */
}

int main()
{
  report_start(START_CMPLT);
  ERR(thr_init(16 * PAGE_SIZE));
  int status;
  int child;

  ERR(child = thr_create(waiter, NULL));
  ERR(thr_join(child, (void **)&status));
  assert(status == '!');
  thr_exit(NULL);
  while(1)
	continue; /* placate compiler portably */
}
