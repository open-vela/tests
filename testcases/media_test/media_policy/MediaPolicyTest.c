#include <media_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/****************************************************************************
 * Test Logic
 ****************************************************************************/
int test_set_int_criterion(const char *name, int value)
{
  int value2 = 0;
  if (media_policy_set_int(name, value, 1))
    {
      syslog(LOG_INFO, "FAIL: %s \n", __func__);
      return -1;
    }

  media_policy_get_int(name, &value2);

  if (value != value2)
    {
      syslog(LOG_INFO, "FAIL: %s value2 %d\n", __func__, value2);
      return -1;
    }

  return 0;
}

int test_set_string_criterion(const char *name, const char *value)
{
  char value2[64];

  if (media_policy_set_string(name, value, 1))
    {
      syslog(LOG_INFO, "FAIL: %s \n", __func__);
      return -1;
    }

  media_policy_get_string(name, value2, sizeof(value2));
  if (strcmp(value, value2))
    {
      syslog(LOG_INFO, "FAIL: %s value2 %s\n", __func__, value2);
      return -1;
    }

  return 0;
}

int test_criterion(const char *criteria_name,
                   const char *criteria_values[], int len, int flag)
{
  int ret;
  int i;

  for (i = 0; i < len; i++)
    {
      if (flag == 0)
        {
          ret = test_set_int_criterion(criteria_name,
                                       atoi(criteria_values[i]));
        }
      else
        {
          ret = test_set_string_criterion(criteria_name,
                                          criteria_values[i]);
        }
      if (0 != ret)
        {
          // syslog(LOG_INFO, "FAIL: %s name: %s, value: %s\n", __func__,
          // criteria_name[0], criteria_values[i]);
          return -1;
        }
    }
  return 0;
}
