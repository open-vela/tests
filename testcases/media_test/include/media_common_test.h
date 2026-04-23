
#ifndef __AUDIO_COMMON_TEST_H
#define __AUDIO_COMMON_TEST_H

#include <syslog.h>

#define MEDIATEST_DEBUG(tag, format, ...)                               \
  syslog(LOG_DEBUG, "[%s] %s: " format, #tag, __FUNCTION__,             \
         ##__VA_ARGS__)

#define MEDIATEST_INFO(tag, format, ...)                                \
  syslog(LOG_INFO, "[%s] %s: " format, #tag, __FUNCTION__, ##__VA_ARGS__)

#define MEDIATEST_WARNING(tag, format, ...)                             \
  syslog(LOG_WARNING, "[%s] %s: " format, #tag, __FUNCTION__,           \
         ##__VA_ARGS__)

#define MEDIATEST_ERR(tag, format, ...)                                 \
  syslog(LOG_ERR, "[%s] %s: " format, #tag, __FUNCTION__, ##__VA_ARGS__)

#endif