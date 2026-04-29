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
2. Do some operation, such as printing, reading and writing files
3. During task execution, get the current task scheduling information: priority, scheduling policy, and so on
4. Modify the task scheduling priority
5. Modify the task scheduling policy
6. exit
----------------------------------------------------------------------------------------------------------------
*/
static int test_flag = 0;

bool run_flag = true;

static int task_entry(int argc, char *argv[])
{
	// struct sched_param task_param;
	int i = 0;
	printf("[%s]:start task ... pid=%d\n", argv[0], getpid());
	while (run_flag)
	{
		printf("running ... ... %d\n", i++);
		sleep(1);
		if (i == 10)
			break;
	}
	printf("[%s]:task run over !\n", argv[0]);
	return EXIT_SUCCESS;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status, ret;

	/* init */
	test_flag = 0;

	struct sched_param task_entry_param;
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
	ret = sched_getparam(pid, &task_entry_param);
	if (ret == OK)
	{
		printf("[%s]:the task priority is %d !\n", argv[0], task_entry_param.sched_priority);
	}
	else
	{
		printf("[%s]:get task priority error !\n", argv[0]);
		test_flag = 1;
	}
	sleep(5);
	printf("change the task ");
#if CONFIG_RR_INTERVAL > 0
	ret = sched_setscheduler(pid, SCHED_RR, &task_entry_param);
#elif defined(CONFIG_SCHED_SPORADIC)
	ret = sched_setscheduler(pid, SCHED_SPORADIC, &task_entry_param);
#else
	ret = sched_setscheduler(pid, SCHED_FIFO, &task_entry_param);
#endif
	if (ret == ERROR)
	{
		printf("[%s]:change scheduler failed !\n", argv[0]);
		test_flag = 1;
	}

	switch (sched_getscheduler(pid))
	{
	case SCHED_FIFO:
		printf("[%s]:scheduling policy is FIFO!\n", argv[0]);
		break;
	case SCHED_RR:
		printf("[%s]:scheduling policy is RR!\n", argv[0]);
		break;
	case SCHED_OTHER:
		printf("[%s]:scheduling policy is OTHER!\n", argv[0]);
		break;
	default:
		break;
	}
	task_entry_param.sched_priority = 100;
	ret = sched_setparam(pid, &task_entry_param);
	if (ret == ERROR)
	{
		printf("[%s]:change task priority error !\n", argv[0]);
		test_flag = 1;
	}

	while ((pid = wait(&status)) > 0)
		;

	if (test_flag == 0)
	{
		printf("TEST PASSED !\n");
		return EXIT_SUCCESS;
	}

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}
