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
	for (i = 0; i < 10; i++)
	{
		sem_wait(&sem);
		flag++;
		sem_post(&sem);
	}
	return NULL;
}

int main(int argc, FAR char *argv[])
{
	int ret;
	pthread_t pthread_id[10];

	/* init flag */
	flag = 0;
	ret = sem_init(&sem, 0, 1);
	if (ret == -1)
	{
		printf("sem_init failed \n");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < 10; i++)
	{
		ret = pthread_create(&pthread_id[i], NULL, (void *)threadroutine, NULL);
		if (ret != 0)
		{
			printf("create thread wrong %d!!\n", i);
			return 0;
		}
	}

	int j;
	for (j = 0; j < 10; j++)
		pthread_join(pthread_id[j], NULL);
	sem_destroy(&sem);

	flag == 100 ? printf("TEST PASSED !\n") : printf("TEST FAILED !\n");

	exit(flag == 100);
}