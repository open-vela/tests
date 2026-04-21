#include "keystore/client.h"
#include "keystore_test.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

static int test_flag;

static void show_usage(void)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-k <key_len>] [-d <data_len>] [-l <looptimes>] "
         "[-t <sleep_time>]\n"
         "\t\t-k: set key length\n"
         "\t\t-d: data length\n"
         "\t\t-l: loop times\n"
         "\t\t-t: sleep time\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  char *data = NULL;
  char *key = NULL;
  int key_len = 5;
  int data_len = 5;
  int ret;
  int ch;
  int loop = 1;
  test_flag = 0;
  int wait = 0;
  while ((ch = getopt(argc, argv, "k:d:l:h:t:")) != EOF)
    {
      switch (ch)
        {
        case 'k':
          key_len = atoi(optarg);
          break;
        case 'd':
          data_len = atoi(optarg);
          break;
        case 'l':
          loop = atoi(optarg);
          break;
        case 't':
          wait = atoi(optarg);
          break;
        case 'h':
        default:
          show_usage();
          break;
        }
    }

  srand(time(NULL));
  for (int i = 0; i < loop; i++)
    {
      key = genRandomString(key_len);
      data = genRandomString(data_len);
      syslog(LOG_INFO, "generate key is %s\n", key);
      sleep(wait);
      syslog(LOG_INFO, "ready to insert\n");
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
             i, key, (char *)data);
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
      char fin[data_len + 1];
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
      if (strncmp(fin, data, len))
        {
          syslog(LOG_ERR, "keystoretest, GET data not equal insert");
          test_flag = 1;
          goto end;
        }
      sleep(wait);
      syslog(LOG_INFO, "ready to del key");
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
      free(key);
      key = NULL;
      free(data);
      data = NULL;
    }
end:
  if (key != NULL)
    {
      free(key);
      key = NULL;
    }
  if (data != NULL)
    {
      free(data);
      data = NULL;
    }

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
