
#ifndef __AUDIO_POLICY_TOOL_H
#define __AUDIO_POLICY_TOOL_H


static int mediatest_play(void);
static int mediatest_quit(void);
static int get_type_of_player(char *stream);
static int mediatest_setvolume(void);
static int mediatest_stop(void);
static int mediatest_print_focus(void);
static int mediatest_help(void);
static int mediatest_isplay(void);
static int mediatest_next_song(void);
static int mediatest_prev_song(void);
static int mediatest_close(void);
static int mediatest_dump(void);
static int mediatest_setint(void);
static int mediatest_pause(void);

static int mediatest_dial(void);
static int mediatest_listen(void);
static int mediatest_answer(void);
static int mediatest_hangup(void);


#endif