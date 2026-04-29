#include "audio_focus.h"
#include <assert.h>
#include <nuttx/nuttx.h>
#include <nuttx/list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
// #define LOG_TAG "audio_focus"
// #include <mico/nx/nxlog.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/
// Audio matrix value
typedef enum
{
  ACC = 0,
  REJ,
  LOS,
  BAC,
  BAC_D,
  BAC_K, // 5
  NONE,
} test_interactive_value_t;

typedef struct test_focus_data
{
  int focus_level;
  int focus_state; // Saveinteractive_value_t
  audio_focus_callback focus_callback;
  void *callback_argv;
} test_focus_data_t;

typedef struct af_stack
{
  struct list_node list_head;
  int top;
  int capacity;
  pthread_mutex_t mutex;
} test_af_stack_t;

typedef struct af_stack_entry
{
  struct list_node list;
  test_focus_data_t data;
} test_af_stack_entry_t;

struct test_audio_matrix
{
  int req_lv;
  int top_lv;
  test_interactive_value_t req_ret;
  test_interactive_value_t top_ret;
};

/****************************************************************************
 * Public Data
 ****************************************************************************/
static test_af_stack_t *g_afs = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void test_print_each_stack_item(test_af_stack_t *af);

static void test_af_stack_lock(void)
{
  // syslog(LOG_DEBUG, "test_af_stack_lock\n");
  pthread_mutex_lock(&g_afs->mutex);
}

static void test_af_stack_unlock(void)
{
  // syslog(LOG_DEBUG, "test_af_stack_unlock\n");
  pthread_mutex_unlock(&g_afs->mutex);
}

static test_af_stack_t *test_af_stack_init(void)
{
  test_af_stack_t *af_stack = malloc(sizeof(test_af_stack_t));
  if (af_stack == NULL)
    {
      syslog(LOG_ERR, "Error! malloc failed");
      return NULL;
    }
  list_initialize(&af_stack->list_head);
  af_stack->capacity = AUDIO_FOCUS_STACK_CAP;
  af_stack->top = 0;
  /* Initial property mutex */
  pthread_mutex_init(&af_stack->mutex, NULL);
  return af_stack;
}
#if 0
static void af_stack_destroy(test_af_stack_t *head)
{
    assert(head);
    test_af_stack_lock();
    test_af_stack_entry_t *af_stack_entry, *temp_entry;
    list_for_every_entry_safe(&head->list_head, af_stack_entry, temp_entry, test_af_stack_entry_t, list)
    {        
        if(af_stack_entry)
        {
            list_delete(&af_stack_entry->list);		
            free(af_stack_entry);
            af_stack_entry = NULL;
        }
    }
    list_clear_node(&head->list_head);
    /* destroy mutex */
    pthread_mutex_destroy(&head->mutex);    
 	head->capacity = 0;
	head->top = 0; 
    free(head);
    test_af_stack_unlock();
}
#endif
static void test_af_stack_push(test_af_stack_t *af, test_focus_data_t x)
{
  if (!af)
    {
      return;
    }
  if (af->top == af->capacity)
    {
      syslog(LOG_ERR, "Stack full, return\n");
      return;
    }
  test_af_stack_entry_t *af_stack_entry =
      malloc(sizeof(test_af_stack_entry_t));
  af_stack_entry->data = x;
  list_add_after(&af->list_head, &af_stack_entry->list);
  af->top++;
}

static void test_af_stack_pop(test_af_stack_t *af)
{
  if (!af || af->top <= 0)
    {
      return;
    }

  /* print af stack */
  test_print_each_stack_item(g_afs);
  test_af_stack_entry_t *af_stack_entry =
      container_of(af->list_head.next, test_af_stack_entry_t, list);
  if (af_stack_entry)
    {
      list_delete(&af_stack_entry->list);
      free(af_stack_entry);
      af_stack_entry = NULL;
      af->top--;
      syslog(LOG_INFO, "%s success\n", __func__);
    }
}

static void
test_af_stack_delete_item(test_af_stack_t *af,
                          test_af_stack_entry_t *af_stack_entry)
{
  if (!af || af->top <= 0)
    {
      return;
    }

  /* print af stack */
  test_print_each_stack_item(g_afs);
  if (af_stack_entry)
    {
      test_af_stack_entry_t *stack_entry, *temp_entry;
      list_for_every_entry_safe(&af->list_head, stack_entry, temp_entry,
                                test_af_stack_entry_t, list)
      {
        if (stack_entry == af_stack_entry)
          {
            list_delete(&af_stack_entry->list);
            free(af_stack_entry);
            af_stack_entry = NULL;
            syslog(LOG_INFO, "%s success\n", __func__);
            af->top--;
          }
      }
    }
}

static bool test_af_stack_empty(test_af_stack_t *af)
{
  if (!af)
    {
      return true;
    }
  return af->top == 0;
}

static test_af_stack_entry_t *test_af_stack_top(test_af_stack_t *af)
{
  if (!af || af->top <= 0)
    {
      return NULL;
    }

  test_af_stack_entry_t *af_stack_entry =
      container_of(af->list_head.next, test_af_stack_entry_t, list);
  return af_stack_entry;
}

