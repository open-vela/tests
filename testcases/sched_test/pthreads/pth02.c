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
static int flag = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void check_return(int ret)
{
	if (ret == OK)
	{

	}
	else
	{
		printf("error : ret = %d\n", ret);
		exit(EXIT_FAILURE);
	}
}

static void *threadroutine(void *arg)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		pthread_mutex_lock(&mutex);
		flag++;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	int res;
	pthread_t pt_1, pt_2, pt_3;

	/* init flag */
	flag = 0;

	res = pthread_create(&pt_1, NULL, (void *)threadroutine, NULL);
	check_return(res);
	res = pthread_create(&pt_2, NULL, (void *)threadroutine, NULL);
	check_return(res);
	res = pthread_create(&pt_3, NULL, (void *)threadroutine, NULL);
	check_return(res);

	pthread_join(pt_1, NULL);
	pthread_join(pt_2, NULL);
	pthread_join(pt_3, NULL);
	sleep(5);
	printf("teh test flag : %d\n", flag);
	if (flag == 300)
	{
		printf("TEST PASSED !\n");
		exit(EXIT_SUCCESS);
	}
	else
	{
		printf("TEST FAILED !\n");
		exit(EXIT_FAILURE);
	}
}