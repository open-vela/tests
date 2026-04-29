/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_test.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "media_focus_test.h"

#define MEDIA_FOCUS_TEST_FILE_READ_JUMP          0
#define MEDIA_FOCUS_TEST_FILE_READ_STREAM_TYPE   1
#define MEDIA_FOCUS_TEST_FILE_READ_STREAM_NUM    2
#define STREAM_TYPE_LEN         32

int media_callback_ret = -1;

void focus_change_listener(int play_ret, void *callback_argv)
{
    media_callback_ret = play_ret;
    return;
}

int printf_expect_list(media_focus_id *expect_list, int len)
{
    for(int i = 0; i < len; i++)
        syslog(LOG_INFO, "%d %d %d %d %p\n", (expect_list + i)->client_id,
                    (expect_list + i)->stream_type, (expect_list + i)->thread_id,
                    (expect_list + i)->focus_state, (expect_list + i)->callback_method);
    return 0;
}

int check_stack_order(media_focus_id *focus_list, media_focus_id *expect_list, int len)
{
    /* printf, test debug */
    // printf_expect_list(expect_list, len);

    for (int i = 0; i < len; i++)
        if ((focus_list + i)->client_id != (expect_list + i)->client_id ||
            (focus_list + i)->stream_type != (expect_list + i)->stream_type ||
            (focus_list + i)->thread_id != (expect_list + i)->thread_id ||
            (focus_list + i)->focus_state != (expect_list + i)->focus_state ||
            (focus_list + i)->callback_method != (expect_list + i)->callback_method) {
                return -1;
        }
    return 0;
}

int media_focus_test_play_ret(int inter_ret)
{
    int play_ret;
    switch (inter_ret) {
    case 0:
        play_ret = MEDIA_FOCUS_STOP;
        break;
    case 1:
    case 2:
        play_ret = MEDIA_FOCUS_PLAY;
        break;
    case 3:
        play_ret = MEDIA_FOCUS_PLAY_BUT_SILENT;
        break;
    case 4:
        play_ret = MEDIA_FOCUS_PLAY_WITH_DUCK;
        break;
    default:
        play_ret = MEDIA_FOCUS_STOP;
        break;
    }
    return play_ret;
}

static char* media_focus_deblank(char *str)
{
    char *out = str, *put = str;
    for (; *str != '\0'; ++str) {
        if (*str != ' ' && *str != '\n') {
            *put++ = *str;
        }
    }
    *put = '\0';
    return out;
}

static char* media_focus_interate_through_comma(char* sl)
{
    char needle = ',';
    return strchr(sl, needle);
}

static int media_focus_str_to_num(char* str)
{
    if (strlen(str) > 0 && (strspn(str, "0123456789") == strlen(str))) {
        int num = atoi(str);
        return num;
    }
    return -EINVAL;
}

static int media_focus_valid_line_check(char* line)
{
    char pre = '\0';
    char cur;
    for (int i = 0; i < strlen(line); i++) {
        cur = *(line + i);
        if (isalnum(cur) == 0 && cur != ',' && cur != ':') {
            // only letter, num and comma in string line
            goto close;
        }
        if (cur == ',' && pre == ',') {
            // no continuous commas
            goto close;
        }
        if (cur == ':' && pre == ':') {
            // no continuous colons
            goto close;
        }
        if (i == strlen(line) - 1) {
            // last should not have comma and colon
            if (cur == ',' || cur == ':') {
                goto close;
            }
        }
        pre = cur;
    }
    return 0;

close:
    return -EINVAL;
}

static int media_focus_divided_by_colon(char* ptr_line, int* pro, int* pas)
{
    char needle = ':';
    char* index_ptr = NULL;

    index_ptr = strchr(ptr_line, needle);
    if (index_ptr == NULL) {
        syslog(LOG_ERR, "invalid conf file\n");
        return -EINVAL;
    }
    *index_ptr = '\0';
    *pro = media_focus_str_to_num(ptr_line);
    if (*pro < 0) {
        return -EINVAL;
    }
    *pas = media_focus_str_to_num(index_ptr + 1);
    if (*pas < 0) {
        return -EINVAL;
    }
    *index_ptr = ':';
    return 0;
}

static int media_focus_line_identity(char *line, int *shift_index)
{
    char *comma_index;
    if (*line == '#') {
        *shift_index = 0;
        return MEDIA_FOCUS_TEST_FILE_READ_JUMP;
    } else if (*line != '\0') {
        if (media_focus_valid_line_check(line) < 0) {
            return -EINVAL;
        }

        // get first comma str from input line
        comma_index = media_focus_interate_through_comma(line);
        if (comma_index == NULL) {
            return -EINVAL;
        }

        //shift index position is position after first comma
        *shift_index = comma_index - line + 1;
        if (strncmp(line, "Stream", (*shift_index - 1)) == 0) {
            return MEDIA_FOCUS_TEST_FILE_READ_STREAM_TYPE;
        } else {
            return MEDIA_FOCUS_TEST_FILE_READ_STREAM_NUM;
        }
    } else {
        return -EINVAL;
    }
}

