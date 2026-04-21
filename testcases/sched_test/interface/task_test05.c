#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "sched_test.h"

/*
-------------------------------------------------
Example description:
1. Create a task
2. the task do something
3. The task gives up the CPU
4. exit
-------------------------------------------------
*/
static int test_flag = 0;

static int task_main(int argc, char *argv[])
{
	int ret;
	int i;
	for (i = 1; i <= 10; i++)
	{

		if (i >= 4 && i <= 7)
		{
			printf("Yield the CPU !\n");
			ret = sched_yield();
			if (ret == -1)
			{
				printf("call sched_yield fail!\n");
				test_flag = 1;
				return -1;
			}
		}
		printf("do no.%d...\n", i);
	}
	return 0;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status;

	/* init */
	test_flag = 0;

	pid = task_create("tst_task1", TASK_PRIORITY, DEFAULT_STACKSIZE, task_main, NULL);
	if (pid > 0)
	{
		printf("create a new task, pid=%d\n", pid);
	}
	else
	{
		printf("create task fail !\n");
		goto FAIL;
	}
	while ((pid = wait(&status)) > 0)
		;

	if (test_flag == 1)
		goto FAIL;

	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}