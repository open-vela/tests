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

static int run_flag_1 = 0;

static int run_flag_2 = 0;

static void *threadroutine_2(void *arg)
{

	/* set enable */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	sleep(1);

	/* cancel point */
	pthread_testcancel();

	/* It can not be executed here */
	run_flag_2 = 1;
	return NULL;
}

static void *threadroutine_1(void *arg)
{
	/* set disable */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	sleep(1);

	/* cancel point */
	pthread_testcancel();

	/* It can be executed here */
	run_flag_1 = 1;
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	int res;
	pthread_t p_t_1, p_t_2;

	/* int flag */
	run_flag_1 = 0;
	run_flag_2 = 0;

	/* create thread_1 */
	pthread_create(&p_t_1, NULL, threadroutine_1, NULL);

	pthread_create(&p_t_2, NULL, threadroutine_2, NULL);

	printf("[main] : call pthread_cancel, cancel thread_1.\n");
	res = pthread_cancel(p_t_1);
	if (res != OK)
	{
		printf("[main] : call pthread_cancel fail !\n");
		goto FAIL;
	}

	printf("[main] : call pthread_cancel, cancel thread_2.\n");
	res = pthread_cancel(p_t_2);
	if (res != OK)
	{
		printf("[main] : call pthread_cancel fail !\n");
		goto FAIL;
	}

	/* join thread_1 */
	pthread_join(p_t_1, NULL);

	/* join thread_2 */
	pthread_join(p_t_2, NULL);

	if (run_flag_1 == 1 && run_flag_2 == 0)
	{
		printf("TEST PASSED !\n");
		return EXIT_SUCCESS;
	}
	else
	{
		goto FAIL;
	}

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}