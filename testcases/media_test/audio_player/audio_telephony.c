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
#include "audio_focus.h"
#include "audio_player.h"
#include "audio_tel_test.h"
#include <errno.h>
#include <fcntl.h>
#include <media_api.h>
#include <nuttx/config.h>
#include <nuttx/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <tapi.h>
#include <uv.h>

static tapi_context g_tapi_context = NULL;
extern void *test_sco_req_handle_0;
extern void *test_ring_req_handle_0;
#define SLOT_ID 0
static bool outgoing_call_start = false;
static bool incoming_call_start = false;
static bool hangup_call_start = false;
static tapi_call_info t_outgoing_call_info = {0};
static tapi_call_info t_incoming_call_info = {0};

static void on_tapi_client_ready(const char *client_name,
                                 void *user_data)
{
  if (client_name != NULL)
    syslog(LOG_DEBUG, "tapi is ready for %s\n", client_name);
}

static void tele_call_async_fun(tapi_async_result *result)
{
  syslog(LOG_DEBUG, "%s : \n", __func__);
  syslog(LOG_DEBUG, "result->msg_id : %d\n", result->msg_id);
  syslog(LOG_DEBUG, "result->status : %d\n", result->status);
  syslog(LOG_DEBUG, "result->arg1 : %d\n", result->arg1);
  syslog(LOG_DEBUG, "result->arg2 : %d\n", result->arg2);

  if (result->status != 0)
    {
      syslog(LOG_DEBUG, "%s msg id : %d result err, return.\n", __func__,
             result->msg_id);
      return;
    }

  if (result->msg_id == MSG_CALL_MERGE_IND ||
      result->msg_id == MSG_CALL_SEPERATE_IND)
    {
      char **ret = result->data;

      syslog(LOG_DEBUG, "conference size : %d\n", result->arg2);

      for (int i = 0; i < result->arg2; i++)
        {
          syslog(LOG_DEBUG, "conference call id : %s\n", ret[i]);
        }
    }
  else if (result->msg_id == EVENT_OEM_RIL_REQUEST_RAW_DONE)
    {
      unsigned char *response = result->data;

      syslog(LOG_DEBUG, "response raw data length : %d\n", result->arg2);
      for (int i = 0; i < result->arg2; i++)
        {
          syslog(LOG_DEBUG, "response raw data : %x\n", response[i]);
        }
    }
  else if (result->msg_id == EVENT_OEM_RIL_REQUEST_STRINGS_DONE)
    {
      char **response = result->data;

      syslog(LOG_DEBUG, "response strings data length : %d\n",
             result->arg2);
      for (int i = 0; i < result->arg2; i++)
        {
          syslog(LOG_DEBUG, "response strings data : %s\n", response[i]);
        }
    }
  else if (result->msg_id == EVENT_REQUEST_DIAL_DONE)
    {

      if (result->status == 0)
        {
          syslog(LOG_DEBUG, "dial successed, call id : %s\n",
                 (char *)result->data);
        }
      else
        {
          syslog(LOG_DEBUG, "dial failed");
        }
    }
  else if (result->msg_id == EVENT_REQUEST_START_DTMF_DONE)
    {
      syslog(LOG_DEBUG, "start dtmf , state : %d\n", result->status);
    }
  else if (result->msg_id == EVENT_REQUEST_STOP_DTMF_DONE)
    {
      syslog(LOG_DEBUG, "stop dtmf , state : %d\n", result->status);
    }
}

static const char *call_state_to_str(tapi_call_status state)
{
  switch (state)
    {
    case CALL_STATUS_ACTIVE:
      return "[start answering]";
    case CALL_STATUS_HELD:
      return "[already hold]";
    case CALL_STATUS_DIALING:
      return "[already dialed]";
    case CALL_STATUS_ALERTING:
      return "[other ring]";
    case CALL_STATUS_INCOMING:
      return "[new calls]";
    case CALL_STATUS_WAITING:
      return "[waiting]";
    case CALL_STATUS_DISCONNECTED:
      return "[already disconnected]";
    default:
      break;
    }
  return "Unknown";
}

void enter_dial_media_policy(void)
{
  media_policy_set_devices_use(MEDIA_DEVICE_MODEM);
  media_policy_set_devices_use(MEDIA_DEVICE_MIC);
  media_policy_set_devices_unuse(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_A2DP);
  media_policy_set_audio_mode(MEDIA_AUDIO_MODE_PHONE);
}

void exit_dial_media_policy(void)
{
  media_policy_set_devices_use(MEDIA_DEVICE_MIC);
  media_policy_set_devices_unuse(MEDIA_DEVICE_MODEM);
  media_policy_set_devices_unuse(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_A2DP);
  media_policy_set_audio_mode(MEDIA_AUDIO_MODE_NORMAL);
}

