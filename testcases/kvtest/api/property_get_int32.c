#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_get_int32
 * Example description:
 	1. Randomly generate a key.
	2. Insert int32 type data. then get it
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	int32_t data1 = -2102323233;
	int32_t data2 = 1054545457;
	int32_t value;
	int ret;

	key = genRandomString(5);
	ret = property_set_int32(key, data1);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data1);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}

	value = property_get_int32(key, 0);
	// printf("get int32 data from db:%d\n", value);
	if (value != data1)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	free(key);
	key = genRandomString(10);
	ret = property_set_int32(key, data2);
	// printf("Store a piece of data , the key: %s    the data: %d\n", key, data2);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	value = property_get_int32(key, 0);
	free(key);
	// printf("get int32 data from db:%d\n", value);
	if (value != data2)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}