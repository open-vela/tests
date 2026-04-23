#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include "sched_test.h"

/*
-------------------------------------------------
Example description:
1.
-------------------------------------------------
*/
static int test_flag = 0;

static void *threadroutine(void *arg)
{
	int pid;
	/* call pthread_self() */
	pid = pthread_self();
	if (pid > 0)
	{
		printf("[threadroutine] : call pthread_self() success, return = %d\n", pid);
	}
	else
	{
		test_flag = 1;
	}
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	pthread_t p_t;

	/* init */
	test_flag = 0;

	/* create thread */
	pthread_create(&p_t, NULL, threadroutine, NULL);

	pthread_join(p_t, NULL);
	if (test_flag == 1)
		goto FAIL;
	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}