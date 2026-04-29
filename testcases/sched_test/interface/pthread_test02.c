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

static void *threadroutine(void *arg)
{
	printf("I am a thread routine !\n");
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	int res;
	pthread_t p_t;
	pthread_attr_t attr;
	size_t statck_size;
	struct sched_param param, o_param;

	/* Initializes thread attributes object (attr) */
	res = pthread_attr_init(&attr);
	if (res == OK)
	{
		printf("[main] : init attr success !\n");
	}
	else
	{
		printf("[main] : init attr fail !\n");
		goto FAIL;
	}

	res = pthread_attr_setstacksize(&attr, PTHREAD_STACK_SIZE);
	if (res == OK)
	{
		printf("[main] : set stacksize success !\n");
	}
	else
	{
		printf("[main] : set stacksize fail !\n");
		goto FAIL;
	}

	res = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if (res != OK)
	{
		printf("[main] : ERROR pthread_attr_setschedpolicy failed, res=%d\n", res);
		goto FAIL;
	}

	param.sched_priority = TASK_PRIORITY + 1;

	res = pthread_attr_setschedparam(&attr, &param);
	if (res != OK)
	{
		printf("[main] : ERROR pthread_attr_setschedparam failed, res=%d\n", res);
		goto FAIL;
	}

	/* create thread */
	pthread_create(&p_t, &attr, threadroutine, NULL);

	/* Wait for the child thread finish */
	pthread_join(p_t, NULL);

	res = pthread_attr_getschedparam(&attr, &o_param);
	if (res == OK)
	{
		printf("sched_priority = %d\n", o_param.sched_priority);
	}
	else
	{
		goto FAIL;
	}

	res = pthread_attr_getstacksize(&attr, &statck_size);
	if (res == OK)
	{
		printf("stacksize = %zu\n", statck_size);
	}
	else
	{
		goto FAIL;
	}
	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}
