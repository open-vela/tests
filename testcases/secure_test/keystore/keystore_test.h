#ifndef __KEYSTORE_TEST_H__
#define __KEYSTORE_TEST_H__

#include "keystore/client.h"
#include "kvdb.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#define KEY_MAX_LEN CONFIG_NAME_MAX - 1

/*
    Generate a string of randomly specified length
*/

static size_t unit_8_strlen(uint8_t *str)
{
  size_t count = 0; // Count
  while (*str != '\0')
    {
      count++;
      str++;
    }
  return count;
}

static char *genRandomString(int length)
{
  int flag, i;
  char *string;
  if ((string = (char *)malloc(length + 1)) == NULL)
    {

      return NULL;
    }
  char str[] = {'!', '#', '$',          '%', '&', '(', ')',
                '-', '`', '@',          '^', '_', '{', '}',
                '~', '+', /*',',*/ ';', '=', '[', ']', '\0'};
  int len = strlen(str);
  int core = 0;

  for (i = 0; i < length; i++)
    {
      flag = rand() % 4;
      switch (flag)
        {
        case 0:
          string[i] = 'A' + rand() % 26;
          break;
        case 1:
          string[i] = 'a' + rand() % 26;
          break;
        case 2:
          string[i] = '0' + rand() % 10;
          break;
        case 3:
          core = rand() % len;
          string[i] = str[core];
          break;
        }
    }
  string[length] = '\0';
  return string;
}

static int logReturnCode(int code)
{
  char buffer[128] = {0};
  switch (code)
    {
    case KEYSTORE_NO_ERROR:
      sprintf(buffer, "KEYSTORE_NO_ERROR");
      break;
    case KEYSTORE_LOCKED:
      sprintf(buffer, "KEYSTORE_LOCKED");
      break;
    case KEYSTORE_UNINITIALIZED:
      sprintf(buffer, "KEYSTORE_UNINITIALIZED");
      break;
    case KEYSTORE_SYSTEM_ERROR:
      sprintf(buffer, "KEYSTORE_SYSTEM_ERROR");
      break;
    case KEYSTORE_PROTOCOL_ERROR:
      sprintf(buffer, "KEYSTORE_PROTOCOL_ERROR");
      break;

    case KEYSTORE_PERMISSION_DENIED:
      sprintf(buffer, "KEYSTORE_PERMISSION_DENIED");
      break;

    case KEYSTORE_KEY_NOT_FOUND:
      sprintf(buffer, "KEYSTORE_KEY_NOT_FOUND");
      break;

    case KEYSTORE_VALUE_CORRUPTED:
      sprintf(buffer, "KEYSTORE_VALUE_CORRUPTED");
      break;

    case KEYSTORE_UNDEFINED_ACTION:
      sprintf(buffer, "KEYSTORE_UNDEFINED_ACTION");
      break;
    case KEYSTORE_WRONG_PASSWORD_0:
      sprintf(buffer, "KEYSTORE_WRONG_PASSWORD_0");
      break;
    case KEYSTORE_WRONG_PASSWORD_1:
      sprintf(buffer, "KEYSTORE_WRONG_PASSWORD_1");
      break;

    case KEYSTORE_WRONG_PASSWORD_2:
      sprintf(buffer, "KEYSTORE_WRONG_PASSWORD_2");
      break;
    case KEYSTORE_WRONG_PASSWORD_3:
      sprintf(buffer, "KEYSTORE_WRONG_PASSWORD_3");
      break;
    case KEYSTORE_SIGNATURE_INVALID:
      sprintf(buffer, "KEYSTORE_SIGNATURE_INVALID");
      break;
    case KEYSTORE_OP_AUTH_NEEDED:
      sprintf(buffer, "KEYSTORE_OP_AUTH_NEEDED");
      break;
    case KEYSTORE_KEY_ALREADY_EXISTS:
      sprintf(buffer, "KEYSTORE_KEY_ALREADY_EXISTS");
      break;
    case KEYSTORE_KEY_PERMANENTLY_INVALIDATED:
      sprintf(buffer, "KEYSTORE_KEY_PERMANENTLY_INVALIDATED");
      break;
    }
  syslog(LOG_INFO, "Return=%s(%d)", buffer, code);
  return code;
}

static int delete_all(void)
{
  int ret = keyStoreReset();
  logReturnCode(ret);
  return ret;
}

#endif