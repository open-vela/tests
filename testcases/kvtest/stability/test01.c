#include <nuttx/config.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>
#include "kvdb.h"
#include "kvtest.h"

/****************************************************************************
 * Name: test01
 * Example description:
	1. Stability test
 * Expect results: TEST PASSED
 ****************************************************************************/
static int test_flag = 0;
#ifdef CONFIG_KVDB_TEMPORARY_STORAGE
#define TEST_KEY_FMT "kvStaTest_%d"
#define TEST_KEY_FMTLEN (10 + 4 + 1)
#else
#define TEST_KEY_FMT "persist.%d"
#define TEST_KEY_FMTLEN (8 + 4 + 1)
#endif

static int clean(int round)
{
	char key[20] = {0};
	for (int i = 0; i < 1000; i++)
	{
		snprintf(key, TEST_KEY_FMTLEN, TEST_KEY_FMT, i);
		property_delete(key);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char key[20] = {0};
	char *data = NULL;
	int rept = 10;
	int count = 0;
	int round;
	int ret;
	int i;

	if (argc == 2)
	{
		rept = atoi(argv[1]);
	}

	for (round = 1; round <= rept; round++)
	{
		syslog(LOG_INFO, "vela_kvdb_stability_test01 Round %d\n", round);
		srand(time(NULL));
		for (i = 0; i < 1000; i++)
		{
			snprintf(key, TEST_KEY_FMTLEN, TEST_KEY_FMT, i);
			data = genRandomString(i % 10 + 20);
			ret = property_set(key, data);
			if (ret != 0)
			{
				syslog(LOG_ERR, "store FAILED !  return %d ,  steps %d\n", ret, i);
				count++;
				if (count > 10)
				{
					free(data);
					goto out;
				}

			}
			else
			{
				syslog(LOG_INFO, "Insert a test data, key=[%s], data=[%s]\n", key, data);
			}

			free(data);
		}
		clean(round);
		sleep(3);
	}

out:
	if (count == 0)
	{
		printf("TEST PASSED !\n");
	}
	else
	{
		test_flag = 1;
		printf("TEST FAEILD !\n");
		printf("TEST %d TIMES AND FAILED %d!\n", (round - 1) * 1000 + i, count);
	}

	exit(test_flag);
}
