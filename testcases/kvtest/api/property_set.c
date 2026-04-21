#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_set
 * Example description:
 	1. Randomly generate a key and data.
	2. Insert the data.
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	char *data = NULL;
	int ret;

	srand(time(NULL));
	key = genRandomString(10);
	data = genRandomString(90);
	ret = property_set(key, data);
	printf("Store a piece of data , the key: %s    the data: %s\n", key, data);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	ret = property_delete(key);
	printf("delete data , the key: %s \n", key);
	free(key);
	free(data);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}