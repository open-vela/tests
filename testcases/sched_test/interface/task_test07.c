#include <nuttx/config.h>

#include <nuttx/sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "sched_test.h"

/*-------------------------------------------------
Example description:
1. Create a task
2. ...
-------------------------------------------------
*/

static int test_flag = 0;

static int task_main(int argc, char *argv[])
{
	/* lock */
	sched_lock();
	int i;
	for (i = 0; i < 10; i++)
	{
		printf("Test steps no.%d\n", i + 1);
	}
	if (sched_lockcount() != 1)
	{
		test_flag = 1;
		return -1;
	}
	/* unlock */
	sched_unlock();
	return 0;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status;

	/* init  */
	test_flag = 0;
	pid = task_create("test_task2", TASK_PRIORITY, DEFAULT_STACKSIZE, task_main, NULL);
	if (pid > 0)
	{
		printf("create a new task, pid=%d\n", pid);
	}
	else
	{
		printf("create task fail !\n");
	}

	while ((pid = wait(&status)) > 0)
		;

	if (test_flag == 1)
	{
		printf("TEST FAILED !\n");
		return EXIT_FAILURE;
	}
	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;
}
