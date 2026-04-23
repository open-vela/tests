#include "kvdb.h"
#include "kvtest.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    char key[50] = {0};
    char value[50] = {0};
    int ret;
    int rept = 1000;
    if (argc == 2)
        rept = atoi(argv[1]);
    for (int i = 1; i <= rept; i++){
        for (int j = 1; j <= 10; j++)
        {
            sprintf(key, "persist.kvpd_%d", j);
            sprintf(value, "kvpd_%d", i);
            ret = property_set(key, value);
            property_commit();
            if (ret == 0)
            {
                syslog(LOG_INFO, "Insert a test data, key=[%s] data=[%s]\n", key, value);
            }
            else {
                syslog(LOG_ERR, "store FAILED ! return %d\n", ret);
            }
        }
    }
    for (int j = 1; j <= 10; j++)
    {
          sprintf(key, "persist.kvpd_%d", j);
          property_delete(key);
          property_commit();
    }
    printf("TEST FINISH !\n");
    return 0;
}