#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include <sched.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: stress  (Multithreaded operation of the database)
 * Example description:
 	1. Create 10 threads.
	2. In each thread, data is inserted and deleted 80 times.
 * Expect results: TEST PASSED
 ****************************************************************************/

#define THREAD_NUM 10
#define MAX_NUM_THREAD 10

static int num_thread = 8;
static int test_flag = 0;
static int test_num = 100;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int num = 0;

typedef struct threadpara
{
	int thread_id;
	char *thread_name;
} threadpara_t;

static void *store_data(void *arg)
{

	char *key = NULL;
	char *data = NULL;
	int ret;
	pid_t pid;
	pid = getpid();
	printf("the Thread_id: %d\n", pid);

	for (int k = 0; k < test_num; k++)
	{
		key = genRandomString(10);
		data = genRandomString(10 + k % 10);

		// printf("[pid %d] : set data %s\n", pid, key);
		ret = property_set("1234", data);
		if (ret != 0)
		{
			printf("store FAILED !  the Thread_id: %d\n", pid);
			test_flag = 1;
			break;
		}
		property_delete(key);

		free(key);
		free(data);
	}
	return 0;
}

int main(int argc, FAR char *argv[])
{

	threadpara_t para = {.thread_id = 0, .thread_name = NULL};
	pthread_t nthread[10];
	pthread_attr_t attr;
	int status;

	if (argc == 3)
	{
		test_num = atoi(argv[1]);
		num_thread = atoi(argv[2]);
	}

	srand(time(NULL));
	status = pthread_attr_init(&attr);
	if (status != 0)
	{
		printf("pthread_test: pthread_attr_init failed, status=%d\n", status);
		goto FAIL;
	}
	status = pthread_attr_setstacksize(&attr, DEFAULT_STACKSIZE);
	if (status != 0)
	{
		printf("pthread_test: pthread_attr_setstacksize failed, status=%d\n", status);
		goto FAIL;
	}

	for (int i = 0; i < num_thread; i++)
	{
		num++;
		para.thread_id = num;
		status = pthread_create(&nthread[i], &attr, store_data, &para);
		if (status != 0)
		{
			printf("pthread_test: pthread_create failed, status=%d\n", status);
			goto FAIL;
		}
	}
	for (int j = 0; j < num_thread; j++)
		pthread_join(nthread[j], NULL);

	if (test_flag == 0)
		printf("TEST PASSED !\n");
	else
		printf("TEST FAILED !\n");
	clean_up();
	return test_flag;

FAIL:
	clean_up();
	exit(1);
}