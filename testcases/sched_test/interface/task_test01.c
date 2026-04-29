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

/*
-------------------------------------------------
Example description:
1. Create a task
2. Request a piece of memory in the newly created task
3. Write this memory and release it
4. exit
-------------------------------------------------
*/
static int test_flag = 0;
static int task_exit = 0;

static int task_main(int argc, char *argv[])
{
	char *str = NULL;
	printf("malloc : Request address, write string\n");
	const int str_size = 10;
	str = zalloc(str_size + 1);
	memset(str, 'A', str_size);
	printf("the str=%s\n", str);
	free(str);
	printf("free address , exit task\n");
	test_flag = 1;
	task_exit = 1;
	return EXIT_SUCCESS;
}

int main(int argc, FAR char *argv[])
{
	pid_t pid;
	int status;

	/* init */
	test_flag = 0;
	task_exit = 0;

	pid = task_create("tst_task1", TASK_PRIORITY, DEFAULT_STACKSIZE, task_main, NULL);
	if (pid > 0)
	{
		printf("create a new task, pid=%d\n", pid);
	}
	else
	{
		printf("create task fail !\n");
		exit(EXIT_FAILURE);
	}

#if defined(CONFIG_SCHED_WAITPID) && defined(CONFIG_SCHED_HAVE_PARENT)
	while ((pid = wait(&status)) > 0)
		;
#else
	while (task_exit != 1)
	{
		sleep(1);
	}
#endif

	if (test_flag == 1)
	{
		printf("TEST PASSED !\n");
		return EXIT_SUCCESS;
	}
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}