static bool test_af_stack_full(test_af_stack_t *af)
{
  if (!af)
    {
      return true;
    }

  return af->top == AUDIO_FOCUS_STACK_CAP;
}

static void
test_audio_focus_arbitration(struct test_audio_matrix *matrix)
{
  if (!matrix)
    {
      return;
    }

  switch (matrix->req_lv)
    {
    case AUDIO_STREAM_SCO:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        default:
          break;
        }
        break;
    case AUDIO_STREAM_RING:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        default:
          break;
        }
      break;
    case AUDIO_STREAM_ALARM:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        default:
          break;
        }
      break;
    case AUDIO_STREAM_ENFORCED:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        default:
          break;
        }
      break;
    case AUDIO_STREAM_NOTIFY:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = BAC_D;
          break;
        default:
          break;
        }
      break;
    case AUDIO_STREAM_RECORD:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        }
      break;
    case AUDIO_STREAM_TTS:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = LOS;
          break;
        }
      break;
    case AUDIO_STREAM_HEALTH:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = BAC_D;
          break;
        }
      break;
    case AUDIO_STREAM_SPORT:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = BAC_D;
          break;
        }
      break;
    case AUDIO_STREAM_INFO:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = BAC_D;
          break;
        }
      break;
    case AUDIO_STREAM_MUSIC:
      switch (matrix->top_lv)
        {
        case AUDIO_STREAM_SCO:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RING:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ALARM:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_ENFORCED:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_NOTIFY:
          matrix->req_ret = BAC_D;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_RECORD:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_TTS:
          matrix->req_ret = REJ;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_HEALTH:
          matrix->req_ret = BAC_D;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_SPORT:
          matrix->req_ret = BAC_D;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_INFO:
          matrix->req_ret = BAC_D;
          matrix->top_ret = NONE;
          break;
        case AUDIO_STREAM_MUSIC:
          matrix->req_ret = ACC;
          matrix->top_ret = REJ;
          break;
        }
      break;
    default:
      break;
    }
}

/****************************************************************************
 * Public functions
 ****************************************************************************/
void test_audio_focus_init(void) { g_afs = test_af_stack_init(); }

int test_get_af_stack_used(void)
{
  if (g_afs)
    {
      return g_afs->top;
    }
  return 0;
}

/**
 * @brief Allow application to request audio focus.
 *
 * @param[out] return_type      pointer of play return suggestion for app
 * @param[in]  stream_type      one of stream types defined in
 * media_api.h
 * @param[in]  callback_method  callback method of request app
 * @param[in]  callback_argv    argument of callback
 * @return     NULL when request failed, void* handle for request.
 * @note       Value of return_type are announced above.
 */
void *test_audio_focus_request(int *return_type, int stream_type,
                               audio_focus_callback callback_method,
                               void *callback_argv)
{
  if (!callback_method)
    {
      return NULL;
    }

  test_focus_data_t fdata = {0};
  test_af_stack_entry_t *req_handle = NULL;

  if (test_af_stack_full(g_afs))
    {
      syslog(LOG_ERR, "audio focus stack full\n");
      return NULL;
    }

  test_af_stack_lock();

  if (test_af_stack_empty(g_afs))
    {
      /* Empty Stack */
      fdata.callback_argv = callback_argv,
      fdata.focus_callback = callback_method,
      fdata.focus_level = stream_type, fdata.focus_state = ACC;
      *return_type = AUDIO_FOCUS_PLAY;
      test_af_stack_push(g_afs, fdata);
      req_handle = test_af_stack_top(g_afs);
      test_af_stack_unlock();
      return (void *)req_handle;
    }

  // Get current Stack top Priority
  test_af_stack_entry_t *top = test_af_stack_top(g_afs);
  struct test_audio_matrix matrix = {0};
  matrix.req_lv = stream_type;
  matrix.top_lv = top->data.focus_level;
  // RequestPriority and Stack top Priority arbitration
  test_audio_focus_arbitration(&matrix);

  int top_return_type = AUDIO_FOCUS_PLAY;
  switch (matrix.req_ret)
    {
    case ACC:
      *return_type = AUDIO_FOCUS_PLAY;
      break;
    case REJ:
      *return_type = AUDIO_FOCUS_STOP;
      break;
    case BAC_D:
      *return_type = AUDIO_FOCUS_PLAY_WITH_DUCK;
      break;
    case NONE:
      *return_type = AUDIO_FOCUS_STOP;
    default:
      break;
    }

  switch (matrix.top_ret)
    {
    case REJ:
      top_return_type = AUDIO_FOCUS_STOP;
      break;
    case LOS:
      top_return_type = AUDIO_FOCUS_PAUSE;
      break;
    case BAC_K:
      top_return_type = AUDIO_FOCUS_PLAY_WITH_KEEP;
      break;
    case BAC_D:
      top_return_type = AUDIO_FOCUS_PLAY_WITH_DUCK;
      break;
    default:
      break;
    }

  /* req gets focus Point */
  if (matrix.req_ret == ACC)
    {
      // Execute Stack top Callback
      top->data.focus_callback(top_return_type, top->data.callback_argv);
      top->data.focus_state = matrix.top_ret;

      // req enters Stack
      fdata.callback_argv = callback_argv,
      fdata.focus_callback = callback_method,
      fdata.focus_level = matrix.req_lv,
      fdata.focus_state = matrix.req_ret;
      test_af_stack_push(g_afs, fdata);
      req_handle = test_af_stack_top(g_afs);
    }
  test_af_stack_unlock();
  return (void *)req_handle;
}

