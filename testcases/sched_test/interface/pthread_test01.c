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
static int run_flag = 0;

static void *threadroutine(void *arg)
{
	printf("I am a thread routine !\n");
	/* set run flag 1 */
	run_flag = 1;
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	pthread_t p_t;

	/* create thread */
	pthread_create(&p_t, NULL, threadroutine, NULL);

	/* pthread_join Wait for the thread to end*/
	pthread_join(p_t, NULL);

	if (run_flag == 1)
		printf("TEST PASSED !\n");
	else
		goto FAIL;
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}