#include "kvdb.h"
#include "kvtest.h"
#include <nuttx/config.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
  if (argc < 3)
    {
      syslog(LOG_ERR, "please input testkey1 and testkey2");
    }
  int nums = atoi(argv[1]);
  char *testkey[nums];
  bool monitored[nums];
  int flag = 0;
  for (int i = 0; i < nums; ++i)
    {
      int g = i + 2;
      testkey[i] = argv[g];
    }
  struct pollfd fds[nums];
  char newkey[PROPERTY_KEY_MAX];
  char newvalue[PROPERTY_VALUE_MAX];

  for (int i = 0; i < nums; ++i)
    {
      int fd1 = property_monitor_open(testkey[i]);
      fds[i].fd = fd1;
      fds[i].events = POLLIN;
      monitored[i] = false;
    }

  while (1)
    {
      flag = 0;
      int ret = poll(fds, nums, -1);
      if (ret <= 0)
        goto out;

      for (int i = 0; i < nums; i++)
        {
          if ((fds[i].revents & POLLIN) == 0)
            continue;

          monitored[i] = true;
          memset(newkey, 0, PROPERTY_KEY_MAX);
          memset(newvalue, 0, PROPERTY_VALUE_MAX);

          ret = property_monitor_read(fds[i].fd, newkey, newvalue, PROPERTY_VALUE_MAX);
          if (ret < 0)
            goto out;

          syslog(LOG_INFO, "the new key: %s\n", newkey);
          syslog(LOG_INFO, "the new value: %s\n", newvalue);
        }

      for (int i = 0; i < nums; ++i)
        {
          if (!monitored[i])
            flag = 1;
        }
      if (flag == 0)
        break;
    }

out:
  for (int i = 0; i < nums; ++i)
    property_monitor_close(fds[i].fd);

  return 0;
}
