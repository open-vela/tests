#include <nuttx/config.h>
#include <stdio.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: property_list
 * Example description:
 	1. Randomly generate a key and data.
	2. Insert the data.
	3. repeat 1-2 10 times.
	4. Check the returned results
 * Expect results: TEST PASSED
 ****************************************************************************/

static void callback(const char *name, const char *value, void *cookie)
{
	printf("%s: %s\n", name, value);
}

int main(int argc, char *argv[])
{
	char *key = NULL;
	char *data = NULL;
	int ret;

	srand(time(NULL));
	for (int i = 0; i < 10; i++)
	{
		key = genRandomString(i + 20);
		data = genRandomString(i + 80);
		ret = property_set(key, data);
		free(key);
		free(data);
	}
	ret = property_list(callback, NULL);
	if (ret != 0)
	{
		printf("TEST FAILED !\n");
		exit(1);
	}
	printf("TEST PASSED !\n");
	clean_up();
	exit(0);
}
