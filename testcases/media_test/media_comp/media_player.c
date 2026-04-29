#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <syslog.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <system/readline.h>

#define MAX_APP_NUMBER 10

struct app_info
{
    void *handle;
    int play_ret;
    char *url;
    int focus_pre_status;
    char *stream_type;
    bool player;
    void *focus_handle;
    void *focus_cb_argv;
};

static int mediatest_play(struct app_info *app);
static int mediatest_pause(struct app_info *app);
static void play_from_ret(int play_ret, int index);
static void audio_focus_change_listener(int play_ret, void *value);
static int mediatest_close(struct app_info *app);
static int mediatest_quit(struct app_info *app);
static int mediatest_get_index_by_value(void *cb_argv);
static void mediatest_focus_cb_argv_add(void);
static int mediatest_get_focus_cb_argv(void);
static int mediatest_get_index_by_handle(void *handle);
static void mediatest_control_focus_start(int play_status, void *player_handle);
static int mediatest_print_app_status(struct app_info *app);
static int mediatest_recorder_play(struct app_info *app);
static int mediatest_help(struct app_info *app);

static char args[5][20] = {0};
static int mediatest_focus_cb_argv = 1;

static struct app_info mediatest_apps[MAX_APP_NUMBER] = {0};

typedef int (*mediatest_func)(struct app_info *app);

struct mediatest_cmd_s
{
    const char *cmd;      /* The command text */
    mediatest_func pfunc; /* Pointer to command handler */
    const char *help;     /* The help text */
};

static struct mediatest_cmd_s g_mediatest_cmds[] =
    {
        {"play",
         mediatest_play,
         "play music (play index url stream_type)"},
        {"pause",
         mediatest_pause,
         "pause music(pause index)"},
        {"close",
         mediatest_close,
         "close app"},
        {"record",
         mediatest_recorder_play,
         "start a recorder"},
        {"q",
         mediatest_quit,
         "quit mediatest"},
        {"p",
         mediatest_print_app_status,
         "display app status"},
        {"help",
         mediatest_help,
         "display help"},
        {0},
};

static char *stream_types[11] = {MEDIA_STREAM_INCALL,
                                 MEDIA_STREAM_RING, MEDIA_STREAM_ALARM, MEDIA_STREAM_SYSTEM_ENFORCED,
                                 MEDIA_STREAM_NOTIFICATION, MEDIA_STREAM_RECORD,
                                 MEDIA_STREAM_TTS, MEDIA_STREAM_ACCESSIBILITY, MEDIA_STREAM_SPORT,
                                 MEDIA_STREAM_INFO, MEDIA_STREAM_MUSIC};

static int check_stream_type(char *type)
{
    if (!strcmp(type, MEDIA_STREAM_INCALL))
        return 0;
    if (!strcmp(type, MEDIA_STREAM_RING))
        return 1;
    if (!strcmp(type, MEDIA_STREAM_ALARM))
        return 2;
    if (!strcmp(type, MEDIA_STREAM_SYSTEM_ENFORCED))
        return 3;
    if (!strcmp(type, MEDIA_STREAM_NOTIFICATION))
        return 4;
    if (!strcmp(type, MEDIA_STREAM_RECORD))
        return 5;
    if (!strcmp(type, MEDIA_STREAM_TTS))
        return 6;
    if (!strcmp(type, MEDIA_STREAM_ACCESSIBILITY))
        return 7;
    if (!strcmp(type, MEDIA_STREAM_SPORT))
        return 8;
    if (!strcmp(type, MEDIA_STREAM_INFO))
        return 9;
    if (!strcmp(type, MEDIA_STREAM_MUSIC))
        return 10;
    return -1;
}

static int mediatest_get_focus_cb_argv(void)
{
    return mediatest_focus_cb_argv;
}
static void mediatest_focus_cb_argv_add(void)
{
    if (++mediatest_focus_cb_argv == 0)
        mediatest_focus_cb_argv = 1;
}

