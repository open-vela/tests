/****************************************************************************
 *
 ****************************************************************************/

#ifndef __INCLUDE_AUDIO_MANAGER_H
#define __INCLUDE_AUDIO_MANAGER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "audio_focus.h"
#include "audio_list.h"
#include "audio_player.h"
#include <syslog.h>
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_CHECK_PLAYING_SONG(p)                                              \
  {                                                                        \
    if (!p.test_song_entry)                                                     \
      {                                                                    \
        syslog(LOG_ERR,                                                    \
               "%s, line %d,\
                                        check playing song info failed\n", \
               __func__, __LINE__);                                        \
        return -1;                                                         \
      }                                                                    \
  }

static test_player_info_t *test_get_player_info(test_player_type_t type);

static int test_update_song_items_to_player_info(test_player_type_t type,
                                            test_song_items_t *items);
static void test_update_playing_song_info_by_entry(test_playing_song_info_t *info,
                                              test_song_entry_t *test_song_entry);

int test_start_player_play(test_player_type_t type, test_song_entry_t *test_song_entry);

int test_play_cur_song(void);
int test_play_prev_song(void);
int test_play_next_song(void);
int test_play_resume_song(void);
int test_play_pause_song(void);
int test_play_stop_song(void);

int test_get_current_position(test_player_type_t type);
int test_get_play_duration(test_player_type_t type);
int test_audio_player_is_playing(void);
int test_get_player_play_status(void);
static void test_play_ctrl_lock(void);
static void test_play_ctrl_unlock(void);

int test_get_audio_player_play_mode(void);
void test_set_audio_player_play_mode(loop_mode_t mode);
static void test_player_event_cb(test_player_info_t *info, int event,
                            void *ext_data);
static void test_load_play_list(void);
int test_load_local_all_songs(char *path, test_play_list_t *play_list);
static void test_play_ctrl_mutex_init(void);
static void test_play_ctrl_mutex_deinit(void);
static void *test_audio_manager_task(void *arg);
void test_play_dump(void);

static test_player_info_t *test_get_player_focus_handle(test_player_type_t type);
static test_player_info_t *test_get_player_info_by_focus(void *handle);
static test_player_type_t test_get_player_type_by_focus(void *handle);
int test_get_which_player_is_playing(void);
int test_audio_manager_init(void);
void test_delete_all_play(void);
void test_play_close(test_player_type_t type);
int test_play_common_stop(test_player_type_t type);
int test_play_common_is_playing(test_player_type_t type);
void test_play_setint(char *stream, int volume);

#ifdef __cplusplus
} /* extern "C" */

#endif

#endif