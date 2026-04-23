#include <nuttx/config.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/wait.h>

#define FILEPATH1 "/data/flash_rw_test01"

/****************************************************************************
 * Name:
 * Example description:
 * Expect results: TEST PASSED
 ****************************************************************************/

#define STACKSIZE 1024 * 4

static char *g_argv[2] =
	{
		NULL,
		NULL,
};

static int test_result = 0;

static int task_1(int argc, int *argv[])
{
	int fd;
	int ret;
	int i = 0;
	char *buf;

	buf = malloc(32 * sizeof(char));
	if (buf == NULL)
	{
		printf("[task_1] : malloc fail !\n");
		test_result = 1;
		return -1;
	}

	fd = open(FILEPATH1, O_CREAT | O_RDWR);
	if (fd < 0)
	{
		printf("[task_1] : open file fail !\n");
		test_result = 1;
		return -1;
	}

	while (1)
	{
		i++;
		memset(buf, 'A' + i % 24, 32);
		ret = write(fd, buf, 32);
		if (ret < 0)
		{
			printf("[task_1] : write file fail !\n");
			test_result = 1;
			break;
		}
		fsync(fd);
		if ((i * 32) == (1024 * 1024))
		{
			ftruncate(fd, 0);
			lseek(fd, 0, SEEK_SET);
			sleep(5);
			i = 0;
		}
		usleep(10);
	}
	printf("[atsk_1] : test fail !\n");
	close(fd);
	free(buf);
	return 0;
}

int main(int argc, FAR char *argv[])
{
	pid_t task_id[5], pid;
	int status;
	char *task_name[5] = {"task1", "task2", "task3", "task4", "task5"};

	srand(time(NULL));

	g_argv[0] = "1";
	task_id[0] = task_create(task_name[0], 100, STACKSIZE, (void *)task_1, g_argv);
	if (task_id[0] == ERROR)
	{
		printf("ERROR: task1 create fail !\n");
		exit(1);
	}
	else
	{
		printf("create task1 success !\n");
		printf("Test task started ...\n");
	}

	while ((pid = wait(&status)) > 0)
		;

	if (test_result == 0)
	{
		printf("TEST PASSED\n");
		exit(0);
	}
	printf("TEST FAILED\n");
	exit(1);
}
