#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"
#define DATA_MAX_LEN 1024

/****************************************************************************
 * Name: exception
 * Example description:
 	1. Randomly generate a key, .
	2. Insert the data. data is empty
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int ret;
	char *key = NULL;
	char *test_data = NULL;
	clean_up();
	key = genRandomString(50);
	ret = property_set(key, "asdadsasdasd");
	if (ret != 0)
	{
		printf("error: store failed ! ret:%d\n", ret);
		goto FAIL;
	}
	ret = property_set(key, test_data);
	if (ret != 0)
	{
		printf("error: store a data ,it's data is NULL ! ret:%d\n", ret);
		printf("TEST FAILED !\n");
		goto FAIL;
	}
	free(key);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);

FAIL:
	clean_up();
	exit(1);
}