static int mediatest_get_index_by_value(void *cb_argv)
{
    int index = -1;
    for (int i = 0; i < MAX_APP_NUMBER; i++)
    {
        if (mediatest_apps[i].focus_cb_argv == cb_argv)
        {
            index = i;
            break;
        }
    }
    return index;
}

static int mediatest_get_index_by_handle(void *handle)
{
    int index = -1;
    for (int i = 0; i < MAX_APP_NUMBER; i++)
    {
        if (handle == mediatest_apps[i].handle)
        {
            index = i;
            return index;
        }
    }

    return index;
}

static void mediatest_callback(void *cookie, int event,
                               int ret, const char *data)
{
    char *str;

    if (event == MEDIA_EVENT_STARTED)
    {
        str = "MEDIA_EVENT_STARTED";
    }
    else if (event == MEDIA_EVENT_STOPPED)
    {
        str = "MEDIA_EVENT_STOPPED";
    }
    else if (event == MEDIA_EVENT_COMPLETED)
    {
        str = "MEDIA_EVENT_COMPLETED";
    }
    else if (event == MEDIA_EVENT_PREPARED)
    {
        str = "MEDIA_EVENT_PREPARED";
    }
    else if (event == MEDIA_EVENT_PAUSED)
    {
        str = "MEDIA_EVENT_PAUSED";
    }
    else if (event == MEDIA_EVENT_SEEKED)
    {
        str = "MEDIA_EVENT_SEEKED";
    }
    else
    {
        str = "UNKNOW EVENT";
    }

    syslog(LOG_INFO, "%s, music event %s, event %d, ret %d, info %s line %d\n",
           __func__, str, event, ret, data ? data : "NULL", __LINE__);
}

static void mediatest_control_focus_start(int play_status,
                                          void *player_handle)
{
    switch (play_status)
    {
    case MEDIA_FOCUS_PLAY:
    case MEDIA_FOCUS_PLAY_WITH_DUCK:
    case MEDIA_FOCUS_PLAY_BUT_SILENT:
    {
        int ret = -1;
        if (play_status == MEDIA_FOCUS_PLAY_WITH_DUCK)
        {
            ret = media_player_set_volume(player_handle, 0.1);
        }
        else if (play_status == MEDIA_FOCUS_PLAY_BUT_SILENT)
        {
            ret = media_player_set_volume(player_handle, 0);
        }
        else
        {
            ret = media_player_set_volume(player_handle, 1);
        }
        if (ret < 0)
        {
            syslog(LOG_ERR, "Player: set_volume failed\n");
        }

        if (media_player_start(player_handle) < 0)
        {
            syslog(LOG_ERR, "Player: start failed\n");
        }
        break;
    }
    case MEDIA_FOCUS_PAUSE:
    {
        if (media_player_set_volume(player_handle, 1) < 0)
        {
            syslog(LOG_ERR, "Player: set_volume failed\n");
        }
        if (media_player_pause(player_handle) < 0)
        {
            syslog(LOG_ERR, "Player: pause failed\n");
        }
        break;
    }
    case MEDIA_FOCUS_STOP:
    {
        int index = mediatest_get_index_by_handle(player_handle);
        if (index == -1)
        {
            return;
        }
        if (mediatest_apps[index].focus_handle)
        {
            media_focus_abandon(mediatest_apps[index].focus_handle);
            mediatest_apps[index].focus_handle = NULL;
            mediatest_apps[index].focus_pre_status = 0;
            mediatest_apps[index].focus_cb_argv = 0;
        }

        if (media_player_stop(player_handle) < 0)
        {
            syslog(LOG_ERR, "Player: stop failed\n");
        }
        break;
    }
    case MEDIA_FOCUS_PLAY_WITH_KEEP:
        break;
    default:
        syslog(LOG_ERR, "unknown play status\r\n");
        break;
    }
}