int test_audio_focus_abandon(void *handle)
{
  if (!handle)
    {
      return -1;
    }

  test_af_stack_lock();
  if (g_afs->top <= 0)
    {
      test_af_stack_unlock();
      return -1;
    }
  test_af_stack_entry_t *top = test_af_stack_top(g_afs);
  int top_return_type = AUDIO_FOCUS_PLAY;
  syslog(LOG_INFO, "test_audio_focus_abandon: %p\n", handle);
  if (top == handle && g_afs->top > 1)
    {
      /* Delete Stack top, need to compare playback suggestion after deletion */
      test_af_stack_pop(g_afs);
      top = test_af_stack_top(g_afs);
      if (top->data.focus_state == LOS || top->data.focus_state == BAC_D)
        {
          top->data.focus_state = ACC; // Get focus Point again
        }
      switch (top->data.focus_state)
        {
        case ACC:
          top_return_type = AUDIO_FOCUS_PLAY;
          break;
        case BAC_K:
          top_return_type = AUDIO_FOCUS_PLAY_WITH_KEEP;
          break;
        case REJ:
          top_return_type = AUDIO_FOCUS_STOP;
          break;
        case BAC_D:
          top_return_type = AUDIO_FOCUS_PLAY_WITH_DUCK;
          break;
        default:
          break;
        }
      top->data.focus_callback(top_return_type, top->data.callback_argv);
    }
  else
    {
      /* Non-Stack top element */
      test_af_stack_delete_item(g_afs, (test_af_stack_entry_t *)handle);
    }
  test_af_stack_unlock();
  return 0;
}

/**
 * @brief Get the af stack top object
 *
 * @return void*
 */
void *test_get_af_stack_top(void)
{
  void *handle = NULL;
  if (g_afs)
    {
      if (g_afs->top > 0)
        {
          handle = (void *)test_af_stack_top(g_afs);
        }
    }
  return handle;
}

/*****************************************************************************
 *   Debug functions
 ******************************************************************************/
static void test_print_stack_item(int client_id,
                                  test_af_stack_entry_t *af_stack_entry,
                                  test_focus_data_t *x)
{
  if (!af_stack_entry || !x)
    {
      syslog(LOG_INFO, "%s:failed\n", __func__);
      return;
    }
  char *focus_level = NULL;
  char *focus_state = NULL;
  switch (x->focus_level)
    {
    case 0:
      focus_level = "NOTIFY";
      break;
    case 1:
      focus_level = "TTS";
      break;
    case 2:
      focus_level = "MUSIC";
      break;
    case 3:
      focus_level = "ALARM";
      break;
    case 4:
      focus_level = "SCO";
      break;
    case 5:
      focus_level = "RING";
      break;
    case 6:
      focus_level = "ENFORCED";
      break;
    case 7:
      focus_level = "RECORD";
      break;
    case 8:
      focus_level = "HEALTH";
      break;
    case 9:
      focus_level = "SPORT";
      break;
    case 10:
      focus_level = "INFO";
      break;
    default:
      break;
    }
  if (x->focus_state == 0)
    {
      focus_state = "PLAY";
    }
  if (x->focus_state == 1)
    {
      focus_state = "STOP";
    }
  if (x->focus_state == 2)
    {
      focus_state = "PAUSE";
    }
  if (x->focus_state == 3)
    {
      focus_state = "PLAY_BUT_SILENT";
    }
  if (x->focus_state == 4)
    {
      focus_state = "PLAY_WITH_DUCK";
    }

  syslog(LOG_INFO,
         "client id:%d, focus handle:%p, focus level:%s, focus "
         "state:%s, callback argv:%p\n",
         client_id, af_stack_entry, focus_level, focus_state,
         x->callback_argv);
}

static void test_print_each_stack_item(test_af_stack_t *af)
{
  test_af_stack_entry_t *af_stack_entry, *temp_entry;
  int i = g_afs->top;
  list_for_every_entry_safe(&af->list_head, af_stack_entry, temp_entry,
                            test_af_stack_entry_t, list)
  {
    test_print_stack_item(i, af_stack_entry, &af_stack_entry->data);
    i--;
  }
}

#if 0
static void print_stack_top(void)
{
    af_stack_entry_t *af_stack_entry = test_af_stack_top(g_afs);
    test_focus_data_t f_top = af_stack_entry->data;
    syslog(LOG_INFO, "Top:");
    test_print_stack_item(0, &f_top);
}
#endif
void test_audio_focus_debug_stack_display(void)
{
  syslog(LOG_INFO, "START:audio focus stack display\n");
  test_print_each_stack_item(g_afs);
  syslog(LOG_INFO, "END:audio focus stack display\n");
}
