#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_get_int64
 * Example description:
 	1. Randomly generate a key.
	2. Insert int64 type data. then get it
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	int64_t data1 = INT64_MAX;
	// -9223372036854775808 to 9223372036854775808
	int64_t data2 = INT64_MIN;
	int64_t value;
	int ret;

	key = genRandomString(5);
	ret = property_set_int64(key, data1);
	printf("int64 MAX data: %lld,  int64 MIN data: %lld\n", INT64_MAX, INT64_MIN);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data1);

	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	value = property_get_int64(key, 0);
	free(key);
	printf("get int64 data from db:%lld\n", value);
	if (value != data1)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	key = genRandomString(10);
	ret = property_set_int64(key, data2);
	printf("Store a piece of data , the key: %s    the data: %lld\n", key, data2);

	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}

	value = property_get_int64(key, 0);
	printf("get int64 data from db:%lld\n", value);
	free(key);
	if (value != data2)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}