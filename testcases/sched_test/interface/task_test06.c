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

/*-------------------------------------------------------------------------------------------------------------
Example description:
1. Create a task
2. Obtain relevant information
3. exit
----------------------------------------------------------------------------------------------------------------
*/
static int test_flag = 0;

static bool run_flag = true;

static int task_entry(int argc, char *argv[])
{
	// struct sched_param task_param;
	printf("[%s]:start task ... pid=%d\n", argv[0], getpid());
	printf("[%s]:task run over !\n", argv[0]);
	return EXIT_SUCCESS;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status;
	int priority_max;
	int priority_min;

	/* init */
	test_flag = 0;
	run_flag = true;

	struct sched_param;
	pid = task_create("test_task", TASK_PRIORITY, DEFAULT_STACKSIZE, task_entry, NULL);
	if (pid > 0)
	{
		printf("[%s]:create a new task, pid=%d\n", argv[0], pid);
	}
	else
	{
		printf("[%s]:create task fail !\n", argv[0]);
		goto FAIL;
	}
	priority_max = sched_get_priority_max(SCHED_FIFO);
	priority_min = sched_get_priority_min(SCHED_FIFO);
	printf("the sched priority max = %d\n", priority_max);
	printf("the sched priority min = %d\n", priority_min);

#if defined(CONFIG_SCHED_WAITPID) && defined(CONFIG_SCHED_HAVE_PARENT)
	while ((pid = wait(&status)) > 0)
		;
#else
	sleep(2);
#endif

	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}