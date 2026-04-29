#include <nuttx/nuttx.h>
#include "video_test.h"
#include "audio_list.h"

static int video_runing;
static uv_async_queue_t g_videotest_queue;

static test_play_list_t *video_play_list = NULL;
static pthread_t g_video_thread;
static video_pri ctrlvideo = {0};

#define LOG_D(fmt, ...) MEDIATEST_DEBUG(videotest_framework, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) MEDIATEST_INFO(videotest_framework, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) MEDIATEST_WARNING(videotest_framework, fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) MEDIATEST_ERR(videotest_framework, fmt, ##__VA_ARGS__)

/*mqueue calling method*/

// function declaration
static int play_cur_video(video_pri *pri);
static int play_prev_video(video_pri *pri);
static int play_next_video(video_pri *pri);
static int play_resume_video(video_pri *pri);
static int play_seek_video(video_pri *pri);
static int play_pause_video(video_pri *pri);
static int play_stop_video(video_pri *pri);
static int play_stop_inner_video(video_pri *pri);
static int get_play_position(video_pri *pri);
static int get_play_duration(video_pri *pri);
static int get_play_isplaying(video_pri *pri);

static void start_event_cb(void *obj);
static void stop_event_cb(void *obj);
static void pause_event_cb(void *obj);
static void completed_event_cb(void *obj);
static void prepared_event_cb(void *obj);
static void duration_event_cb(void *obj, int ret, unsigned duration);
static void isplaying_event_cb(void *obj, int ret, int val);

static int mediatest_load_local_all_songs(char *path,
                                          test_play_list_t *play_list_);

static void video_suit_poll_cb(uv_poll_t *handle, int status, int events);


static int video_start_queue(uv_loop_t *loop);
void *g_video_test_loop_thread(void *parm);
// Global function definition

static int play_cur_video(video_pri *pri)
{
    int ret = 0;
    if(pri->file && video_play_list == NULL)
        mediatest_load_play_list(pri);
    pri->video = vg_video_create(lv_scr_act());
    lv_obj_set_size(pri->video, LV_PCT(100), LV_PCT(100));
    vg_video_set_src_opt(pri->video, pri->url, pri->option);
    vg_video_set_callback(pri->video, MEDIA_EVENT_STARTED, pri,
                           start_event_cb);
    vg_video_set_callback(pri->video, MEDIA_EVENT_PREPARED, pri,
                           prepared_event_cb);
    vg_video_set_callback(pri->video, MEDIA_EVENT_COMPLETED, pri,
                           completed_event_cb);
    vg_video_set_callback(pri->video, MEDIA_EVENT_PAUSED, pri,
                           pause_event_cb);
    vg_video_set_callback(pri->video, MEDIA_EVENT_STOPPED, pri,
                           stop_event_cb);

    lv_obj_align(pri->video, LV_ALIGN_CENTER, 0, 0);
    ret = vg_video_start(pri->video);
    pri->state = VIDEO_PLAYSTATE_PLAY;
    return ret;
}

static int play_prev_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    test_song_entry_t *test_song_entry = pri->test_song_entry;
    LOG_I("get prev song...\n");
    test_get_prev_song_entry(pri->test_song_entry, &test_song_entry);
    if (!test_song_entry)
    {
        LOG_E("get prev song failed\n");
        test_song_entry = pri->test_song_entry;
    }

    LOG_I("url is %s\n", test_song_entry->song_url);
    pri->url = test_song_entry->song_url;
    pri->test_song_entry= test_song_entry;
    pri->ops = VIDEO_CTRL_PLAY;
    send_msg_video(pri);
    return ret;
}

static int play_next_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    test_song_entry_t *test_song_entry = pri->test_song_entry;
    LOG_I("get next song...\n");
    test_get_next_song_entry(pri->test_song_entry, &test_song_entry);
    if (!test_song_entry)
    {
        LOG_E("get next song failed\n");
        test_song_entry = pri->test_song_entry;
    }

    LOG_I("url is %s\n", test_song_entry->song_url);
    pri->url = test_song_entry->song_url;
    pri->test_song_entry= test_song_entry;
    pri->ops = VIDEO_CTRL_PLAY;
    send_msg_video(pri);
    return ret;
}

static int play_resume_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    int state = pri->state;
    if (state == VIDEO_PLAYSTATE_PAUSE)
    {
        ret = vg_video_resume(pri->video);
        pri->state = VIDEO_PLAYSTATE_PLAY;
    }

    return ret;
}

