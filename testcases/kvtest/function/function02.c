#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: function (Override the data)
 * Example description:
 	1. Randomly generate a key.
	2. Insert the data.
	3. Insert the data again to override the last data.
 * Expect results: TEST PASSED
 ****************************************************************************/
static int abs_rand(void)
{
    int r = rand();
    return (r < 0) ? (-r) : r;
}

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	char *old_data_string = NULL;
	char *new_data_string = NULL;
	char value[VALUE_MAX_LEN] = {0};
	int32_t data_32, value_32;
	int64_t data_64, value_64;
	bool data_bool = false;
	bool value_bool;
	int ret;
	clean_up();
	srand((unsigned)time(NULL) + 1);
	key = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
	old_data_string = genRandomString(abs_rand() % (VALUE_MAX_LEN - 1) + 1);
	ret = property_set(key, old_data_string);
	if (ret != 0)
	{
		printf("error: stroe a data failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	srand((unsigned)time(NULL) + 2);
	new_data_string = genRandomString(abs_rand() % (VALUE_MAX_LEN - 1) + 1);
	ret = property_set(key, new_data_string);
	if (ret != 0)
	{
		printf("error: stroe a data failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	ret = property_get(key, value, "");
	if (ret == 0)
	{
		printf("error: read data from db failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	else
	{
		if (strcmp(value, new_data_string) != 0)
		{
			printf("error: test string, The data is not overwritten !\n");
			goto FAIL;
		}
	}

	srand((unsigned)time(NULL) + 3);
	data_32 = abs_rand() % INT32_MAX + 1;
	ret = property_set_int32(key, data_32);
	if (ret != 0)
	{
		printf("stroe a data(int32) failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	value_32 = property_get_int32(key, 0);
	if (value_32 != data_32)
	{
		printf("error: test int32, The data is not overwritten !\n");
		goto FAIL;
	}

	srand((unsigned)time(NULL) + 4);
	data_64 = abs_rand() % INT64_MAX + 1;
	ret = property_set_int64(key, data_64);
	if (ret != 0)
	{
		printf("stroe a data(int32) failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	value_64 = property_get_int64(key, 0);
	if (value_64 != data_64)
	{
		printf("error: test int64, The data is not overwritten !\n");
		goto FAIL;
	}

	srand((unsigned)time(NULL) + 5);
	data_bool = abs_rand() % 2;
	ret = property_set_bool(key, data_bool);
	if (ret != 0)
	{
		printf("stroe a data(bool) failed, exit() , return: %d!\n", ret);
		goto FAIL;
	}
	value_bool = property_get_bool(key, 0);
	if (value_bool != data_bool)
	{
		printf("error: test bool, The data is not overwritten !\n");
		goto FAIL;
	}
	free(key);
	free(new_data_string);
	free(old_data_string);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);

FAIL:
	clean_up();
	exit(1);
}