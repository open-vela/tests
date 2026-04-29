#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_get_bool
 * Example description:
 	1. Randomly generate a key.
	2. Insert bool type data. then get it
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	int8_t ret;
	key = genRandomString(10);
	property_set_bool(key, 1);
	ret = property_get_bool(key, 0);
	if (ret != 1)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	free(key);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}