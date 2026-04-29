#include "kvdb.h"
#include "kvtest.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

/****************************************************************************
 * Name: test02
 * Example description:
        1. Stability test
 * Expect results: TEST PASSED
 ****************************************************************************/
static int test_rept = 0;
static int test_flag = 0;

static int clean(void)
{
  char key[50] = {0};
  for (int i = 0; i < test_rept; i++)
    {
      sprintf(key, "persist.kvStaTest_%d", i);
      property_delete(key);
      usleep(1000);
    }

  return 0;
}

int main(int argc, char *argv[])
{
  char key[50] = {0};
  char *data = NULL;
  int ret;
  int rept = 10;
  if (argc == 2)
    rept = atoi(argv[1]);

  while (rept--)
    {
      srand(time(NULL));
      test_rept = rand() % 1000 + 1000;

      for (int i = 0; i < test_rept; i++)
        {
          sprintf(key, "persist.kvStaTest_%d", i);
          data = genRandomString(i % 10 + 20);
          ret = property_set(key, data);
          if (ret != 0)
            {
              syslog(LOG_ERR, "store FAILED !  return %d ,  steps %d\n", ret, i);
              if (ret != -ENOSPC){
                test_flag = 1;
              } else {
                syslog(LOG_ERR, "No space left on device");
              }
              test_rept = i;
              free(data);
              break;
            }
          syslog(LOG_INFO, "Insert a test data, key is [%s] data=[%s]\n", key, data);
          free(data);
        }
      property_commit();
      clean();
    }

  if (test_flag == 1)
    {
      printf("TEST FAILED !\n");
    }
  else
    {
      printf("TEST PASSED !\n");
    }
  exit(test_flag);
}
