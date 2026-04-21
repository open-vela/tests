/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_stack_test01.c
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

#include <nuttx/config.h>
#include <stdlib.h>
#include <string.h>
#include "media_focus_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-08

Description:
    1. This Test is stack size boundary value test.
    2. The number of push events is less than the stack.
    3. The number of push events equals the stack.
    4. The number of push events is greater than the stack.

**************************************************************************/
int main(int argc, char const *argv[])
{
    const char *type_stream = MEDIA_STREAM_MUSIC;
    void *handles[MEDIA_FOCUS_STACK_NUM + 1];
    int return_type;

    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM; i++) {
        handles[i] = media_focus_request(&return_type, type_stream, &focus_change_listener, (void *)i);
        if (handles[i] == NULL) {
            syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
            goto out;
        }
    }
    /* the number of events pushed is greater than the stack */
    handles[MEDIA_FOCUS_STACK_NUM] = media_focus_request(
                                        &return_type,type_stream, &focus_change_listener, (void *)0);
    if (handles[MEDIA_FOCUS_STACK_NUM] != NULL) {
        syslog(LOG_ERR, "FAIL! the number of events pushed is greater than the stack"
                        "media_focus_request() should fail.\n");
        goto out;
    }

    syslog(LOG_INFO, "PASS! media_focus_stack_test01 pass.\n");

out:
    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM + 1; i++) {
        if (handles[i] && media_focus_abandon(handles[i]) != 0) {
            syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            return 0;
        }
    }
    return 0;
}