static void call_state_change_cb(tapi_async_result *result)
{
  tapi_call_info *call_info;

  syslog(LOG_DEBUG, "%s : %d\n", __func__, result->status);
  call_info = (tapi_call_info *)result->data;

  syslog(LOG_DEBUG, "call changed call_id : %s\n", call_info->call_id);
  syslog(LOG_DEBUG, "call state: %d \n", call_info->state);
  syslog(LOG_DEBUG, "call LineIdentification: %s \n",
         call_info->lineIdentification);
  syslog(LOG_DEBUG, "call IncomingLine: %s \n",
         call_info->incoming_line);
  syslog(LOG_DEBUG, "call Name: %s \n", call_info->name);
  syslog(LOG_DEBUG, "call StartTime: %s \n", call_info->start_time);
  syslog(LOG_DEBUG, "call Multiparty: %d \n", call_info->multiparty);
  syslog(LOG_DEBUG, "call RemoteHeld: %d \n", call_info->remote_held);
  syslog(LOG_DEBUG, "call RemoteMultiparty: %d \n",
         call_info->remote_multiparty);
  syslog(LOG_DEBUG, "call Information: %s \n", call_info->info);
  syslog(LOG_DEBUG, "call Icon: %d \n", call_info->icon);
  syslog(LOG_DEBUG, "call Emergency: %d \n",
         call_info->is_emergency_number);
  syslog(LOG_DEBUG, "call disconnect_reason: %d \n\n",
         call_info->disconnect_reason);
  syslog(LOG_INFO, "the phone state is %s\n",
         call_state_to_str(call_info->state));
  if (call_info->state == CALL_STATUS_DIALING ||
      call_info->state == CALL_STATUS_ALERTING)
    {
      outgoing_call_start = true;
      memcpy(&t_outgoing_call_info, call_info, sizeof(tapi_call_info));
      enter_dial_media_policy();
    }
  else if (call_info->state == CALL_STATUS_INCOMING)
    {
      incoming_call_start = true;
      memcpy(&t_incoming_call_info, call_info, sizeof(tapi_call_info));
      player_request_play(PLAYER_SCO);
      enter_dial_media_policy();
    }
  else if (call_info->state == CALL_STATUS_DISCONNECTED)
    {
      if (outgoing_call_start == true &&
          strcmp(t_outgoing_call_info.call_id, call_info->call_id) == 0)
        {
          outgoing_call_start = false;
          memset(&t_outgoing_call_info, 0, sizeof(tapi_call_info));
        }
      if (incoming_call_start == true &&
          strcmp(t_incoming_call_info.call_id, call_info->call_id) == 0)
        {
          incoming_call_start = false;
          memset(&t_incoming_call_info, 0, sizeof(tapi_call_info));
        }
      test_audio_focus_abandon(test_sco_req_handle_0);
      exit_dial_media_policy();
      hangup_call_start = false;
    }
}

void *uv_keep(void *arg)
{
  int ret = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  if(ret <0){
    syslog(LOG_ERR, "UV start failed\n");
    return NULL;
  }
  ret = uv_loop_close(uv_default_loop());
  syslog(LOG_INFO, "[%s][%d] out:%d\n", __func__, __LINE__, ret);
  return NULL;
}

static int player_request_play(test_player_type_t type)
{
  int ret = -1;
  int play_ret = -1;

  switch (type)
    {
    case PLAYER_SCO:
      test_sco_req_handle_0 =
          test_audio_focus_request(&play_ret, AUDIO_STREAM_SCO,
                                   &modem_focus_change_listener, NULL);

      if (test_sco_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_MODEM) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_sco_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_MODEM) failed");
        }
      break;

    case PLAYER_RING:
      test_ring_req_handle_0 =
          test_audio_focus_request(&play_ret, AUDIO_STREAM_RING,
                                   &modem_focus_change_listener, NULL);

      if (test_ring_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_RING) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_ring_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_RING) failed");
        }
      break;

    default:
      break;
    }
  ret = play_ret;
  test_audio_focus_debug_stack_display();
  return ret;
}

int init_tapi_and_modem(void)
{
  if (g_tapi_context != NULL)
    {
      return 0;
    }

  g_tapi_context =
      tapi_open("media.telephone.test", on_tapi_client_ready, NULL);
  syslog(LOG_INFO, "tapi_open g_tapi_context : %p\n", g_tapi_context);
  if (g_tapi_context == NULL)
    {
      syslog(LOG_ERR, "LOG : tapi_open failed");
      return -1;
    }
  reset_call_info();
  pthread_t thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_attr_setstacksize(&attr, 4096);

  pthread_create(&thread, &attr, uv_keep, NULL);

  return 0;
}