static int mediatest_play(struct app_info *app)
{
    int index = atoi(args[0]);
    app->url = args[1];
    app->stream_type = args[2];
    if (!(app->url && app->stream_type))
    {
        syslog(LOG_ERR, "the args is error\n");
        return -1;
    }
    int stream_index = check_stream_type(app->stream_type);
    syslog(LOG_INFO, "the stream is %s\n", app->stream_type);
    syslog(LOG_INFO, "the url is %s\n", app->url);
    syslog(LOG_INFO, "play stream start: %s\n", app->stream_type);
    int play_status = 0;
    void *focus_handle = media_focus_request(&play_status, app->stream_type, audio_focus_change_listener, (void *)mediatest_get_focus_cb_argv());
    if (focus_handle == NULL)
    {
        syslog(LOG_ERR, "media focus req is NULL\n");
        return -1;
    }

    syslog(LOG_INFO, "status is %d(0:play,1:stop,2:pause,3:silent,4:duck,5:keep)\n", play_status);

    void *handler = media_player_open(app->stream_type);
    if (!handler)
    {
        media_focus_abandon(focus_handle);
        syslog(LOG_ERR, "player open fail, %s\n", app->stream_type);
        return -1;
    }

    /* set event callback */
    int ret = media_player_set_event_callback(handler, handler,
                                              mediatest_callback);
    if (ret < 0)
    {
        syslog(LOG_ERR, "Player: set event failed\n");
        media_player_close(handler, 0);
        media_focus_abandon(focus_handle);
        return -1;
    }

    /* prepare url or local file */
    ret = media_player_prepare(handler, app->url, NULL);
    if (ret < 0)
    {
        syslog(LOG_ERR, "Player: prepare failed\n");
        media_player_close(handler, 0);
        media_focus_abandon(focus_handle);
        return -1;
    }

    syslog(LOG_INFO, "stream index is %d\n", stream_index);
    syslog(LOG_INFO, "stream_types[stream_index] is %s\n", stream_types[stream_index]);
    mediatest_apps[index].stream_type = stream_types[stream_index];
    mediatest_apps[index].handle = handler;
    mediatest_apps[index].focus_cb_argv = (void *)mediatest_get_focus_cb_argv();
    mediatest_apps[index].focus_handle = focus_handle;
    mediatest_apps[index].focus_pre_status = play_status;
    mediatest_apps[index].player = true;
    mediatest_apps[index].url = app->url;
    if (NULL == mediatest_apps[index].url)
    {
        syslog(LOG_ERR, "## strdup error!\n");
    }

    mediatest_focus_cb_argv_add();

    mediatest_control_focus_start(play_status, handler);
    return 0;
}

static int mediatest_recorder_play(struct app_info *app)
{
    int index = atoi(args[0]);
    app->url = args[1];
    app->stream_type = args[2];
    int play_status = 0;
    void *focus_handle = media_focus_request(&play_status, app->stream_type, audio_focus_change_listener, (void *)mediatest_get_focus_cb_argv());
    if (focus_handle == NULL || (play_status != MEDIA_FOCUS_PLAY))
    {
        syslog(LOG_INFO, "Recorder: req focus failed\n");
        if (focus_handle)
        {
            media_focus_abandon(focus_handle);
        }
        return -1;
    }
    syslog(LOG_INFO, "recorder status is %d(0:play,1:stop,2:pause,3:silent,4:duck,5:keep)\n", play_status);

    void *recorder = (void *)media_recorder_open("Capture");
    if (!recorder)
    {
        syslog(LOG_ERR, "Recorder: open failed\n");
        media_focus_abandon(focus_handle);
        return -1;
    }
    int ret = media_recorder_set_event_callback(recorder, recorder, mediatest_callback);
    if (ret < 0)
    {
        syslog(LOG_INFO, "Recorder: set callback failed\n");
    }
    int stream_index = check_stream_type(app->stream_type);
    mediatest_apps[index].stream_type = stream_types[stream_index];
    mediatest_apps[index].handle = recorder;
    mediatest_apps[index].focus_cb_argv = (void *)mediatest_get_focus_cb_argv();
    mediatest_apps[index].focus_handle = focus_handle;
    mediatest_apps[index].focus_pre_status = play_status;
    mediatest_apps[index].player = false;
    mediatest_apps[index].url = app->url;

    mediatest_focus_cb_argv_add();

    syslog(LOG_INFO, "Recorder: url is %s\n", app->url);
    ret = media_recorder_prepare(recorder, app->url, "format=opus:sample_rate=16000:ch_layout=mono");
    if (ret < 0)
    {
        syslog(LOG_ERR, "Recorder: prepare failed\n");
        media_focus_abandon(focus_handle);
        return -1;
    }
    ret = media_recorder_start(recorder);
    if (ret < 0)
    {
        syslog(LOG_ERR, "Recorder: start failed\n");
        media_focus_abandon(focus_handle);
    }
    return 0;
}

