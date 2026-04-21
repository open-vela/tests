#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: function (Repeat the query)
 * Example description:
 	1. Randomly generate ten keys.
	2. Write ten data, and then query one or more of them, looping the query. 
 * Expect results: TEST PASSED
 ****************************************************************************/
static int abs_rand(void)
{
    int r = rand();
    return (r < 0) ? (-r) : r;
}

int main(int argc, FAR char *argv[])
{
	char *key[10] = {NULL};
	char *data[10] = {NULL};
	char value[VALUE_MAX_LEN] = {0};
	int ret, id;
	clean_up();
	printf("size: %d\n", sizeof(char));
	for (int i = 0; i < 10; i++)
	{
		srand((unsigned)time(NULL) + i);
		key[i] = genRandomString(abs_rand() % (KEY_MAX_LEN - 1) + 1);
		data[i] = genRandomString(abs_rand() % (VALUE_MAX_LEN - 1) + 1);
		ret = property_set(key[i], data[i]);
		if (ret != 0)
		{
			printf("error: stroe a data failed, exit() , return: %d!\n", ret);
			goto FAIL;
		}
	}
	id = abs_rand() % 10;
	for (int j = 0; j < 100; j++)
	{
		ret = property_get(key[id], value, "");
		if (ret == 0)
		{
			printf("error: read data from db failed, exit() , return: %d!\n", ret);
			goto FAIL;
		}
		if (strcmp(value, data[id]) != 0)
		{
			printf("error: search data is different!\n");
			goto FAIL;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		free(key[i]);
		free(data[i]);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);

FAIL:
	clean_up();
	exit(1);
}