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
 	1. Randomly generate a key.
	2. Insert the data. 
	3. Delete a data from db, it's key does not exist !
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int ret;
	char *key = NULL;
	char *test_data = NULL;
	clean_up();
	for (int i = 1; i < 10; i++)
	{
		key = genRandomString(5 + i);
		test_data = genRandomString(5 + i);
		ret = property_set(key, test_data);
		free(key);
		free(test_data);
		if (ret != 0)
		{
			printf("store data failed !\n");
			goto FAIL;
		}
	}
	key = genRandomString(100);
	ret = property_delete(key);
	printf("delete return %d\n", ret);
	free(key);
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);

FAIL:
	clean_up();
	exit(1);
}