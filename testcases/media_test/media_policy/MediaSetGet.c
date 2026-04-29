/****************************************************************************
 * frameworks/media_volume/media_volume_policy.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 *The ASF licenses this file to you under the Apache License, Version 2.0
 *(the "License"); you may not use this file except in compliance with
 *the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *implied.  See the License for the specific language governing
 *permissions and limitations under the License.
 *
 ****************************************************************************/

#include <MediaPolicyTest.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

int main(int argc, FAR char *argv[])
{
  int len, len1;
  const char *ring_names[] = {"Ring",   "SCO",   "Notify",
                              "Health", "Music", "Sport",
                              "TTS",    "Alarm", "Enforced"};
  const char *ring_values[] = {"zero"};
  const char *volume_names[] = {
      "SCOVolume",   "RingVolume",  "NotifyVolume", "HealthVolume",
      "MusicVolume", "SportVolume", "TTSVolume",    "AlarmVolume"};
  const char *volume_values[] = {"0", "1", "2", "3", "4", "5",
                                 "6", "7", "8", "9", "10"};

  len = sizeof(ring_names) / sizeof(ring_names[0]);
  for (int i = 0; i < len; i++)
    {
      syslog(LOG_INFO, "name:%s value:%s\n", ring_names[i],
             ring_values[0]);
      if (0 != test_criterion(ring_names[i], ring_values, 1, 1))
        goto error_out;
    }

  len = sizeof(volume_names) / sizeof(volume_names[0]);
  len1 = sizeof(volume_values) / sizeof(volume_values[0]);
  for (int i = 0; i < len; i++)
    {
      syslog(LOG_INFO, "name:%s value:%s\n", volume_names[i],
             volume_values[0]);
      if (0 != test_criterion(volume_names[i], volume_values, len1, 0))
        goto error_out;
    }

  syslog(LOG_INFO, "test success\n");
  return 0;

error_out:
  syslog(LOG_INFO, "test fail\n");
  return 1;
}
