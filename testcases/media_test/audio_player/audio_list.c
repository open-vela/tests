
#include "audio_list.h"
#include <nuttx/list.h>
#include <nuttx/mutex.h>
#include <nuttx/nuttx.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <syslog.h>

static test_play_list_t *g_play_list = NULL;

static void play_list_lock(void)
{
  pthread_mutex_lock(&g_play_list->prop_mutex);
}

static void play_list_unlock(void)
{
  pthread_mutex_unlock(&g_play_list->prop_mutex);
}

static test_play_list_t *play_list_create(void)
{
  test_play_list_t *prop = malloc(sizeof(test_play_list_t));

  if (prop == NULL)
    {
      syslog(LOG_ERR, "Error! create play list failed\n");
      return NULL;
    }
  memset(prop, 0, sizeof(test_play_list_t));
  list_initialize(&prop->song_head);
  /* Initial property mutex */
  pthread_mutex_init(&prop->prop_mutex, NULL);
  return prop;
}

/**
 * @brief Destroy play list
 *
 * @param p
 */
static void play_list_destroy(test_play_list_t *p)
{
  /* destroy mutex */
  pthread_mutex_destroy(&p->prop_mutex);

  /* clear class_head */
  list_clear_node(&p->song_head);
  free(p);
  p = NULL;
}

test_play_list_t *test_play_list_init(void)
{
  g_play_list = play_list_create();
  if (!g_play_list)
    {
      return NULL;
    }
  return g_play_list;
}

void test_play_list_deinit(void)
{
  play_list_destroy(g_play_list);
  g_play_list = NULL;
}

test_play_list_t *test_get_play_list(void) { return g_play_list; }

int test_song_num_of_play_list(void) { return g_play_list->top; }

test_song_entry_t *test_add_new_song(char *id, char *url,
                                     test_song_items_t *test_song_items,
                                     int direction)
{
  if (!id)
    {
      return NULL;
    }
  test_song_entry_t *new = malloc(sizeof(test_song_entry_t));
  if (new == NULL)
    {
      syslog(LOG_ERR, "ERROR: malloc new song entry failed\n");
      return NULL;
    }
  play_list_lock();
  memset(new, 0, sizeof(test_song_entry_t));
  if (direction == SONG_APPEND)
    {
      list_add_before(&g_play_list->song_head, &new->song_list);
    }
  else
    {
      list_add_after(&g_play_list->song_head, &new->song_list);
    }

  char *song_id = malloc(strlen(id) + 1);
  memset(song_id, '\0', strlen(id) + 1);
  strcpy(song_id, id);

  if (url)
    {
      char *song_url = malloc(strlen(url) + 1);
      memset(song_url, '\0', strlen(url) + 1);
      strcpy(song_url, url);
      new->song_url = song_url;
    }
  else
    {
      new->song_url = NULL;
    }
  new->song_id = song_id;

  if (test_song_items)
    {
      test_song_items_t *items = malloc(sizeof(test_song_items_t));
      memset(items, 0, sizeof(test_song_items_t));
      new->test_song_items = items;
      *new->test_song_items = *test_song_items;
    }
  g_play_list->top++;
  play_list_unlock();
  return new;
}

test_song_entry_t *test_get_song_by_id(char *song_id)
{
  test_song_entry_t *test_song_entry, *temp_entry, *ret = NULL;
  if (!song_id)
    {
      return NULL;
    }
  /* Iterates song list */
  list_for_every_entry_safe(&g_play_list->song_head, test_song_entry,
                            temp_entry, test_song_entry_t, song_list)
  {
    if (strcmp(test_song_entry->song_id, song_id) == 0)
      {
        ret = test_song_entry;
        break;
      }
  }
  return ret;
}

int test_delete_song_entry_by_id(char *song_id)
{
  if (!song_id)
    {
      return -1;
    }
  test_song_entry_t *del = test_get_song_by_id(song_id);
  if (del == NULL)
    {
      /* Song not exist */
      return -1;
    }
  play_list_lock();

  MEM_FREE_(del->song_url);
  MEM_FREE_(del->song_id);
  MEM_FREE_(del->test_song_items);
  list_delete(&del->song_list);
  free(del);
  del = NULL;
  g_play_list->top--;
  play_list_unlock();
  return 0;
}

int test_delete_all_song_entry_of_play_list(test_play_list_t *play_list)
{
  if (!play_list)
    {
      return -1;
    }

  test_song_entry_t *test_song_entry, *temp_entry;
  play_list_lock();
  g_play_list->top = 0;

  list_for_every_entry_safe(&play_list->song_head, test_song_entry,
                            temp_entry, test_song_entry_t, song_list)
  {
    MEM_FREE_(test_song_entry->song_url);
    MEM_FREE_(test_song_entry->song_id);
    MEM_FREE_(test_song_entry->test_song_items)

    list_delete(&test_song_entry->song_list);
    free(test_song_entry);
    test_song_entry = NULL;
  }
  play_list_unlock();

  return 0;
}

void test_print_play_list(void)
{
  syslog(LOG_DEBUG, "Print play list...\n");
  if (!g_play_list)
    {
      return;
    }

  test_song_entry_t *test_song_entry = NULL;

  test_song_entry_t *tmp_song_entry = NULL;

  play_list_lock();
  /* iterates song list */
  list_for_every_entry_safe(&g_play_list->song_head, test_song_entry,
                            tmp_song_entry, test_song_entry_t, song_list)
  {

    syslog(LOG_DEBUG, "        song_id:%s\n", test_song_entry->song_id);
    if (test_song_entry->song_url)
      {
        syslog(LOG_DEBUG, "        song_url:%s\n",
               test_song_entry->song_url);
      }
  }
  play_list_unlock();
}

void test_update_playing_song_info(test_playing_song_info_t *info,
                                   char *song_id)
{
  test_song_entry_t *test_song_entry = NULL;

  play_list_lock();

  test_song_entry = test_get_song_by_id(song_id);
  if (test_song_entry)
    {
      info->test_song_entry = test_song_entry;
    }
  else
    {
      syslog(LOG_WARNING, "%s:get song failed\n", __func__);
    }
  play_list_unlock();
}

void test_get_prev_song_entry(test_song_entry_t *in_song_entry,
                              test_song_entry_t **out_song_entry)
{
  if (!in_song_entry)
    {
      return;
    }
  test_song_entry_t *prev_entry = NULL;
  struct list_node *prev;

  prev = in_song_entry->song_list.prev;
  if (prev == &g_play_list->song_head)
    {
      prev_entry = container_of(g_play_list->song_head.prev,
                                test_song_entry_t, song_list);
    }
  else
    {
      prev_entry = container_of(prev, test_song_entry_t, song_list);
    }

  *out_song_entry = prev_entry;
  return;
}

void test_get_next_song_entry(test_song_entry_t *in_song_entry,
                              test_song_entry_t **out_song_entry)
{
  if (!in_song_entry)
    {
      return;
    }
  test_song_entry_t *next_entry = NULL;
  struct list_node *next;

  next = in_song_entry->song_list.next;
  if (next == &g_play_list->song_head)
    {
      next_entry = container_of(g_play_list->song_head.next,
                                test_song_entry_t, song_list);
    }
  else
    {
      next_entry = container_of(next, test_song_entry_t, song_list);
    }

  *out_song_entry = next_entry;
  return;
}
