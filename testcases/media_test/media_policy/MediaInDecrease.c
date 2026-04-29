/****************************************************************************
 * tests/testcases/media_policy
 *
 ****************************************************************************/

#include <media_api.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#define LOOPTIME 10
#define INDECREASE_VALUE 10

static int test_InDecrease(const char *name)
{
  int i;
  int value2 = 0;

  // loop LOOPTIME times
  for (i = 0; i < LOOPTIME; i++)
    {
      // increase loop INDECREASE_VALUE times
      for (int j = 0; j < INDECREASE_VALUE; j++)
        media_policy_increase(name, 1);
      media_policy_get_int(name, &value2);
      if (value2 < INDECREASE_VALUE)
        {
          syslog(LOG_INFO, "FAIL: %s couter %d times return %d\n", name,
                 INDECREASE_VALUE, value2);
          return -1;
        }

      // decrease loop INDECREASE_VALUE times
      for (int j = 0; j < INDECREASE_VALUE; j++)
        media_policy_decrease(name, 1);
      media_policy_get_int(name, &value2);
      if (value2 != 0)
        {
          syslog(LOG_INFO,
                 "FAIL: %s decrease couter %d times return %d\n", name,
                 INDECREASE_VALUE, value2);
          return -1;
        }
    }

  return 0;
}

int main(int argc, FAR char *argv[])
{
  int len;
  int i;
  const char *list[] = {"RingVolume", "NotifyVolume", "MusicVolume",
                        "TTSVolume", "AlarmVolume"};

  len = sizeof(list) / sizeof(list[0]);
  for (i = 0; i < len; i++)
    {
      if (0 != test_InDecrease(list[i]))
        goto error_out;
    }

  syslog(LOG_INFO, "test success\n");
  return 0;

error_out:
  syslog(LOG_INFO, "test fail\n");
  return 1;
}
