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

static void *test_1(void *arg)
{
	int i = 0;
	for (i = 0; i <= 5; i++)
	{
		if (i == 3)
		{
			printf("[test_1] : call pthread_exit() exit.\n");
			pthread_exit(0);
		}
		sleep(1);
	}
	run_flag = 1;
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	pthread_t pid_1;
	int ret;

	ret = pthread_create(&pid_1, NULL, (void *)test_1, NULL);
	if (ret != 0)
	{
		printf("Create pthread error!\n");
		goto FAIL;
	}
	pthread_join(pid_1, NULL);

	if (run_flag == 1)
		goto FAIL;

	printf("TEST PASSED !\n");
	return EXIT_SUCCESS;

FAIL:
	printf("TEST FAILED !\n");
	return EXIT_FAILURE;
}