/****************************************************************************
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
#include "audio_player.h"
#include <errno.h>
#include <fcntl.h>
#include <media_api.h>
#include <mqueue.h>
#include <nuttx/config.h>
#include <nuttx/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/****************************************************************************
 * main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int ret __attribute__((unused)) = 0;
  test_msg_player_t msg;
  int player_type = -1;
  int mqid = -1;
  test_player_info_t *test_player_info = NULL;
  void *af_top_handle = NULL;

  test_audio_focus_init();

  syslog(LOG_INFO, "mediatest player start successfully ....\n");

  /* Create player MessageQueue */
  struct mq_attr attr;
  attr.mq_flags = 0; // QueueProperty: Blocking
  attr.mq_maxmsg = PLAYER_MQ_MSG_LEN;
  attr.mq_msgsize = sizeof(test_msg_player_t);
  attr.mq_curmsgs = 0;
  create_mq(PLAYER_MQ_PAHT, &attr);

  /* OpenReceiveMessageQueue */
  mqid = mq_open(PLAYER_MQ_PAHT, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "Error %d (%s) on mq_open.\n", errno,
             strerror(errno));
    }
  usleep(200 * 1000);
  int running = 1;

  // #endif
  /* Receive command */
  while (1)
    {
      if (mq_receive(mqid, (void *)&msg, sizeof(test_msg_player_t),
                     NULL) == -1)
        {
          syslog(LOG_ERR, "msgrcv failed width errno: %d\n", errno);
        }
      syslog(LOG_DEBUG,
             "******mq received msg!msg.type = %d, msg.cmd = %d\n",
             msg.type, msg.cmd);
      player_type = msg.type;
      /* determine command belongs to */
      test_player_type_t test_playing_type = player_type;
      test_set_info(&test_player_info, player_type);
      switch (msg.cmd)
        {
        case PLAYER_PLAY:
          test_player_info->type = msg.type;
          memset(test_player_info->url, '\0',
                 sizeof(test_player_info->url));
          if (msg.p1)
            {
              strlcpy(test_player_info->url, (char *)msg.p1,
                      sizeof(test_player_info->url));
              free(msg.p1);
              msg.p1 = NULL;
              /* play music */
              ret = player_prepare(test_player_info);
            }
          break;

        case PLAYER_PAUSE:
          test_player_info->state = MEDIA_EVENT_PAUSING;
          if (player_type == PLAYER_RECORD)
            {
              media_recorder_pause(test_player_info->handle);
            }
          else
            {
              media_player_pause(test_player_info->handle);
            }
          break;

        case PLAYER_RESUME:
          test_player_info->state = MEDIA_EVENT_STARTING;
          if (player_type == PLAYER_RECORD)
            {
              media_recorder_start(test_player_info->handle);
            }
          else
            {
              media_player_start(test_player_info->handle);
            }
          break;

        case PLAYER_STOP:
          test_player_info->state = MEDIA_EVENT_STOPPING;
          focus_abandon(test_player_info);
          if (player_type == PLAYER_RECORD)
            {
              media_recorder_stop(test_player_info->handle);
              media_recorder_close(test_player_info->handle);
              memset(test_player_info, 0, sizeof(test_player_info_t));
            }
          else
            {
              media_player_stop(test_player_info->handle);
              media_player_close(test_player_info->handle, 0);
              memset(test_player_info, 0, sizeof(test_player_info_t));
            }
          break;

        case PLAYER_RESET:
          test_player_info->state = MEDIA_EVENT_STOPPING;
          focus_abandon(test_player_info);
          if (player_type == PLAYER_RECORD)
            {
              media_recorder_reset(test_player_info->handle);
              media_recorder_close(test_player_info->handle);
              memset(test_player_info, 0, sizeof(test_player_info_t));
            }
          else
            {
              media_player_reset(test_player_info->handle);
              media_player_close(test_player_info->handle, 0);
              memset(test_player_info, 0, sizeof(test_player_info_t));
            }
          break;

        case PLAYER_CLOSE:
          focus_abandon(test_player_info);
          if (test_player_info->handle != NULL)
            {
              media_player_stop(test_player_info->handle);
              media_player_close(test_player_info->handle, 0);
              memset(test_player_info, 0, sizeof(test_player_info_t));
            }
          break;

        case PLAYER_SET_POS:
          media_player_seek(test_player_info->handle, *(int *)msg.p1);
          test_player_info->position = *(int *)msg.p1;
          free(msg.p1);
          msg.p1 = NULL;
          break;
        case PLAYER_GET_POS:
          media_player_get_position(
              test_player_info->handle,
              (unsigned int *)&test_player_info->position);
          break;

        case PLAYER_GET_DUR:
          media_player_get_duration(
              test_player_info->handle,
              (unsigned int *)&test_player_info->duration);
          break;

        case PLAYER_GET_VOLUME:
          media_player_get_volume(test_player_info->handle,
                                  (float *)&test_player_info->volume);
          break;

        case PLAYER_SET_VOLUME:
          media_player_set_volume(test_player_info->handle,
                                  *(float *)msg.p1);
          free(msg.p1);
          msg.p1 = NULL;
          break;

        case PLAYER_ISPLAYING:
          test_player_info->isplaying =
              media_player_is_playing(test_player_info->handle);
          break;

        case PLAYER_START:
          /* If player_info is not at Stack top, then do not execute start */
          af_top_handle = test_get_af_stack_top();
          if (af_top_handle)
            {
              if (af_top_handle != *test_player_info->af_req_handle)
                {
                  syslog(LOG_INFO, "player is not on the top, break\n");
                  break;
                }
              else
                {
                  test_player_info->state = MEDIA_EVENT_STARTING;
                  if (player_type == PLAYER_RECORD)
                    {
                      media_recorder_start(test_player_info->handle);
                    }
                  else
                    {
                      media_player_start(test_player_info->handle);
                    }
                }
            }
          break;
        
        case PLAYER_EXIT:
          running = 0;
          break;

        default:
          break;
        }
      if (running == 0)
        break;
    }
  /* Never run here */
  mq_close(mqid);
  return -1;
}
