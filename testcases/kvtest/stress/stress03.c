#include <nuttx/config.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "kvdb.h"
#include "kvtest.h"

#define DATA_NUM 10000

/****************************************************************************
 * Name: stress  (Database read and write speed test)
 * Example description:
 	1. Insert 10,000 data.
	2. Calculate the insertion speed.
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *data = NULL;
	clock_t start, finish;
	double duration;
	int ret;
	char key[5] = {0};
	int test_round = DATA_NUM;

	if (argc == 2)
		test_round = atoi(argv[1]);

	srand(time(NULL));
	data = genRandomString(20);
	start = clock();
	printf("start: %f\n", (double)(start));
	for (int i = 0; i < test_round; i++)
	{
		itoa(i, key, 10);
		ret = property_set(key, data);
		if (ret != 0)
		{
			printf("stroe a data(int64) failed  no.%d, exit() , return: %d!\n", i, ret);
			clean_up();
			return -1;
		}
	}
	finish = clock();
	free(data);
	printf("finish: %f\n", (double)(finish));
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("[write test]  data number: %d, takes %f seconds\n", test_round, duration);
	clean_up();
	return 0;
}
