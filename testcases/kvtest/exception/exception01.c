#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"

#define DATA_MAX_LEN 99

/****************************************************************************
 * Name: exception
 * Example description:
 	1. Randomly generate a key, .
	2. Insert the data. Key length range 1~4096
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int test_len = 1, ret;
	char *key = NULL;
	char *data = NULL;
	char value[DATA_MAX_LEN] = {0};
	clean_up();
	for (int i = 1; i <= KEY_MAX_LEN; i++)
	{
		test_len = i;
		srand((unsigned)time(NULL) + i);
		key = genRandomString(test_len);
		data = genRandomString(50);
		ret = property_set(key, data);
		if (ret != 0)
		{
			printf("error: stroe a data failed, exit() , return: %d!  test no.%d\n", ret, i);
			printf("note  key_len %d!\n", test_len);
			goto FAIL;
		}

		ret = property_get(key, value, "");
		if (ret == 0)
		{
			printf("error: read data from db failed, exit() , return: %d!  test no.%d\n", ret, i);
			goto FAIL;
		}
		if (strcmp(value, data) != 0)
		{
			printf("error: search data is different!\n");
			goto FAIL;
		}
		property_delete(key);
		free(key);
		free(data);
	}
	printf("TEST PASSED !\n");
	exit(0);

FAIL:
	clean_up();
	exit(1);
}