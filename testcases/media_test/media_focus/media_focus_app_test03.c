/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_app_test03.c
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
#include <signal.h>
#include <sys/types.h>
#include "media_focus_test.h"

#define MEDIA_FOCUS_TASK_PRIORITY (-1)

static int ret = 1;
static volatile int status = 1;
static int count = 0;
static int lock = 1;

void sig_fun(int args);
static int task_media_focus_app(int argc, char *argv[]);

/* the callback function of thread */
static void focus_callback_thread(int callback_ret, void *callback_args)
{
    if (callback_ret != MEDIA_FOCUS_PLAY)
        ret = -1;
    return;
}

/* the callback function of main function */
static void focus_callback_main(int callback_ret, void *callback_args)
{
    syslog(LOG_INFO, "callback args:%d\n", (int)callback_args);
    if (count == 0 && callback_ret != MEDIA_FOCUS_PLAY_BUT_SILENT)
        ret = -1;
    if (count == 1 && callback_ret != MEDIA_FOCUS_PLAY)
        ret = -1;
    if (count == 0)
        count ++;
    return;
}

/* thread function to request and abandon focus */
static void *thread_fun(void *args)
{
    void *handle;
    char *stream = MEDIA_STREAM_INCALL;
    int return_type;

    handle = media_focus_request(&return_type, stream, focus_callback_thread, (void *)0);
    lock = 0;
    if (!handle)
        ret = -1;
    sleep(10);
    if (handle && media_focus_abandon(handle) < 0)
        ret = -1;
    return NULL;
}

/* signal handle funciton */
void sig_fun(int args)
{
    status = 0;
    return;
}

/* task function to request and abandon focus */
static int task_media_focus_app(int argc, char *argv[])
{
    void *handle;
    int return_type;
    char *stream = MEDIA_STREAM_RING;

    signal(SIGINT, sig_fun);
    handle = media_focus_request(&return_type, stream, &focus_change_listener, (void *)0);
    if (!handle)
        goto err;

    while(status)
        usleep(100);

    if (handle && media_focus_abandon(handle) < 0)
        goto err;
    return 0;
err:
    ret = -1;
    return ret;
}

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2022-01-11

Description:
    1. Create a task then request focus, create a thread then request focus,
        and then request focus in the main function.
    2. The task abandon focus first, then the thread abandon focus,
        then the main function abandon focus.
    3. Tests that the stack order is correct and that the value
        returned by the callback function is as expected.

**************************************************************************/
int main(int argc, char const *argv[])
{
    pid_t pid;
    pthread_t thread;
    char *stream2 = MEDIA_STREAM_SPORT;
    void *handle;
    int return_type, num;
    media_focus_id focus_list[MEDIA_FOCUS_STACK_NUM];
    media_focus_id *p_focus_list = focus_list;

    media_focus_id expect_result[2] = {
                {1, 0, 0, 1, &focus_callback_thread},
                {0, 1, 0, -2, &focus_change_listener}};

    media_focus_id expect_result1 = {1, 0, 0, 1, &focus_callback_thread};

    /* create task and request focus */
    pid = task_create("task_media_focus", MEDIA_FOCUS_TASK_PRIORITY,
            CONFIG_DEFAULT_TASK_STACKSIZE, task_media_focus_app, NULL);
    if (pid < 0)
        goto err;
    expect_result[1].thread_id = pid;

    /* create thread and request focus */
    pthread_create(&thread, NULL, thread_fun, NULL);
    while (lock)
        usleep(10);

    expect_result[0].thread_id = thread;
    expect_result1.thread_id = thread;

    /* main_function request focus */
    handle = media_focus_request(&return_type, stream2, &focus_callback_main, (void *)0);
    if (!handle)
        goto err;
    if (return_type == MEDIA_FOCUS_STOP)
        handle = NULL;

    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    media_focus_debug_stack_display();
    if (check_stack_order(p_focus_list, expect_result, MEDIA_FOCUS_STACK_NUM - num) == -1)
        goto err;

    /* task exit */
    kill(pid, SIGINT);
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    media_focus_debug_stack_display();
    if (check_stack_order(p_focus_list, &expect_result1,
            MEDIA_FOCUS_STACK_NUM - num) == -1)
        goto err;

    /* wait for thread exit */
    pthread_join(thread, NULL);
    num = media_focus_debug_stack_return(p_focus_list, MEDIA_FOCUS_STACK_NUM);
    media_focus_debug_stack_display();
    if (MEDIA_FOCUS_STACK_NUM - num != 0)
        goto err;

    if (handle && (media_focus_abandon(handle) < 0 || ret == -1))
        goto err;

    syslog(LOG_INFO, "PASS! media_focus_app_test03 pass.\n");
    return 0;
err:
    kill(pid, SIGINT);
    pthread_join(thread, NULL);
    syslog(LOG_ERR, "FAIL! media_focus_app_test03 fail.\n");
    return -1;
}