static int play_seek_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    ret = vg_video_seek(pri->video, pri->pos);
    return ret;
}

static int play_pause_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    int state = pri->state;
    if (state == VIDEO_PLAYSTATE_PLAY)
    {
        ret = vg_video_pause(pri->video);
        pri->state = VIDEO_PLAYSTATE_PAUSE;
    }
    return ret;
}
static int play_stop_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    ret = vg_video_stop(pri->video);
    lv_obj_del(pri->video);
    pri->state = VIDEO_PLAYSTATE_STOP;
    if(video_play_list){
        test_delete_all_song_entry_of_play_list(video_play_list);
        video_play_list = NULL;
    }
    memset(pri, 0, sizeof(video_pri));
    
    return ret;
}

static int play_stop_inner_video(video_pri *pri)
{
    int ret = 0;
    CHECK_PLAYING_VIDEO(pri);
    ret = vg_video_stop(pri->video);
    lv_obj_del(pri->video);
    pri->state = VIDEO_PLAYSTATE_STOP;
    pri->video = NULL;
    return ret;
}

static int get_play_position(video_pri *pri)
{
    int ret = -1;
    CHECK_PLAYING_VIDEO(pri);
    if (!pri->state || (pri->state == MEDIA_EVENT_STOPPED))
        return ret;
    vg_video_t *video = (vg_video_t *)pri->video;
    
    pri->pos = video->cur_time;
    return (int)pri->pos;
}
static int get_play_duration(video_pri *pri)
{
    int ret = -1;
    CHECK_PLAYING_VIDEO(pri);
    if (!pri->state || (pri->state == MEDIA_EVENT_STOPPED))
        return ret;
    ret = vg_video_get_dur(pri->video, duration_event_cb, pri);
    return ret;
}
static int get_play_isplaying(video_pri *pri)
{
    int ret = -1;
    CHECK_PLAYING_VIDEO(pri);
    ret = vg_video_get_playing(pri->video, isplaying_event_cb, pri);
    return ret;
}

//  Function callback definition

static void start_event_cb(void *obj)
{
    video_pri *pri = (video_pri *)obj;
    pri->state = VIDEO_PLAYSTATE_PLAY;
    return;
}
static void stop_event_cb(void *obj)
{
    video_pri *pri = (video_pri *)obj;
    pri->state = VIDEO_PLAYSTATE_STOP;
    return;
}
static void pause_event_cb(void *obj)
{
    video_pri *pri = (video_pri *)obj;
    pri->state = VIDEO_PLAYSTATE_PAUSE;
    return;
}
static void completed_event_cb(void *obj)
{
    video_pri *pri = (video_pri *)obj;
    play_stop_inner_video(pri);
    pri->ops = VIDEO_EVENT_ONEND;
    send_msg_video(pri);
    return;
}
static void prepared_event_cb(void *obj)
{
    video_pri *pri = (video_pri *)obj;
    pri->state = VIDEO_PLAYSTATE_READY;
    get_play_duration(pri);
    return;
}
static void duration_event_cb(void *obj, int ret, unsigned duration)
{
    if (ret < 0)
    {
        LOG_E("recv duration FAILED, ret is %d\n", ret);
        return;
    }

    video_pri *pri = (video_pri *)obj;
    vg_video_t *video = (vg_video_t *)pri->video;

    video->duration = duration / 1000;
    return;
}
static void isplaying_event_cb(void *obj, int ret, int val)
{
    if (ret < 0)
    {
        LOG_E("recv isplaying FAILED, ret is %d\n", ret);
        return;
    }
    video_pri *pri = (video_pri *)obj;
    pri->isplaying = val;
    return;
}

// auxiliary function

static int mediatest_load_local_all_songs(char *path,
                                          test_play_list_t *play_list_)
{
    if (!path || !play_list_)
    {
        return -1;
    }
    FILE *fp = fopen(path, "r");
    char line[256];
    int id = 1;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int len = strlen(line);
        char c_id[200];

        while (isspace(line[len - 1]))
            len--;

        line[len] = '\0';
        sprintf(c_id, "%d", id++);
        test_add_new_song(c_id, line, NULL, 1);
    }
    fclose(fp);
    return 0;
}

