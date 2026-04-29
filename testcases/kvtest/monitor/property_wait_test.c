#include "kvdb.h"
#include "kvtest.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
  if (argc < 3)
    {
      syslog(LOG_ERR, "please input key and time\n");
      return -1;
    }
  char *key = argv[1];
  int timeout = atoi(argv[2]);
  char newkey[PROPERTY_KEY_MAX] = {0};
  char newvalue[PROPERTY_VALUE_MAX] = {0};
  int ret = property_wait(key, newkey, newvalue, PROPERTY_VALUE_MAX, timeout);
  if (ret < 0)
    {
      syslog(LOG_ERR, "property_wait failed, ret=%d\n", ret);
      goto out;
    }

  syslog(LOG_INFO, "the new key: %s\n", newkey);
  syslog(LOG_INFO, "the new value: %s\n", newvalue);

out:
  return ret;
}
