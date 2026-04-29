#ifndef __INCLUDE_AUDIO_LIST_H
#define __INCLUDE_AUDIO_LIST_H

#include <nuttx/list.h>
#include <nuttx/mutex.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SONG_APPEND 0
#define SONG_FRONT 1
#define MEM_FREE_(p)                                                    \
  {                                                                     \
    if (p)                                                              \
      {                                                                 \
        free(p);                                                        \
        p = NULL;                                                       \
      }                                                                 \
  }

typedef enum
{
  LP_LOOP = 1, // Loop play
  LP_SINGLE    // loop one song
} loop_mode_t;

typedef struct test_play_list_property
{
  struct list_node song_head;
  int top;
  pthread_mutex_t prop_mutex;
} test_play_list_t;

typedef struct test_song_items
{
  int offset_in_ms;
  int duration_in_ms;
} test_song_items_t;

typedef struct test_song_entry
{
  struct list_node song_list;
  char *song_url;
  test_song_items_t *test_song_items;
  char *song_id;
} test_song_entry_t;

typedef struct test_playing_song_info
{
  test_song_entry_t *test_song_entry;
} test_playing_song_info_t;

test_play_list_t *test_play_list_init(void);
void test_play_list_deinit(void);
int test_song_num_of_play_list(void);
test_play_list_t *test_get_play_list(void);
test_song_entry_t *test_add_new_song(char *id, char *url, test_song_items_t *test_song_items,
                           int direction);
test_song_entry_t *test_get_song_by_id(char *song_id);
int test_delete_song_entry_by_id(char *song_id);
int test_delete_all_song_entry_of_play_list(test_play_list_t *play_list);
void test_print_play_list(void);
void test_update_playing_song_info(test_playing_song_info_t *info, char *song_id);
void test_get_prev_song_entry(test_song_entry_t *in_song_entry,
                         test_song_entry_t **out_song_entry);
void test_get_next_song_entry(test_song_entry_t *in_song_entry,
                         test_song_entry_t **out_song_entry);

#endif