/****************************************************************************
 * tests/testcases/media_test/media_graph/media_graph_open.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 *The ASF licenses this file to you under the Apache License, Version 2.0
 *(the "License"); you may not use this file except in compliance with
 *the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *implied.  See the License for the specific language governing
 *permissions and limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "media_graph_test.h"
#include <media_api.h>
#include <nuttx/config.h>
#include <stdlib.h>
#include <syslog.h>


int main(int argc, FAR char *argv[])
{
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  mediatest_getopt(argc, argv, media);

  if (media->type == MEDIATEST_PLAYER)
    {
      // music
      FUN_CHECK(mediatest_common_open(media), media,
                mediatest_common_close, 0, "FAIL! media open %s fail.\n",
                media->stream_type);
    }
  else if(media->type == MEDIATEST_RECORDER)
    {
      // cap
      FUN_CHECK(mediatest_common_open(media), media,
                mediatest_common_close, 0, "FAIL! media open %s fail.\n",
                media->stream_type);
    }

  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            0, "FAIL! media open %s fail.\n", media->stream_type);
  syslog(LOG_INFO, "PASS! media open success. \n");

  free(media);
  media = NULL;

  return 0;
}