static int mediatest_print_app_status(struct app_info *app)
{
    for (int i = 0; i < 10; i++)
    {
        if (mediatest_apps[i].handle)
        {
            syslog(LOG_INFO, "the media app is %d, stream is %s, handle is %p, status is %d(0:play,1:stop,2:pause,3:silent,4:duck,5:keep)\n",
                   i, mediatest_apps[i].stream_type, mediatest_apps[i].handle, mediatest_apps[i].focus_pre_status);
        }
    }
    return 0;
}

static int mediatest_pause(struct app_info *app)
{
    if (app->handle == NULL)
    {
        syslog(LOG_ERR, "app handle is NULL");
        return -1;
    }
    int index = mediatest_get_index_by_handle(app->handle);
    if (index == -1)
    {
        syslog(LOG_ERR, "mediatest app close, get index error\n");
        return -1;
    }
    syslog(LOG_INFO, "pause %s stream", app->stream_type);
    int ret;
    if (app->player)
        ret = media_player_pause(app->handle);
    else
        ret = media_recorder_pause(app->handle);
    if (mediatest_apps[index].focus_handle)
    {
        media_focus_abandon(mediatest_apps[index].focus_handle);
        mediatest_apps[index].focus_handle = NULL;
        mediatest_apps[index].focus_pre_status = 0;
        mediatest_apps[index].focus_cb_argv = 0;
    }
    return ret;
}

static int mediatest_help(struct app_info *app)
{
    for (int i = 0; g_mediatest_cmds[i].cmd; i++)
    {
        syslog(LOG_INFO, "%-16s %s\n", g_mediatest_cmds[i].cmd, g_mediatest_cmds[i].help);
    }
    return 0;
}

static int mediatest_quit(struct app_info *app)
{
    for (int i = 0; i < MAX_APP_NUMBER; ++i)
    {
        if (mediatest_apps[i].handle != 0)
        {
            mediatest_close(&mediatest_apps[i]);
        }
    }
    return 1;
}

static int mediatest_close(struct app_info *app)
{
    if (app->handle == NULL)
    {
        syslog(LOG_ERR, "app handle is NULL");
        return -1;
    }
    int ret;
    int index = mediatest_get_index_by_handle(app->handle);
    if (index == -1)
    {
        syslog(LOG_ERR, "mediatest app close, get index error\n");
        return -1;
    }
    syslog(LOG_INFO, "close %s stream", app->stream_type);
    if (app->player)
    {
        ret = media_player_close(app->handle, 0);
    }
    else
    {
        ret = media_recorder_close(app->handle);
    }
    mediatest_apps[index].handle = NULL;
    mediatest_apps[index].player = false;
    mediatest_apps[index].stream_type = NULL;
    mediatest_apps[index].url = NULL;
    if (mediatest_apps[index].focus_handle)
    {
        media_focus_abandon(mediatest_apps[index].focus_handle);
        mediatest_apps[index].focus_handle = NULL;
        mediatest_apps[index].focus_pre_status = 0;
        mediatest_apps[index].focus_cb_argv = 0;
    }
    return ret;
}

