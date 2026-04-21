#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "sched_test.h"

/*-------------------------------------------------
Example description:
1. Create a task
2. Perform a write operation, such as printing, reading and writing files
3. During task execution, restart this task
4. exit
-------------------------------------------------
*/

static int task_count = 0;

static int task_main(int argc, char *argv[])
{
	int i;
	for (i = 0; i < 10; i++)
	{
		task_count++;
		printf("Count once  no.%d\n", i + 1);
		sleep(1);
	}
	return EXIT_SUCCESS;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status;
	/* Initialize variables */
	task_count = 0;
	pid = task_create("test_task2", TASK_PRIORITY, DEFAULT_STACKSIZE, task_main, NULL);
	if (pid > 0)
	{
		printf("create a new task, pid=%d\n", pid);
	}
	else
	{
		printf("create task fail !\n");
		goto FAIL;
	}
	while (1)
	{
		usleep(20000);
		if (task_count == 5)
		{
			printf("restart the task !\n");
			task_restart(pid);
			break;
		}
	}
	while ((pid = wait(&status)) > 0)
		;
	if (task_count == 15)
	{
		printf("TEST PASSED !\n");
		return EXIT_SUCCESS;
	}

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}