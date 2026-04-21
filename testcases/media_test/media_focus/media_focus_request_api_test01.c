/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_request_api_test01.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdlib.h>
#include "media_focus_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-02

Description:
    1. Test parameter stream_type of interface media_focus_request().
    2. overwrite SCO, RING,NOTIFICATION,ACCESSIBILITY,
            MUSIC, SPORT, TTS,ALARM,SYSTEM_ENFORCED.

**************************************************************************/
int main(int argc, char const *argv[])
{
    const char *type_stream[MEDIA_FOCUS_TYPE_NUM] = {MEDIA_STREAM_INCALL, MEDIA_STREAM_RING,
                            MEDIA_STREAM_ALARM, MEDIA_STREAM_SYSTEM_ENFORCED,
                            MEDIA_STREAM_NOTIFICATION, MEDIA_STREAM_ACCESSIBILITY,
                            MEDIA_STREAM_MUSIC, MEDIA_STREAM_SPORT, MEDIA_STREAM_TTS,
                            MEDIA_STREAM_RECORD, MEDIA_STREAM_INFO};
    int return_type;
    void *handle = NULL;

    for (int i = 0; i < MEDIA_FOCUS_TYPE_NUM; i++) {
        /* request and then abandon */
        handle = media_focus_request(&return_type, type_stream[i], &focus_change_listener, (void *)i);
        if (!handle) {
            syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
            return -1;
        }
        if (handle && media_focus_abandon(handle) != 0) {
            syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            return -1;
        }
    }

    syslog(LOG_INFO, "PASS! media_focus_request_api_test01 pass.\n");
    return 0;
}