static void mediatest_load_play_list(video_pri *pri)
{
    LOG_I(
        "********************************\n");
    LOG_I("parse play list\n");

    video_play_list = test_play_list_init();
    if (!video_play_list)
    {
        video_play_list = test_play_list_init();
        if (!video_play_list)
        {
            LOG_E("ERROE test_play_list_init failed\n");
            return;
        }
    }

    test_song_entry_t *test_song_entry = NULL;

    mediatest_load_local_all_songs(pri->file, video_play_list);
    test_song_entry = container_of(video_play_list->song_head.next,
                                   test_song_entry_t, song_list);

    pri->url = test_song_entry->song_url;
    pri->test_song_entry = test_song_entry;

    test_print_play_list();

    return;
}

static void videotest_queue_cb(uv_async_queue_t *queue_async, void *data)
{
  if (queue_async == NULL || data == NULL)
    {
     LOG_E("videotest_player invalid\n");
      return;
    }
    struct videotest_app *player = data;
    player->uv_play(player->priv);
    free(player);
}

/*mqueue main*/
static int video_start_queue(uv_loop_t *loop)
{
    uv_async_queue_init(loop, &g_videotest_queue, videotest_queue_cb);
    return 0;
}

#ifdef CONFIG_LV_USE_NUTTX_LIBUV

static void* lv_nuttx_uv_loop_init(uv_loop_t* loop, lv_nuttx_result_t* result)
{
    lv_nuttx_uv_t uv_info;
    lv_memset(&uv_info, 0, sizeof(uv_info));

    uv_loop_init(loop);
    uv_info.loop = loop;
    uv_info.disp = result->disp;
    uv_info.indev = result->indev;
#ifdef CONFIG_UINPUT_TOUCH
  uv_info.uindev = result->utouch_indev;
#endif

    return lv_nuttx_uv_init(&uv_info);
}

static void lv_nuttx_uv_loop_run(uv_loop_t* loop, void* data)
{
    uv_run(loop, UV_RUN_DEFAULT);
    lv_nuttx_uv_deinit(&data);
}
#endif


void *g_video_test_loop_thread(void *parm) {
    lv_nuttx_dsc_t info;
    lv_nuttx_result_t result;
    void *param = NULL;

    #ifdef CONFIG_LV_USE_NUTTX_LIBUV
    uv_loop_t ui_loop = { .data=NULL };
    #endif

    if (lv_is_initialized()) {
            LV_LOG_ERROR("lvgl is already initialized, exit!");
            return 0;
        }

    lv_init();

    lv_nuttx_dsc_init(&info);
    lv_nuttx_init(&info, &result);
    vg_init();

    #ifdef CONFIG_LV_USE_NUTTX_LIBUV
        void* data = lv_nuttx_uv_loop_init(&ui_loop, &result);
        vg_uv_init(&ui_loop);
        param = &ui_loop;
    #endif


    if (result.disp == NULL)
        {
        LV_LOG_ERROR("lv_demos initialization failure!");
        return NULL;
        }

    video_start_queue(param);
    

    #ifdef CONFIG_LV_USE_NUTTX_LIBUV
    lv_nuttx_uv_loop_run(&ui_loop, data);
    #else
    while (1)
        {
        lv_timer_handler();
        usleep(10 * 1000);
        }
    #endif

    vg_uv_deinit();
    lv_disp_remove(result.disp);
    vg_deinit();
    lv_deinit();

    return NULL;
}

void video_start_thread(void){
    pthread_attr_t attr __attribute__((unused));
    struct sched_param param __attribute__((unused));
    param.sched_priority = 102;

    pthread_attr_init(&attr);

    pthread_attr_setstacksize(&attr, 100 * 1024);

    pthread_attr_setschedparam(&attr, &param);

    pthread_create(&g_video_thread, &attr, g_video_test_loop_thread, NULL);
    sleep(1);
    LOG_I("Thread started\n");
    return;
}


