#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_set_bool
 * Example description:
 	1. Randomly generate a key.
	2. Insert a bool type data.
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	int ret;

	key = genRandomString(10);
	ret = property_set_bool(key, 1);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	property_delete(key);
	free(key);
	key = genRandomString(20);
	ret = property_set_bool(key, 0);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	property_delete(key);
	free(key);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}