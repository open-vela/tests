/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_app_test02.c
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
#include <stdio.h>
#include <pthread.h>
#include "media_focus_test.h"

static int ret = 1;
static int lock = 1;

static void *thread_fun(void *args)
{
    char *stream = MEDIA_STREAM_MUSIC;
    int return_type;

    if (!media_focus_request(&return_type, stream, &focus_change_listener, (void *)0))
        ret = -1;
    lock = 0;
    return NULL;
}

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2022-01-11

Description:
    1. After application A requests the focus, application B does not give
        up and directly exits the application. Then application B requests
        the focus.

**************************************************************************/
int main(int argc, char const *argv[])
{
    pthread_t thread;
    void *handle = NULL;
    char *stream_type = MEDIA_STREAM_RING;
    media_focus_id focus_list[MEDIA_FOCUS_STACK_NUM];
    media_focus_id *p_focus_list = focus_list;
    int return_type, num;

    media_focus_id expect_result[1] = {
                {0, 1, getpid(), 1, &focus_change_listener}
    };

    /* Thread a request focus */
    pthread_create(&thread, NULL, (void *)thread_fun, NULL);
    while (lock)
        usleep(10);

    media_focus_debug_stack_return(p_focus_list, 1);
    media_focus_debug_stack_display();
    if (ret == -1 || p_focus_list->stream_type != 10 || p_focus_list->focus_state != 1
        || p_focus_list->client_id != 0 || p_focus_list->thread_id != thread)
        goto err;

    /* Thread a exit, thread b request focus */
    handle = media_focus_request(&return_type, stream_type, &focus_change_listener, (void *)0);
    if (!handle)
        goto err;
    media_focus_debug_stack_display();
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    if (check_stack_order(p_focus_list, expect_result, MEDIA_FOCUS_STACK_NUM - num) == -1)
        goto err;

    /* Thread b media_focus_abandon */
    if (handle && media_focus_abandon(handle) < 0)
        goto err;
    pthread_join(thread, NULL);
    media_focus_debug_stack_display();
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    if (ret == -1 || MEDIA_FOCUS_STACK_NUM - num != 0)
        goto err;

    syslog(LOG_INFO, "PASS! media_focus_app_test02 pass.\n");
    return 0;

err:
    if (handle)
        media_focus_abandon(handle);
    pthread_join(thread, NULL);
    syslog(LOG_ERR, "FAIL! media_focus_app_test02 fail.\n");
    return 0;
}