#include <nuttx/config.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sched.h>
#include <sys/wait.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: stress  (Multi-process operation database)
 * Example description:
 	1. Create 3 tasks. named task1 task2 task3.
	2. Database operations are performed in each task.
	3. task1 set some data, task2 get some data, task3 delete some data.
 * Expect results: TEST PASSED
 ****************************************************************************/

#define STACKSIZE 1024 * 4

static int test_result = 0;

static int data_set(int argc, int *argv[])
{
	char *key = NULL;
	char *data = NULL;
	int ret;
	for (int i = 0; i < 300; i++)
	{
		// printf("[task %s]: data set no.%d\n", argv[1], i+1);
		key = genRandomString(KEY_MAX_LEN);
		data = genRandomString(10);
		printf("[task_id : %d] set data key=%s\n", getpid(), key);
		ret = property_set(key, data);
		if (ret != 0)
		{
			printf("set a data failed ! \n");
			test_result = 1;
		}
		usleep(10000);
		//0.01 S
		free(key);
		free(data);
	}
	return 0;
}

static int data_get(int argc, int *argv[])
{
	char *key = NULL;
	char *data = NULL;
	char value[VALUE_MAX_LEN] = {0};
	int ret;
	for (int i = 0; i < 200; i++)
	{
		// printf("[task %s]: data set no.%d\n", argv[1], i+1);
		key = genRandomString(10);
		data = genRandomString(90);
		ret = property_set(key, data);
		if (ret != 0)
		{
			printf("set a data failed ! \n");
			test_result = 1;
		}
		printf("[task_id : %d] get data key=%s\n", getpid(), key);
		ret = property_get(key, value, "");
		usleep(20000);
		//0.02 S
		free(key);
		free(data);
	}
	return 0;
}

static int data_delete(int argc, int *argv[])
{
	char *key = NULL;
	char *data = NULL;
	int ret;
	for (int i = 0; i < 100; i++)
	{
		// printf("[task %s]: data set no.%d\n", argv[1], i+1);
		key = genRandomString(7);
		data = genRandomString(30);
		ret = property_set(key, data);
		printf("[task_pid : %d] delete data key=%s\n", getpid(), key);
		ret = property_delete(key);
		if (ret != 0)
		{
			printf("delete a data failed ! \n");
			test_result = 1;
		}
		usleep(30000);
		//0.03 S
		free(key);
		free(data);
	}
	return 0;
}
/*
Multitas operanta database
*/
int main(int argc, FAR char *argv[])
{
	pid_t task_id[5], pid;
	int status;
	char *task_name[5] = {"task1", "task2", "task3", "task4", "task5"};

	srand(time(NULL));

	task_id[0] = task_create(task_name[0], 100, STACKSIZE, (void *)data_set, NULL);
	if (task_id[0] == ERROR)
	{
		printf("ERROR: task1 create fail !\n");
		goto FAIL;
	}
	else
	{
		printf("create task1 success !\n");
	}

	task_id[1] = task_create(task_name[1], 99, STACKSIZE, (void *)data_get, NULL);
	if (task_id[1] == ERROR)
	{
		printf("ERROR: task2 create fail !\n");
		goto FAIL;
	}
	else
	{
		printf("create task2 success !\n");
	}

	task_id[2] = task_create(task_name[1], 98, STACKSIZE, (void *)data_delete, NULL);
	if (task_id[2] == ERROR)
	{
		printf("ERROR: task3 create fail !\n");
		goto FAIL;
	}
	else
	{
		printf("create task3 success !\n");
	}

	while ((pid = wait(&status)) > 0)
		;

	if (test_result == 0)
		printf("TEST PASSED\n");
	else
		printf("TEST FAILED\n");
	clean_up();
	exit(test_result);

FAIL:
	clean_up();
	exit(1);
}
