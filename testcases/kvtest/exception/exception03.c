#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"
#define DATA_MAX_LEN 1024

/****************************************************************************
 * Name: exception   Limit range test(int32 / int64)
 * Example description:
 	1. Randomly generate a key,
	2. Insert the data. test the boundary value
		* int32 max : 2147483647 *
		* 1000000000 * 
		* int64 max : 9223372036854775807 *
		* 9223372036859999997*
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int ret;
	char *key = NULL;
	int32_t test_int32_1, test_int32_2;
	int64_t test_int64_1, test_int64_2;

	clean_up();
	key = genRandomString(5);
	ret = property_set_int32(key, 2147483647);
	if (ret != 0)
	{
		printf("store data failed !\n");
		goto FAIL;
	}
	test_int32_1 = property_get_int32(key, 0);
	free(key);
	key = genRandomString(6);
	ret = property_set_int64(key, 9223372036854775807);
	if (ret != 0)
	{
		printf("store data failed !\n");
		goto FAIL;
	}
	test_int64_1 = property_get_int64(key, 0);
	key = genRandomString(7);
	ret = property_set_int32(key, -2147483648);
	if (ret != 0)
	{
		printf("store data failed !\n");
		goto FAIL;
	}
	test_int32_2 = property_get_int32(key, 0);
	free(key);
	key = genRandomString(8);
	ret = property_set_int64(key, INT64_MIN);
	if (ret != 0)
	{
		printf("store data failed !\n");
		goto FAIL;
	}
	test_int64_2 = property_get_int64(key, 0);
	if (test_int32_1 == INT32_MAX && test_int64_1 == INT64_MAX && test_int32_2 == INT32_MIN && test_int64_2 == INT64_MIN)
	{
		printf("TEST PASSED !\n");
		clean_up();
		exit(0);
	}
	free(key);
	printf("TEST FAILED !\n");
	clean_up();
	exit(0);

FAIL:
	clean_up();
	exit(1);
}