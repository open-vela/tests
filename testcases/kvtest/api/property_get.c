#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_get
 * Example description:
 	1. Randomly generate a key and data.
	2. Insert the data. then get it
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char* key = NULL;
	char* data = NULL;
	char* value = NULL;
	int ret;

	srand(time(NULL));
	key = genRandomString(10);
	data = genRandomString(90);
	value = genRandomString(90);
	ret = property_set(key, data);
	printf("Store a piece of data , the key: %s    the data:%s\n", key, data);

	free(data);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}

	printf("get data from db\n");
	ret = property_get(key, value, "");
	printf("get data , the key: %s   the data:%s\n", key, value);

	if (ret != strlen(value))
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	property_delete(key);
	printf("TEST PASSED !\n");
	free(key);
	free(value);
	clean_up();
	exit(0);
}