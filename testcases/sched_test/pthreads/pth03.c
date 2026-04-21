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
#include <semaphore.h>
#include "sched_test.h"

/*
-------------------------------------------------
Example description:
1.
-------------------------------------------------
*/
static sem_t sem;

static int flag = 0;

static void *threadroutine(void *arg)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		sem_wait(&sem);
		printf("wait...\n");
		flag++;
	}
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	int ret;
	pthread_t pthread_id;

	/* init */
	flag = 0;
	ret = sem_init(&sem, 0, 0);
	if (ret == -1)
	{
		printf("sem_init failed \n");
		exit(EXIT_FAILURE);
	}

	/* create pthread */
	ret = pthread_create(&pthread_id, NULL, (void *)threadroutine, NULL);
	if (ret == OK)
	{
		printf("create pthread success !\n");
	}
	else
	{
		printf("create pthread fail !\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		sleep(2);
		sem_post(&sem);
		printf("post .. \n");
		if (flag == 5)
			break;
	}
	sem_destroy(&sem);

	flag == 5 ? printf("TEST PASSED !\n") : printf("TEST FAILED !\n");

	exit(flag == 5);
}