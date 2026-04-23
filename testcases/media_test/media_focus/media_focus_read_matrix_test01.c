/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_read_matrix_test01.c
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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "media_focus_test.h"

#define FILE_PATH_LEN 50
#define FILE_NUM 4

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-12-17

Description:
    1. Overwrite configuration file validity check.
    2. In the first case, there are multiple commas in the configuration file.
    3. In the second case, there are multiple colons in the configuration file.
    4. In the third case, the numbers in the configuration file are written
        in letters.
    5. The fourth case, where there is no number after a colon or there is
        no number after a colon, for example 0: or 0, the correct way to
        write it is such as 0:1.

**************************************************************************/
int main(int argc, char const *argv[])
{
    char file_path[FILE_PATH_LEN];
    struct media_focus_matrix *matrix = NULL;
    int ret;

    if (argc < 2) {
        syslog(LOG_ERR, "You need to pass in the filename prefix,"
            " Such as /etc/media.\n");
        goto err;
    }

    for (int i = 0;i < FILE_NUM; i++) {
        sprintf(file_path, "%s/media_focus_test%d.conf", argv[1], i + 1);
        ret = read_file_init_matrix(file_path, &matrix);
        if (ret == -ENOENT || ret >= 0)
            goto err;
    }
    syslog(LOG_INFO, "PASS! media_focus_read_matrix_test01 pass.\n");
    return 0;

err:
    if (matrix)
        free(matrix);
    syslog(LOG_ERR, "FAIL! media_focus_read_matrix_test01 fail.\n");
    return -1;
}