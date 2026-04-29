/****************************************************************************
 * tests/testcases/media_test/media_focus/media_focus_nomemory_test01.c
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
#include <malloc.h>
#include <stdlib.h>
#include <syslog.h>
#include "media_focus_test.h"

#define MEDIA_FOCUS_NEED_MEMORY 968

static unsigned long get_memsize(void);

/* Gets the size of the maximum memory block */
static unsigned long get_memsize(void)
{
    unsigned long memsize;
    struct mallinfo alloc_info;
    unsigned long mem_largest;

    alloc_info = mallinfo();
    mem_largest = alloc_info.mxordblk;
    memsize = mem_largest * 0.9;
    return memsize;
}

/* A linkedlist that stores malloc memory */
struct focus_test_handle_list {
    void *handle;
    struct focus_test_handle_list *next;
};

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2022-01-13

Description:
    1. Test out of memory when initializing focus.
    2. This test case is supported only on boards.

**************************************************************************/
int main(int argc, char const *argv[])
{
    unsigned long memsize = 0;
    struct focus_test_handle_list *handle_list = NULL;
    struct focus_test_handle_list *t;
    struct focus_test_handle_list *handle;
    void *focus_handle;
    char *type = MEDIA_STREAM_RING;
    int return_type;

    t = handle_list;
    while ((memsize = get_memsize()) >= MEDIA_FOCUS_NEED_MEMORY) {
        handle = (struct focus_test_handle_list *)malloc(sizeof(struct focus_test_handle_list));
        handle->handle = (void *)malloc(memsize);
        if (!handle->handle) {
            syslog(LOG_ERR, "FAIL! malloc fail.\n");
            goto out;
        }
        handle->next = NULL;
        /* is head node */
        if (!handle_list) {
            handle_list = handle;
            t = handle_list;
            continue;
        }
        t->next = handle;
        t = t->next;
    }

    /* init focus should fail */
    focus_handle = media_focus_request(&return_type, type, &focus_change_listener, (void *)0);
    if (focus_handle) {
        syslog(LOG_ERR, "FAIL! media focus should init fail.\n");
        goto out;
    }

    syslog(LOG_INFO, "PASS! media_focus_nomemory_test01 pass.\n");

out:
    t = handle_list;
    while (t->next != NULL) {
        handle_list = t->next;
        free(t->handle);
        free(t);
        t = handle_list;
    }
    if (t) {
        free(t->handle);
        free(t);
    }
    return 0;
}
