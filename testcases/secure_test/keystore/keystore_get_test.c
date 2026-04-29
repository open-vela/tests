#include "keystore/client.h"
#include "keystore_test.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

static int test_flag;

int main(int argc, char *argv[])
{
  char key[19] = {0};
  char *data = "hell$*o";
  int ret;
  test_flag = 0;

  snprintf(key, 19, "Key$StoreTest_%d", 2000);
  ret = keyStoreInsert(key, strlen(key), (uint8_t *)data, strlen(data));
  logReturnCode(ret);
  if (ret != 1)
    {
      syslog(LOG_ERR, "keystoretest,store FAILED !  return %d", ret);
      test_flag = 1;
      goto end;
    }
  syslog(LOG_INFO, "Insert a test data, key=[%s] data=[%s]\n", key,
         (char *)data);
  for (int i = 0; i < 20000; i++)
    {
      // usleep(5*1000);
      ret = keyStoreExist(key, strlen(key));
      if (ret != 1)
        {
          syslog(LOG_ERR,
                 "keystoretest,exist FAILED !  return %d ,  steps %d\n",
                 ret, i);
          test_flag = 1;
          goto end;
        }
      uint8_t *item = NULL;
      size_t len = 0;
      char fin[128];
      memset(fin, 0, sizeof(fin));
      ret = keyStoreGet(key, strlen(key), &item, &len);
      logReturnCode(ret);
      if (ret != 1)
        {
          syslog(LOG_ERR,
                 "keystoretest,get FAILED !  return %d ,  steps %d\n",
                 ret, i);
          test_flag = 1;
          goto end;
        }
      syslog(LOG_INFO, "len is %d\n", len);
      strncpy(fin, (char *)item, len);
      syslog(LOG_INFO, "get a test data, step=%d key=[%s] data=[%s]\n",
             i, key, fin);
      if (strncmp(fin, (char *)data, len))
        {
          syslog(LOG_ERR, "keystoretest, GET data not equal insert");
          test_flag = 1;
          goto end;
        }
    }
end:
  if (test_flag == 1)
    {
      printf("TEST FAILED !\n");
    }
  else
    {
      printf("TEST PASSED !\n");
    }
  delete_all();
  exit(test_flag);
}
