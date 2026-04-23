/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_event_test01.c
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
#include "media_focus_test.h"

extern int media_callback_ret;

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-09

Description:
    1. Overrides the sound event type interaction matrix.

**************************************************************************/
int main(int argc, char const *argv[])
{
    int return_type;
    int rever_num;
    struct media_focus_matrix *matrix;
    void *handlers_x[MEDIA_FOCUS_MATRIX_NUM];
    void *handlers_y[MEDIA_FOCUS_MATRIX_NUM];
    char *path = CONFIG_MEDIA_FOCUS_CONFIG_PATH"/media_focus.conf";
    const char *stream_type[MEDIA_FOCUS_TYPE_NUM] = {
                    MEDIA_STREAM_INCALL, MEDIA_STREAM_RING, MEDIA_STREAM_ALARM,
                    MEDIA_STREAM_SYSTEM_ENFORCED, MEDIA_STREAM_NOTIFICATION,
                    MEDIA_STREAM_RECORD, MEDIA_STREAM_TTS, 
                    MEDIA_STREAM_ACCESSIBILITY, MEDIA_STREAM_SPORT,
                    MEDIA_STREAM_INFO, MEDIA_STREAM_MUSIC};

    if(read_file_init_matrix(path, &matrix) < 0)
        goto err;
    media_focus_debug_print_matrix(matrix);

    for (int i = 0; i < MEDIA_FOCUS_MATRIX_NUM; i++) {
        handlers_x[i] = media_focus_request(&return_type, stream_type[i],
                &focus_change_listener, (void *)i);

        if (!handlers_x[i])
            goto err;
        for (int j = 0;j < MEDIA_FOCUS_MATRIX_NUM; j++) {
            handlers_y[j] = media_focus_request(&return_type, stream_type[j],
                    &focus_change_listener, (void *)i);
            if (return_type == MEDIA_FOCUS_STOP)    continue;
            usleep(1000);

            /* Judge the positive matrix table */
            if (!handlers_y[j] || return_type !=
                matrix[j * MEDIA_FOCUS_MATRIX_NUM + i].posi_num)
                goto err;

            /* Judge the reverse matrix table */
            rever_num = matrix[j * MEDIA_FOCUS_MATRIX_NUM + i].rever_num;
            if (rever_num != 6 && media_callback_ret != 0 && rever_num != media_callback_ret)
                goto err;

            if (handlers_y[j] && media_focus_abandon(handlers_y[j]) != 0)
                goto err;
        }
        if (handlers_x[i] && media_focus_abandon(handlers_x[i]) != 0)
            goto err;
    }
    syslog(LOG_INFO, "PASS! media_focus_event_test01 pass.\n");
    return 0;

err:
    for (int i = 0; i < MEDIA_FOCUS_MATRIX_NUM; i++) {
        if (handlers_x[i])
            media_focus_abandon(handlers_x[i]);
        if (handlers_y[i])
            media_focus_abandon(handlers_y[i]);
    }
    syslog(LOG_ERR, "FAIL! media_focus_event_test01 fail.\n");
    return 0;
}