static void play_from_ret(int play_ret, int index)
{
    syslog(LOG_INFO, "stream %s, focus status "
                     "%d(0:play,1:stop,2:pause,3:silent,4:duck,5:keep)\n",
           mediatest_apps[index].stream_type, play_ret);
    int pre_status = mediatest_apps[index].focus_pre_status;
    switch (play_ret)
    {
        case MEDIA_FOCUS_PLAY:
            syslog(LOG_INFO, "%s app audio change with play normal\n", mediatest_apps[index].stream_type);
            if (!mediatest_apps[index].player)
            {
                media_recorder_start(mediatest_apps[index].handle);
                mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PLAY;
                return;
            }
            if (pre_status != MEDIA_FOCUS_PLAY)
            {
                if (pre_status == MEDIA_FOCUS_PLAY_BUT_SILENT || pre_status == MEDIA_FOCUS_PLAY_WITH_DUCK)
                {
                    media_player_set_volume(mediatest_apps[index].handle, 1);
                    mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PLAY;
                    syslog(LOG_INFO, "just set volume\n");
                    return;
                }

                if (pre_status == MEDIA_FOCUS_PAUSE)
                {
                    syslog(LOG_INFO, "no need prepare, start\n");
                    media_player_start(mediatest_apps[index].handle);
                }
                else
                {
                    syslog(LOG_INFO, "need prepare and start\n");
                    media_player_prepare(mediatest_apps[index].handle,
                                        mediatest_apps[index].url, NULL);
                    media_player_start(mediatest_apps[index].handle);
                }

                mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PLAY;
            }
            break;

        case MEDIA_FOCUS_STOP:
            syslog(LOG_INFO, "%s, app audio change with stop\n", mediatest_apps[index].stream_type);
            if (pre_status != MEDIA_FOCUS_STOP)
            {
                // if (mediatest_apps[index].focus_handle)
                // {
                //     syslog(LOG_INFO, "abundon focus is %p\n", mediatest_apps[index].focus_handle);
                //     media_focus_abandon(mediatest_apps[index].focus_handle);
                //     mediatest_apps[index].focus_handle = NULL;
                //     mediatest_apps[index].focus_pre_status = 0;
                //     mediatest_apps[index].focus_cb_argv = 0;
                // }
                if (!mediatest_apps[index].player)
                {
                    syslog(LOG_INFO, "the record is stop\n");
                    media_recorder_stop(mediatest_apps[index].handle);
                    return;
                }

                syslog(LOG_INFO, "player stop\n");
                media_player_stop(mediatest_apps[index].handle);
            }
            break;

        case MEDIA_FOCUS_PAUSE:
            syslog(LOG_INFO, "%s, app audio change with pasue\n", mediatest_apps[index].stream_type);
            if (pre_status != MEDIA_FOCUS_PAUSE)
            {
                mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PAUSE;
                syslog(LOG_INFO, "player pause\n");
                int ret = media_player_pause(mediatest_apps[index].handle);
                if (ret < 0)
                {
                    syslog(LOG_INFO, "the pause handle is %p, index is %d\n", mediatest_apps[index].handle, index);
                    syslog(LOG_ERR, "the ret is %d\n", ret);
                    syslog(LOG_ERR, "media player pause failed\n");
                }
            }
            break;

        case MEDIA_FOCUS_PLAY_BUT_SILENT:
            syslog(LOG_INFO, "%s app audio change with play silent\n", mediatest_apps[index].stream_type);
            if (pre_status != MEDIA_FOCUS_PLAY_BUT_SILENT)
            {
                mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PLAY_BUT_SILENT;
                syslog(LOG_INFO, "set volume to 0\n");
                media_player_set_volume(mediatest_apps[index].handle, 0);
            }
            break;

        case MEDIA_FOCUS_PLAY_WITH_DUCK:
            syslog(LOG_INFO, "%s app audio change with play ducked\n", mediatest_apps[index].stream_type);
            if (pre_status != MEDIA_FOCUS_PLAY_WITH_DUCK)
            {
                syslog(LOG_INFO, "set volume to 0.1\n");
                media_player_set_volume(mediatest_apps[index].handle, 0.1);

                if (pre_status != MEDIA_FOCUS_PLAY)
                {
                    if (pre_status == MEDIA_FOCUS_STOP)
                    {
                        media_player_prepare(mediatest_apps[index].handle,
                                            mediatest_apps[index].url, NULL);
                    }
                    media_player_start(mediatest_apps[index].handle);
                }

                mediatest_apps[index].focus_pre_status = MEDIA_FOCUS_PLAY_WITH_DUCK;
            }
            break;

        case MEDIA_FOCUS_PLAY_WITH_KEEP:
            break;

        default:
            syslog(LOG_ERR, "unknown play_status\n");
            break;
    }
}

