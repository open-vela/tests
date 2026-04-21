#include "keystore/client.h"
#include "keystore_test.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

static int test_flag;

void *test_data(void *v)
{
  int num = *((int *)v);
  char *data = NULL;
  test_flag = 0;
  int ret;

  srand(time(NULL));
  for (int i = num; i < num + 10000; i++)
    {
      char key[20];
      snprintf(key, 20, "KeyStoreTest_%d", i);
      data = genRandomString(i % 100 + 100);
      ret = keyStoreInsert(key, strlen(key), (uint8_t *)data,
                           strlen(data));
      logReturnCode(ret);
      if (ret != 1)
        {
          syslog(LOG_ERR,
                 "keystoretest,store FAILED !  return %d ,  steps %d\n",
                 ret, i);
          test_flag = 1;
          goto end;
        }
      syslog(LOG_INFO, "Insert a test data,step %d key=[%s] data=[%s]\n",
             i, key, data);
      uint8_t *item = NULL;
      size_t len = 0;
      char fin[256];
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
      strncpy(fin, (char *)item, len);
      syslog(LOG_INFO, "get a test data, key=[%s] data=[%s]\n", key,
             fin);
      if (strncmp(fin, (char *)data, len))
        {
          syslog(LOG_ERR, "keystoretest, GET data not equal insert");
          test_flag = 1;
          goto end;
        }
      ret = keyStoreDel(key, strlen(key));
      logReturnCode(ret);
      if (ret != 1)
        {
          syslog(LOG_ERR,
                 "keystoretest,delete FAILED !  return %d ,  steps "
                 "%d\n",
                 ret, i);
          test_flag = 1;
          goto end;
        }
      free(data);
      data = NULL;
    }
end:
  if (data != NULL)
    {
      free(data);
      data = NULL;
    }

  delete_all();
  return NULL;
}

int main(int argc, char *argv[])
{
  int res;
  test_flag = 0;

  pthread_t myThread1, myThread2;
  int arg = 0;
  int arg1 = 10000;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 4096);
  res = pthread_create(&myThread1, &attr, test_data, (void *)&arg);
  if (res != 0)
    {
      syslog(LOG_ERR, "pthread create failed\n");
      return 0;
    }
  sleep(2);
  res = pthread_create(&myThread2, &attr, test_data, (void *)&arg1);
  if (res != 0)
    {
      syslog(LOG_ERR, "pthread create failed\n");
      return 0;
    }
  pthread_join(myThread1, NULL);
  pthread_join(myThread2, NULL);
  if (test_flag == 1)
    {
      printf("TEST FAILED !\n");
    }
  else
    {
      printf("TEST PASSED !\n");
    }
  return 0;
}
