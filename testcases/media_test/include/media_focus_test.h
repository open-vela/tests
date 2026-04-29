/****************************************************************************
 * tests/testcases/media_test/include/media_focus_test.h
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

#ifndef __MEDIA_FOCUS_TEST_H
#define __MEDIA_FOCUS_TEST_H
#define MEDIA_FOCUS_TYPE_NUM        11
#define MEDIA_FOCUS_MATRIX_NUM      11
#define MEDIA_FOCUS_STACK_NUM       CONFIG_MEDIA_FOCUS_STACK_DEPTH
#define MAX_BUFF		    1024

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "media_focus.h"
#include "media_api.h"
#include <syslog.h>

struct media_focus_matrix {
    int posi_num;       /* Positive table numbers */
    int rever_num;      /* Reverse table numbers */
};

extern char **media_focus_stream;

/****************************************************************************
 * Function Name: focus_change_listener
 *
 * Description:
 *  A callback function.
 *
 ****************************************************************************/
void focus_change_listener(int play_ret, void *callback_argv);

/****************************************************************************
 * Function Name: check_stack_order
 *
 * Description:
 *  Determines whether the actual stack information is consistent
 *      with the desired stack information result.
 *
 * Input Parameters:
 *   focus_list                  -  actual stack information
 *   expect_list                 -  desired stack information
 *   len                         -  the length of the stack
 ****************************************************************************/
int check_stack_order(media_focus_id *focus_list, media_focus_id *expect_list, int len);

/****************************************************************************
 * Function Name: media_focus_test_play_ret
 *
 * Description:
 *  Conversion of the matrix and the result of the return value
 *      of the callback function.
 *
 * Input Parameters:
 *   inter_ret                  -  callback function return value
 ****************************************************************************/
int media_focus_test_play_ret(int inter_ret);

/****************************************************************************
 * Function Name: read_file_init_matrix
 *
 * Description:
 *  Initialize the matrix instance by reading the configuration file.
 *
 * Input Parameters:
 *   file_path      - config file path
 *   matrix         - matrix instance table
 ****************************************************************************/
int read_file_init_matrix(char *file_path, struct media_focus_matrix **matrix);

/****************************************************************************
 * Function Name: printf_expect_list
 *
 * Description:
 *  Prints the expect stack list.
 *
 * Input Parameters:
 *   expect_list         - the expect stack list
 *   len                 - the len of list
 ****************************************************************************/
int printf_expect_list(media_focus_id *expect_list, int len);

int media_focus_debug_print_matrix(struct media_focus_matrix *matrix);

#endif