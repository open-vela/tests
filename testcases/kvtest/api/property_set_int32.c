#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_set_int32
 * Example description:
 	1. Randomly generate a key.
	2. Insert a int32 type data.
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	int32_t data1 = -2147483648;
	int32_t data2 = -2135555587;
	int32_t data3 = 0;
	int32_t data4 = 1135555587;
	int32_t data5 = 2147483647;
	int ret;

	key = genRandomString(2);
	ret = property_set_int32(key, data1);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data1);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(4);
	ret = property_set_int32(key, data2);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data2);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(5);
	ret = property_set_int32(key, data3);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data3);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(6);
	ret = property_set_int32(key, data4);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data4);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(9);
	ret = property_set_int32(key, data5);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data5);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}