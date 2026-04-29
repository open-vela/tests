#ifndef __INCLUDE_AUDIO_FOCUS_H
#define __INCLUDE_AUDIO_FOCUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <assert.h>
#include <string.h>
#include <nuttx/list.h>
#include <nuttx/mutex.h>
#include <pthread.h>

// Audio focus stack
#define AUDIO_FOCUS_STACK_CAP   10

// Audio stream type && audio focus level
#define AUDIO_STREAM_NOTIFY 		0 		// "TONE" 
#define AUDIO_STREAM_TTS            1    	// "TTS"  
#define AUDIO_STREAM_MUSIC          2		// "MUSIC"
#define AUDIO_STREAM_ALARM          3		// "ALARM"
#define AUDIO_STREAM_SCO            4
#define AUDIO_STREAM_RING           5
#define AUDIO_STREAM_ENFORCED       6
#define AUDIO_STREAM_RECORD         7
#define AUDIO_STREAM_HEALTH         8
#define AUDIO_STREAM_SPORT          9
#define AUDIO_STREAM_INFO           10


// Audio focus request play result suggestion,
#define AUDIO_FOCUS_PLAY                0   // media play
#define AUDIO_FOCUS_STOP                1   // media stop
#define AUDIO_FOCUS_PAUSE               2   // media pause
#define AUDIO_FOCUS_PLAY_BUT_SILENT     3   // media play but silent in background
#define AUDIO_FOCUS_PLAY_WITH_DUCK      4   // media play in background, duck volumn down
#define AUDIO_FOCUS_PLAY_WITH_KEEP      5   // media play keep current status in background

/****************************************************************************
 * Public Types
 ****************************************************************************/
typedef void (*audio_focus_callback)(int return_type, void* callback_argv);

/****************************************************************************
 * Public functions
 ****************************************************************************/
void test_audio_focus_init(void);
void* test_audio_focus_request(int* return_type, int stream_type, audio_focus_callback callback_method, void* callback_argv);
int test_audio_focus_abandon(void* handle);
void test_audio_focus_debug_stack_display(void);
void *test_get_af_stack_top(void);
int test_get_af_stack_used(void);

#endif
