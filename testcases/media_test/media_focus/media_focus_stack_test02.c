/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_stack_test02.c
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
#include <pthread.h>
#include "media_focus_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-08

Description:
    1. Multiple events of the same priority are pushed in the correct order on the stack.

**************************************************************************/
int main(int argc, char const *argv[])
{
    const char *type_stream = MEDIA_STREAM_MUSIC;
    media_focus_id focus_list[MEDIA_FOCUS_STACK_NUM];
    media_focus_id *p_focus_list = focus_list;
    void *handles[MEDIA_FOCUS_STACK_NUM];
    int num = CONFIG_MEDIA_FOCUS_STACK_DEPTH, len, return_type;
    int type_priority = 10, thread_id = getpid();

    media_focus_id expect_result[MEDIA_FOCUS_STACK_NUM] = {
                    {7, type_priority, thread_id, 1,&focus_change_listener},
                    {6, type_priority, thread_id, -2, &focus_change_listener},
                    {5, type_priority, thread_id, -2, &focus_change_listener},
                    {4, type_priority, thread_id, -2, &focus_change_listener},
                    {3, type_priority, thread_id, -2, &focus_change_listener},
                    {2, type_priority, thread_id, -2, &focus_change_listener},
                    {1, type_priority, thread_id, -2, &focus_change_listener},
                    {0, type_priority, thread_id, -2, &focus_change_listener}
    };
    len = sizeof(expect_result) / sizeof(media_focus_id);

    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM; i++) {
        handles[i] = media_focus_request(&return_type, type_stream, &focus_change_listener, (void *)i);
        if (!handles[i]) {
            syslog(LOG_ERR, "FAIL! media_focus_request() api fail.\n");
            goto out;
        }
    }
    media_focus_debug_stack_return(p_focus_list, num);
    if (num < 0 || num != len || check_stack_order(p_focus_list, expect_result, num) == -1) {
        syslog(LOG_ERR, "FAIL! The actual result of the matrix is inconsistent"
            " with the expected result.\n");
        goto out;
    }

    syslog(LOG_INFO, "PASS! media_focus_stack_test02 pass.\n");

out:
    for (int i = 0; i < MEDIA_FOCUS_STACK_NUM; i++) {
        if (handles[i] && media_focus_abandon(handles[i]) != 0) {
            syslog(LOG_ERR, "FAIL! media_focus_abandon() api fail.\n");
            return 0;
        }
    }
    return 0;
}