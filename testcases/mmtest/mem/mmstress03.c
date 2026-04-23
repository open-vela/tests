#include <nuttx/config.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include "mmtest.h"

/****************************************************************************
 * Name: mmstress03
 * Example description:
	1. Basic memory stress test
 * Expect results: no exception
 ****************************************************************************/

/* Used to generate a random character */
static char get_a_randchar(void)
{
	int value = rand() % 63;
	if (value == 0)
	{
		return '0';
	}
	else if (value <= 10)
	{
		return value + '0' - 1;
	}
	else if (value <= 36)
	{
		return value + 'a' - 11;
	}
	else
	{
		return value + 'A' - 37;
	}
}

/* Generate a random size in the range min~max */
int get_rand_size(int min, int max)
{
	return rand() % (max - min) + min;
}

static void show_usage(void)
{
	printf("\nUsage: mm_stress_test  <min_size>  <max_size>  <test_num> <delay_time>\n");
	printf("\nWhere:\n");
	printf("  <min_size>    Minimum number of memory requests.\n");
	printf("  <max_size>    Maximum number of memory requests.\n");
	printf("  <test_num>    Number of tests.\n");
	printf("  <delay_time>  Malloc delay time, Unit: millisecond.\n");
}

/****************************************************************************
 * Name: main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	int malloc_size;
	int mallc_min_size; /* The minimum memory length requested in the test */
	int mallc_max_size; /* The maximum memory length requested in the test */
	int test_num;
	int application_delay_time = 1; /* Delay in test */
	char check_character;			/* Memory write content check character */
	char *address_ptr = NULL;

	if (argc < 4)
	{
		syslog(LOG_WARNING, "Missing required arguments\n");
		show_usage();
		exit(1);
	}

	mallc_min_size = atoi(argv[1]);
	mallc_max_size = atoi(argv[2]);
	test_num = atoi(argv[3]);
	application_delay_time = atoi(argv[4]);

	for (int i = 0; i < test_num; i++)
	{
		srand((int)time(0) + i);
		malloc_size = get_rand_size(mallc_min_size, mallc_max_size);
		check_character = get_a_randchar();

		address_ptr = (char *)malloc(malloc_size * sizeof(char));
		if (address_ptr != NULL)
		{
			syslog(LOG_INFO, "TEST NO.%d Malloc succeeded --- address:%p size:%d\n", i, address_ptr, malloc_size);
			memset(address_ptr, check_character, malloc_size);
		}
		else
		{
			syslog(LOG_ERR, "Malloc failed ! The remaining memory may be insufficient\n");
			syslog(LOG_ERR, "Continue to test !!\n");
			continue;
		}

		/* Checking Content Consistency */
		for (int j = 0; j < malloc_size; j++)
		{
			if (address_ptr[j] != check_character)
			{
				syslog(LOG_ERR, "ERROR:Inconsistent content checking\n");
				free(address_ptr);
				return -1;
			}
		}

		/* Free test memory */
		free(address_ptr);
		usleep(application_delay_time);
	}

	return 0;
}
