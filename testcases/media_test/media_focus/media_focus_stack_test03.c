/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_stack_test03.c
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
#include <string.h>
#include <unistd.h>
#include "media_focus_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-09

Description:
    1. Multiple events of the different priority are pushed in the correct order on the stack.

**************************************************************************/
int main(int argc, char const *argv[])
{
    const char *type_stream[MEDIA_FOCUS_TYPE_NUM] = {MEDIA_STREAM_INCALL, MEDIA_STREAM_RING,
                            MEDIA_STREAM_ALARM, MEDIA_STREAM_SYSTEM_ENFORCED,
                            MEDIA_STREAM_ACCESSIBILITY, MEDIA_STREAM_TTS,MEDIA_STREAM_SPORT,
                            MEDIA_STREAM_NOTIFICATION, MEDIA_STREAM_MUSIC};
    media_focus_id focus_list[MEDIA_FOCUS_STACK_NUM];
    media_focus_id *p_focus_list = focus_list;
    void *handles[MEDIA_FOCUS_STACK_NUM];
    int return_type;
    int num = 0, thread_id = getpid();

    media_focus_id expect_result = {0, 0, thread_id, 1,&focus_change_listener};
    media_focus_id expect_result1 = {0, 10, thread_id, 1,&focus_change_listener};

    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM; i++) {
        handles[i] = media_focus_request(&return_type, type_stream[i], &focus_change_listener, (void *)i);
        if (!handles[i]) {
            syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
            goto out;
        }
        if (return_type == MEDIA_FOCUS_STOP)
            handles[i] = NULL;
    }
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);

    if (num < 0 || MEDIA_FOCUS_STACK_NUM - num != 1 || check_stack_order(p_focus_list,
        &expect_result, MEDIA_FOCUS_STACK_NUM - num) == -1) {
        syslog(LOG_ERR, "FAIL! The actual result of the matrix is inconsistent"
            " with the expected result.\n");
        goto out;
    }

    if (handles[0] && media_focus_abandon(handles[0]) != 0) {
        syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            goto out;
    }
    handles[0] = media_focus_request(&return_type, type_stream[MEDIA_FOCUS_STACK_NUM],
        &focus_change_listener, (void *)0);
    if (!handles[0]) {
        syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
        goto out;
    }

    media_focus_debug_stack_display();
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    if (num < 0 || MEDIA_FOCUS_STACK_NUM - num != 1 || check_stack_order(p_focus_list,
        &expect_result1, MEDIA_FOCUS_STACK_NUM - num) == -1) {
        syslog(LOG_ERR, "FAIL! The actual result of the matrix is inconsistent"
            " with the expected result.\n");
        goto out;
    }

    syslog(LOG_INFO, "PASS! media_focus_stack_test03 pass.\n");

out:
    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM; i++) {
        if (handles[i] && media_focus_abandon(handles[i]) != 0) {
            syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            return 0;
        }
    }
    return 0;
}