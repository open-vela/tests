#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_delete
 * Example description:
 	1. Randomly generate a key and data.
	2. insert the data. then delete it.
	3. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	char *key = NULL;
	char *data = NULL;
	int ret;

	for (int i = 0; i < 10; i++)
	{
		key = genRandomString(i + 20);
		data = genRandomString(i + 80);
		ret = property_set(key, data);
		ret = property_delete(key);
		free(key);
		free(data);
		if (ret != 0)
		{
			printf("TEST FAILED !\n");
			exit(1);
		}
	}

	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}