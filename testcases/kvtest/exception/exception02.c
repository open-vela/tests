#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"
#define DATA_MAX_LEN 1024

/****************************************************************************
 * Name: exception
 * Example description:
 	1. Randomly generate a key, .
	2. Insert the data. data length range 1~1024
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int ret;
	char *key = NULL;
	char *data = NULL;
	char value[DATA_MAX_LEN] = {0};
	int test_flag = 0;
	clean_up();
	key = genRandomString(5);
	srand((unsigned)time(NULL));
	data = genRandomString(PROP_VALUE_MAX - 1);
	ret = property_set(key, data);
	if (ret != 0)
	{
		printf("error: stroe a data failed, exit() , return: %d!\n", ret);
		test_flag = 1;
	}

	ret = property_get(key, value, "aaaa");
	if (ret == 0)
	{
		printf("error: read data from db failed, exit()\n");
		printf("TEST PASSED !\n");
		clean_up();
		exit(0);
	}
	else
	{
		if (strcmp(value, data) != 0)
		{
			printf("error: search data is different!\n");
			test_flag = 1;
		}
	}
	free(data);
	srand((unsigned)time(NULL) + 1);
	data = genRandomString(PROP_VALUE_MAX - 1);
	ret = property_set(key, data);
	if (ret != 0)
	{
		printf("error: stroe a data failed, exit() , return: %d!\n", ret);
		test_flag = 1;
	}

	ret = property_get(key, value, "aaaa");
	if (ret == 0)
	{
		printf("error: read data from db failed, exit()\n");
		printf("TEST PASSED !\n");
		exit(0);
	}
	else
	{
		if (strcmp(value, data) != 0)
		{
			printf("error: search data is different!\n");
			test_flag = 1;
		}
	}
	if (test_flag == 1)
	{
		printf("TEST FAILED !\n");
		clean_up();
		exit(1);
	}
	free(key);
	free(data);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}