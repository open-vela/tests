
#ifndef __AUDIO_TELEPHONY_TEST_H
#define __AUDIO_TELEPHONY_TEST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "audio_focus.h"
#include <media_api.h>
#include <tapi.h>

static void on_tapi_client_ready(const char *client_name,
                                 void *user_data);
static void tele_call_async_fun(tapi_async_result *result);
static const char* call_state_to_str(tapi_call_status state);
void enter_dial_media_policy(void);
void exit_dial_media_policy(void);
static void call_state_change_cb(tapi_async_result *result);
int init_tapi_and_modem(void);
int test_dial_listen(void);
static void modem_focus_change_listener(int play_ret, void *call_arg);
int player_hangup_phone(void);
static void reset_call_info(void);
int palyer_answer_call(void);
int player_new_call(const char *number);
static int player_request_play(test_player_type_t type);

#endif /* __AUDIO_TELEPHONY_H */