// callback method
static void audio_focus_change_listener(int play_ret, void *value)
{
    int index = mediatest_get_index_by_value(value);
    syslog(LOG_INFO, "the index is %d\n", index);
    play_from_ret(play_ret, index);
}

static int deal_data(char *arg)
{
    int i = 0;
    char *token = strtok(arg, " ");
    while (token != NULL)
    {
        strcpy(args[i++], token);
        syslog(LOG_INFO, "token is %s\n", token);
        token = strtok(NULL, " ");
    }

    return 0;
}

static int mediatest_execute(char *cmd, char *arg)
{
    int ret = 0;
    int x;
    int app_index;

    /* Find the command in our cmd array */
    ret = deal_data(arg);
    if (ret < 0)
    {
        return -1;
    }
    app_index = atoi(args[0]);
    if (app_index < 0 || app_index > MAX_APP_NUMBER)
    {
        syslog(LOG_ERR, "the app index is error\n");
    }

    for (x = 0; g_mediatest_cmds[x].cmd; x++)
    {
        if (strcmp(cmd, g_mediatest_cmds[x].cmd) == 0)
        {
            ret = g_mediatest_cmds[x].pfunc(&mediatest_apps[app_index]);
            if (ret < 0)
                syslog(LOG_ERR, "cmd %s error %d\n", cmd, ret);

            if (g_mediatest_cmds[x].pfunc == mediatest_quit)
                ret = 1;

            break;
        }
    }

    if (x == sizeof(g_mediatest_cmds) / sizeof(g_mediatest_cmds[0]))
        syslog(LOG_ERR, "Unknown cmd: %s\n", cmd);

    return ret;
}

int main(int argc, char *argv[])
{
    char *cmd, *arg;
    char *buffer;
    int ret, len;

    buffer = malloc(CONFIG_NSH_LINELEN);
    if (!buffer)
        return -1;

    while (1)
    {
        printf("mediatest> ");
        fflush(stdout);

        len = readline_stream(buffer, CONFIG_NSH_LINELEN, stdin, stdout);
        buffer[len] = '\0';
        if (len < 0)
            continue;

        if (buffer[0] == '!')
        {
#ifdef CONFIG_SYSTEM_SYSTEM
            system(buffer + 1);
#endif
            continue;
        }

        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        cmd = strtok_r(buffer, " \n", &arg);
        if (cmd == NULL)
            continue;

        while (*arg == ' ')
            arg++;

        syslog(LOG_INFO, "the cmd is %s\n", cmd);
        ret = mediatest_execute(cmd, arg);
        if (ret > 0)
            break;
    }

    free(buffer);
    return 0;
}
