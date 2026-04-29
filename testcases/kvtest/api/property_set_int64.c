#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_set_int64
 * Example description:
 	1. Randomly generate a key.
	2. Insert a int64 type data.
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	// -9223372036854775808 9223372036854775807
	int64_t data1 = INT64_MIN;
	int64_t data2 = -6727346527346527834;
	int64_t data3 = 0;
	int64_t data4 = 8757357537575373757;
	int64_t data5 = 9223372036854775807;
	int ret;

	key = genRandomString(2);
	ret = property_set_int64(key, data1);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data1);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(4);
	ret = property_set_int64(key, data2);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data2);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(5);
	ret = property_set_int64(key, data3);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data3);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(6);
	ret = property_set_int64(key, data4);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data4);
	free(key);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(9);
	ret = property_set_int64(key, data5);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data5);
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