int test_dial_listen(void)
{
  syslog(LOG_INFO, "context %p, slot_id %d\n", g_tapi_context, SLOT_ID);
  int watch_id = tapi_call_register_call_state_change(
      g_tapi_context, SLOT_ID, NULL, call_state_change_cb);

  syslog(LOG_DEBUG, "%s, slot_id : %d,  watch_id : %d \n", __func__,
         SLOT_ID, watch_id);
  return watch_id;
}

static void modem_focus_change_listener(int play_ret, void *call_arg)
{
  syslog(
      LOG_INFO,
      "change listener trigered: play ret is: %d, player_type is: %s\n",
      play_ret, "MODEM");

  switch (play_ret)
    {
    case AUDIO_FOCUS_PLAY:
      break;
    case AUDIO_FOCUS_STOP:
      break;
    case AUDIO_FOCUS_PAUSE:
      break;
    case AUDIO_FOCUS_PLAY_BUT_SILENT:
      break;
    case AUDIO_FOCUS_PLAY_WITH_DUCK:
      break;
    case AUDIO_FOCUS_PLAY_WITH_KEEP:
      break;
    }
}

int player_hangup_phone(void)
{
  if (outgoing_call_start == false && incoming_call_start == false)
    {
      syslog(LOG_INFO, "no ongoing calls");
      return 0;
    }

  if (strlen(t_outgoing_call_info.call_id) == 0 &&
      strlen(t_incoming_call_info.call_id) == 0)
    {
      syslog(LOG_INFO, "no calling ID");
      return 0;
    }

  if (hangup_call_start == true)
    {
      syslog(LOG_INFO, "hanging up in progress");
      return 0;
    }

  char *call_id = t_outgoing_call_info.call_id;
  if (strlen(t_outgoing_call_info.call_id) == 0)
    {
      call_id = t_incoming_call_info.call_id;
    }

  int ret = tapi_call_hangup_by_id(g_tapi_context, SLOT_ID, call_id);
  if (ret == 0)
    {
      hangup_call_start = true;
      syslog(LOG_INFO,
             "tapi_call_hangup_call ret : %d \nhangup call : %s", ret,
             call_id);
    }
  else
    {
      syslog(LOG_ERR,
             "tapi_call_hangup_call ret : %d \nhangup failed : %s", ret,
             call_id);
    }
  return 0;
}

static void reset_call_info(void)
{
  memset(&t_outgoing_call_info, 0, sizeof(tapi_call_info));
  memset(&t_incoming_call_info, 0, sizeof(tapi_call_info));

  outgoing_call_start = false;
  incoming_call_start = false;
}

int palyer_answer_call(void)
{
  if (incoming_call_start == false)
    {
      syslog(LOG_INFO, "no new incoming calls");
      return -1;
    }

  if (strlen(t_incoming_call_info.call_id) == 0)
    {
      syslog(LOG_INFO, "no incoming calls ID\n");
      return -2;
    }

  syslog(LOG_INFO, "context is %p, call id is %s\n", g_tapi_context,
         t_incoming_call_info.call_id);
  int ret = tapi_call_answer_by_id(g_tapi_context, SLOT_ID,
                                   t_incoming_call_info.call_id);
  syslog(LOG_INFO, "answer a new call : ret = %d", ret);
  return ret;
}

int player_new_call(const char *number)
{
  if (outgoing_call_start == true)
    {
      syslog(LOG_INFO, "The phone is currently being dialed...\n");
      return -1;
    }

  if (incoming_call_start == true)
    {
      syslog(LOG_INFO, "Please answer/end a new call first\n");
      return -1;
    }
  int ret = 0;
  int return_type = AUDIO_FOCUS_PLAY;
  return_type = player_request_play(PLAYER_SCO);
  if (return_type == -1)
    {
      return -1;
    }

  switch (return_type)
    {
    case AUDIO_FOCUS_PLAY:
      syslog(LOG_DEBUG, "dial start\n");
      ret = tapi_call_dial(g_tapi_context, 0, (char *)number, 0,
                           EVENT_REQUEST_DIAL_DONE, tele_call_async_fun);
      if (ret == 0)
        {
          outgoing_call_start = true;
          syslog(LOG_INFO, "dial call %s start\n", number);
        }
      else
        {
          syslog(LOG_INFO, "dial call %s failed %d\n", number, ret);
        }
      break;
    case AUDIO_FOCUS_STOP:
      ret = -1;
      break;
    }
  return ret;
}