static int create_mq(char *path)
{
  int mqid = -1;
  struct mq_attr attr;
  attr.mq_flags = 0; // QueueProperty：Blocking
  attr.mq_maxmsg = PLAYER_MQ_MSG_LEN;
  attr.mq_msgsize = sizeof(video_pri);
  attr.mq_curmsgs = 0;
  mqid = mq_open(path, O_RDWR | O_CREAT, NULL, &attr);

  if (mqid == -1)
    {
      LOG_I("mq_open failed width error: %d\n", errno);
    }
  else
    {
      mq_close(mqid);
    }
  return mqid;
}
static int get_mq_curmsgs(char *path)
{
  struct mq_attr mqStat = {0};
  mqd_t mqid = -1;

  mqid = mq_open(path, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "[video_test_h] [%s]:Error %d (%s) on mq_open.\n", __func__,
             errno, strerror(errno));
      return -1;
    }
  if (mq_getattr(mqid, &mqStat) == 0)
    {
      syslog(LOG_INFO, "[video_test_h] get %s current message:%ld\n", path,
             mqStat.mq_curmsgs);
      mq_close(mqid);
      return mqStat.mq_curmsgs;
    }
  mq_close(mqid);
  return 0;
}

int send_msg_video(video_pri *msg)
{
  int ret;
  int mqid = -1;

  if (get_mq_curmsgs(VIDEO_MQ_PATH) >= PLAYER_MQ_MSG_LEN - 4)
    {
      syslog(LOG_INFO, "[video_test_h] [%s]player mq too long, return\n", __func__);
      return 0;
    }
  mqid = mq_open(VIDEO_MQ_PATH, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "[video_test_h] Error %d (%s) on mq_open.\n", errno,
             strerror(errno));
      return -1;
    }

  ret = mq_send(mqid, (const void *)msg, sizeof(*msg), 0);
  mq_close(mqid);
  if (ret < 0)
    {
      syslog(LOG_ERR, "[video_test_h] Error %d (%s) on mq_send.\n", errno,
             strerror(errno));
      return -1;
    }
  syslog(LOG_DEBUG, "[video_test_h] Send msg to player success.\n");
  return 0;
}


int main(int argc, FAR char *argv[])
{
  video_pri msg;
  int mqid = -1;
  video_runing = 1;
  video_start_thread();

  LOG_I("mediatest player start successfully ....\n");

  create_mq(VIDEO_MQ_PATH);
  struct videotest_app *player; 

  mqid = mq_open(VIDEO_MQ_PATH, O_RDWR);
  if (mqid < 0)
    {
      LOG_E("Error %d (%s) on mq_open.\n", errno,
             strerror(errno));
    }
  usleep(200 * 1000);

  while (video_runing)
    {
      if (mq_receive(mqid, (void *)&msg, sizeof(video_pri),
                     NULL) == -1)
        {
          LOG_E("msgrcv failed width errno: %d\n", errno);
        }
      syslog(LOG_DEBUG,
             "******mq received msg!msg.cmd = %d\n", msg.ops);
      switch (msg.ops)
        {
        case VIDEO_CTRL_NEXT:
            SEND_COMMAND(play_stop_inner_video, &ctrlvideo);
            sleep(1);
            SEND_COMMAND(play_next_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_PREV:
            SEND_COMMAND(play_stop_inner_video, &ctrlvideo);
            sleep(1);
            SEND_COMMAND(play_prev_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_PLAY:
            memcpy(&ctrlvideo, &msg, sizeof(video_pri));
            SEND_COMMAND(play_cur_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_PAUSE:
            SEND_COMMAND(play_pause_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_STOP:
            SEND_COMMAND(play_stop_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_SEEK:
            ctrlvideo.pos = msg.pos;
            SEND_COMMAND(play_seek_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_RESUME:
            SEND_COMMAND(play_resume_video, &ctrlvideo);
            break;

        case VIDEO_CTRL_GET_DURATION:
            SEND_COMMAND(get_play_duration, &ctrlvideo);
            break;

        case VIDEO_CTRL_GET_POSITION:
            SEND_COMMAND(get_play_position, &ctrlvideo);
            break;

        case VIDEO_CTRL_GET_ISPLAYING:
            SEND_COMMAND(get_play_isplaying, &ctrlvideo);
            break;
        
        case VIDEO_EVENT_ONEND:
            if(msg.autoplay == true)
                SEND_COMMAND(play_next_video, &ctrlvideo);
            else
                LOG_I("the video come to end\n");
            break;

        case VIDEO_CTRL_QUIT:
            if(ctrlvideo.video)
            {
                ctrlvideo.ops = VIDEO_CTRL_STOP;
                SEND_COMMAND(play_stop_video, &ctrlvideo);
            }
            break;

        default:
          break;
        }
    }
  mq_close(mqid);
  return -1;
}
