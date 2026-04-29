#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: function (Verify that the data is correct)
 * Example description:
 	1. Randomly generate a key.
	2. Insert the data.
	3. Check that the got data is consistent with the inserted data.
 * Expect results: TEST PASSED
 ****************************************************************************/
static int abs_rand(void)
{
    int r = rand();
    return (r < 0) ? (-r) : r;
}

int test_string(int run_num)
{
	int ret;
	char *key = NULL;
	char *data = NULL;
	char value[VALUE_MAX_LEN] = {0};
	clean_up();
	for (int i = 0; i < run_num; i++)
	{
		srand((unsigned)time(NULL) + i);
		key = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
		data = genRandomString(abs_rand() % (VALUE_MAX_LEN - 1) + 1);
		printf("run test %d , key: %s , data: %s\n", (i + 1), key, data);
		ret = property_set(key, data);
		if (ret != 0)
		{
			printf("stroe a data failed, exit() , return: %d!\n", ret);
			return -1;
		}
		ret = property_get(key, value, "");
		if (ret == 0)
		{
			printf("read data from db failed, exit() , return: %d!\n", ret);
			return -1;
		}
		if (strcmp(value, data) != 0)
		{
			printf("The data stored and read is different ! store: %s  , read: %s\n", data, value);
			return -1;
		}
		free(key);
		free(data);
	}
	return 0;
}

int test_int32(int run_num)
{
	char *key = NULL;
	int32_t data;
	int32_t value;
	int ret;
	for (int i = 0; i < run_num; i++)
	{
		srand((unsigned)time(NULL) + i);
		data = abs_rand() % INT32_MAX + 1;
		key = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
		// printf("run test %d , key: %s , data: %d\n", (i + 1), key, data);
		ret = property_set_int32(key, data);
		if (ret != 0)
		{
			printf("stroe a data(int32) failed, exit() , return: %d!\n", ret);
			return -1;
		}
		value = property_get_int32(key, 0);
		if (value != data)
		{
			// printf("The data stored and read is different ! store: %d  , read: %d\n", data, value);
			return -1;
		}
		free(key);
	}
	return 0;
}

int test_int64(int run_num)
{
	char *key = NULL;
	int64_t data;
	int64_t value;
	int ret;
	for (int i = 0; i < run_num; i++)
	{
		srand((unsigned)time(NULL) + i);
		data = abs_rand() % INT64_MAX + 1;
		key = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
		printf("run test %d , key: %s , data: %lld\n", (i + 1), key, data);
		ret = property_set_int64(key, data);
		if (ret != 0)
		{
			printf("stroe a data(int64) failed, exit() , return: %d!\n", ret);
			return -1;
		}
		value = property_get_int64(key, 0);
		if (value != data)
		{
			printf("The data stored and read is different ! store: %lld  , read: %lld\n", data, value);
			return -1;
		}
		free(key);
	}
	return 0;
}

int test_bool(int run_num)
{
	char *key = NULL;
	int data;
	int ret;
	int value;
	for (int i = 0; i < run_num; i++)
	{
		srand((unsigned)time(NULL) + i);
		data = abs_rand() % 2;
		key = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
		printf("run test %d , key: %s , data: %d\n", (i + 1), key, data);
		ret = property_set_bool(key, data);
		if (ret != 0)
		{
			printf("stroe a data(bool) failed, exit() , return: %d!\n", ret);
			return -1;
		}
		value = property_get_bool(key, 0);
		if (value != data)
		{
			printf("The data stored and read is different ! store: %d  , read: %d\n", data, value);
			return -1;
		}
		free(key);
	}
	return 0;
}

int main(int argc, FAR char *argv[])
{
	int ret;
	int test_run = 20;
	printf("test string type------------\n");
	ret = test_string(test_run);
	if (ret == -1)
	{
		printf("test string failed !");
		goto FAIL;
	}
	printf("\ntest int32 type ------------\n");
	ret = test_int32(test_run);
	if (ret == -1)
	{
		printf("test int32 failed !");
		goto FAIL;
	}
	sleep(2);
	printf("\ntest int64 type ------------\n");
	ret = test_int64(test_run);
	if (ret == -1)
	{
		printf("test int64 failed !");
		goto FAIL;
	}
	sleep(2);
	printf("\ntest bool type ------------\n");
	ret = test_bool(test_run);
	if (ret == -1)
	{
		printf("test int64 failed !");
		goto FAIL;
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
FAIL:
	clean_up();
	exit(1);
}