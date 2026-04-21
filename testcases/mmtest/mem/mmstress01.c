#include <nuttx/config.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include "mmtest.h"

/****************************************************************************
 * Name: mmstress01
 * Example description:
 	1. Basic memory stress test
 * Expect results: crash
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
	/* sleep 30s before programer start */
        sleep(30);

	int *address = NULL;
	int memory = 1024 * sizeof(int);

	/* malloc memory */
	address = (int *)malloc(memory);
	if (address == NULL)
	{
		syslog(LOG_ERR, "malloc failed\n");
		exit(1);
	}
	syslog(LOG_ERR, "malloc...%d \n", memory);

	/* write data to address */
	memset(address-2, 'a', 8);

	return 0;
}
