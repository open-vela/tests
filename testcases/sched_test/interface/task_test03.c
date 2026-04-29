#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "sched_test.h"

/*-------------------------------------------------
Example description:
1. Create a task
2. Perform a write operation, such as printing
3. During task execution, delete this task
4. exit
-------------------------------------------------
*/

int task1(int argc, char *argv[])
{
	printf("%s: pid = %d \n", argv[0], getpid());
	int i;
	for (i = 0; i < 10; ++i)
	{
		usleep(20000);
		printf("%s: do some thing %d \n", argv[0], i + 1);
	}

	exit(EXIT_SUCCESS);
}

int main(int argc, FAR char *argv[])
{

	pid_t ret;
	ret = task_create("task1", TASK_PRIORITY, DEFAULT_STACKSIZE, task1, &argv[1]);
	if (ret < 0)
	{
		int errcode = errno;
		printf("%s: ERROR: Failed to start task1: %d\n", argv[0], errcode);
		return EXIT_FAILURE;
	}
	else
		printf("%s: Started task at PID=%d\n", argv[0], ret);

	int i;
	for (i = 0; i < 5; ++i)
	{
		usleep(20000);
		printf("%s: do some thing %d  \n", argv[0], i + 1);
	}

	ret = task_delete(ret);
	if (ret == 0)
	{
		printf("%s: Delete the task  return:%d\n", argv[0], ret);
		printf("TEST PASSED !\n");
		return EXIT_SUCCESS;
	}
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}