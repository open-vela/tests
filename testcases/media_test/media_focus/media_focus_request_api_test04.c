/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_request_api_test04.c
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
#include <errno.h>
#include "media_focus_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-02

Description:
    1. Test call interface media_focus_abandon() multiple times
        after invoking one interface media_focus_request().

**************************************************************************/
int main(int argc, char const *argv[])
{
    const char *type_stream = MEDIA_STREAM_MUSIC;
    int return_type, times = 4;
    void *handle = NULL;

    /* media_focus_request() should fail */
    handle = media_focus_request(&return_type, type_stream, &focus_change_listener, (void *)0);
    if (!handle) {
        syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
        return 0;
    }
    for (int i = 0; i < times; i++) {
        /* first to abandon, should succeed. */
        if (i == 0 && media_focus_abandon(handle) != 0) {
            syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            return 0;
        }

        /* call interface media_focus_abandon() multiple times, should fail */
        if (i != 0 && media_focus_abandon(handle) == 0) {
            syslog(LOG_ERR, "FAIL! call interface media_focus_abandon() multiple times, should fail.\n");
            return 0;
        }
    }
    syslog(LOG_INFO, "PASS! media_focus_request_api_test04 pass.\n");
    return 0;
}