static struct media_focus_matrix *media_focus_matrix_init(int len, char *line, int *index, struct media_focus_matrix *matrix)
{
    int count = len;
    char* ptr = strtok(line, ",");
    if (len == 0)
        return NULL;
    if (matrix == NULL) {
        matrix = (struct media_focus_matrix *) malloc((len * len) * sizeof(struct media_focus_matrix));
        if (matrix == NULL) {
            syslog(LOG_ERR, "no mem for media focus matrix\n");
            goto err;
        }
    }

    while (ptr != NULL) {
        if (count >= 0) {
            int pro_num, pas_num;
            if (media_focus_divided_by_colon(ptr, &pro_num, &pas_num) < 0)
                goto err;

            (matrix + *index)->posi_num = pro_num;
            (matrix + *index)->rever_num = pas_num;

            // counts minus and index plus after one media_focus_cell added in matrix
            *index += 1;
            count -= 1;
        } else {
            break;
        }
        ptr = strtok(NULL, ",");
    }
    if (count != 0 && ptr != NULL)
        goto err;

    return matrix;
err:
    if (matrix)
        free(matrix);
    matrix = NULL;
    return matrix;
}

static int media_focus_stream_type_counts(char* sl)
{
    int count = 0;
    char* front_str = sl;

    if (sl == NULL) {
        return count;
    }
    while (1) {
        (*front_str == ',') ? count++ : count;
        (*front_str == '\0') ? count++ : count;
        if (*front_str == '\0') {
            break;
        }
        sl = front_str + 1;
        front_str++;
    }
    return count;
}

static int media_focus_streams_init(int col, char* line, int *num)
{
    int s_count = 0;

    if (media_focus_stream_type_counts(line) == 0) {
        syslog(LOG_ERR, "invalid input in matrix stream\n");
        return -1;
    }
    (*num) = media_focus_stream_type_counts(line);

    char* ptr = strtok(line, ",");
    // filler streams content with line read
    while (ptr != NULL) {
        int ptr_len = strlen(ptr);
        if (s_count < (*num) && ptr_len < col) {
            s_count += 1;
        } else {
            return -1;
        }
        ptr = strtok(NULL, ",");
    }
    if (s_count != (*num)) {
        return -1;
    }
    return 0;
}

int read_file_init_matrix(char *file_path, struct media_focus_matrix **matrix)
{
    FILE* fp;
    char* buf = NULL;
    int ret = 0, index = 0, num = 0, shift_index = 0;
    (*matrix) = NULL;

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "no such interaction matrix file\n");
        return -ENOENT;
    }
    buf = (char *) malloc(MAX_BUFF * sizeof(char));
    if (buf == NULL) {
        ret = -ENOMEM;
        goto out;
    }
    buf[MAX_BUFF - 1] = '\0';

    while (fgets(buf, MAX_BUFF - 1, fp) != NULL) {
        char* line = media_focus_deblank(buf);
        ret = media_focus_line_identity(line, &shift_index);
        line += shift_index;
        switch (ret) {
        case MEDIA_FOCUS_TEST_FILE_READ_STREAM_TYPE:
            /* malloc space for stream types array based on number of stream type and fill with line read */
            ret = media_focus_streams_init(STREAM_TYPE_LEN, line, &num);
            if (ret != 0) {
                syslog(LOG_ERR, "no mem for media focus streams\n");
                ret = -ENOMEM;
                goto out;
            }
            break;

        case MEDIA_FOCUS_TEST_FILE_READ_STREAM_NUM:
            *matrix = media_focus_matrix_init(num, line, &index, *matrix);
            if (!(*matrix)) {
                ret = -ENOMEM;
                goto out;
            }
            break;
        case MEDIA_FOCUS_TEST_FILE_READ_JUMP:
            break;
        default:
            ret = -EINVAL;
            goto out;
        }
    }

out:
    fclose(fp);
    free(buf);
    if (ret < 0 && (*matrix) != NULL) {
        free((*matrix));
        (*matrix) = NULL;
        return ret;
    }
    return ret;
}

int media_focus_debug_print_matrix(struct media_focus_matrix *matrix)
{
    /* Print the front matrix table */
    for (int i = 0; i < MEDIA_FOCUS_MATRIX_NUM; i++) {
        for (int j = 0; j < MEDIA_FOCUS_MATRIX_NUM; j++)
            printf("%d ",matrix[i * MEDIA_FOCUS_MATRIX_NUM + j].posi_num);
        printf("\n");
    }

    printf("\n");

    /* Print the reverse matrix table */
    for (int i = 0; i < MEDIA_FOCUS_MATRIX_NUM; i++) {
        for (int j = 0; j < MEDIA_FOCUS_MATRIX_NUM; j++)
            printf("%d ",matrix[i * MEDIA_FOCUS_MATRIX_NUM + j].rever_num);
        printf("\n");
    }
    return 0;
}

