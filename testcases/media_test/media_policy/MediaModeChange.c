/****************************************************************************
 * apps/tests/testcases/media_test/media_policy/MediaModeChange.c
 *
 * Name: MediaModeChange
 * Example description:
 *  1. MediaModeChange
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "MediaPolicyTest.h"
#include "media_graph_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  char url[24] = "/stream/1.mp3";
  char volume_type[20] = "MusicVolume";
  char url2[24] = "/stream/2.mp3";
  char volume_type2[20] = "RingVolume";
  char url3[24] = "/stream/3.mp3";
  const char *mode_names[] = {"AudioMode"};
  const char *mode_values[] = {"normal"};
  const char *mode_names2[] = {"AvailableDevices"};
  const char *mode_values2[] = {"a2dp"};
  const char *alarm[] = {"Alarm"};
  const char *alarm_value[] = {"0"};
  struct mediatest_data *media[3];

  if (argc < 6)
    return 0;

  sprintf(url, "%s", argv[1]);
  sprintf(url2, "%s", argv[2]);
  sprintf(url3, "%s", argv[3]);
  mode_names[0] = argv[4];
  mode_values[0] = argv[5];

  for (int i = 0; i < 3; i++)
    media[i] =
        (struct mediatest_data *)malloc(sizeof(struct mediatest_data));

  // setup
  // loop 1 Music
  media[0]->stream_type = "Music";
  FUN_CHECK(mediatest_player_open(media[0]), media[0],
            mediatest_common_close, 0, "FAIL! media open fail.\n");
  FUN_CHECK(media_policy_set_int(volume_type, 4, 100), media[0],
            mediatest_common_close, 1,
            "FAIL! media set MusicVolume fail.\n");
  media[0]->loop = 1;
  FUN_CHECK(mediatest_player_loop(media[0]), media[0],
            mediatest_common_close, 1, "FAIL! media set loop fail.\n");
  media[0]->url = url;
  FUN_CHECK(mediatest_common_prepare(media[0]), media[0],
            mediatest_common_close, 1, "FAIL! media prepare %s fail\n",
            url);
  STATE_CHECK(PLAYER_PREPARED, media[0], mediatest_common_close,
              "player: prepare event return failed\n");
  mediatest_common_start(media[0]);
  STATE_CHECK(PLAYER_STARTED, media[0], mediatest_common_close,
              "player: start event return failed\n");

  // loop 2 Ring
  media[0]->stream_type = "Ring";
  FUN_CHECK(mediatest_player_open(media[1]), media[1],
            mediatest_common_close, 0, "FAIL! media open fail.\n");
  FUN_CHECK(media_policy_set_int(volume_type2, 4, 100), media[1],
            mediatest_common_close, 1,
            "FAIL! media set MusicVolume fail.\n");
  media[1]->loop = 1;
  FUN_CHECK(mediatest_player_loop(media[1]), media[1],
            mediatest_common_close, 1, "FAIL! media set loop fail.\n");
  media[1]->url = url2;
  FUN_CHECK(mediatest_common_prepare(media[1]), media[1],
            mediatest_common_close, 1, "FAIL! media prepare %s fail\n",
            url2);
  STATE_CHECK(PLAYER_STARTED, media[1], mediatest_common_close,
              "player: start event return failed\n");
  mediatest_common_start(media[1]);
  STATE_CHECK(PLAYER_STARTED, media[1], mediatest_common_close,
              "player: start event return failed\n");

  /*
      //loop 3 Alarm
      FUN_CHECK(mediatest_player_open(media[2], "Alarm"), media[2],
     mediatest_common_close, 0, "FAIL! media open fail.\n");
      FUN_CHECK(media_policy_set_int(volume_type3, 4, 100), media[2],
     mediatest_common_close, 1, "FAIL! media set MusicVolume fail.\n");
      FUN_CHECK(mediatest_player_loop(media[2], looptime), media[2],
     mediatest_common_close, 1, "FAIL! media set loop fail.\n");
      mediatest_common_prepare(media[2], 1, url3, NULL);
      RET_CHECK(media[2]->ret, media[2], mediatest_common_close, "FAIL!
     media prepare %s fail\n", url); mediatest_common_start(media[2]);
  */

  sleep(5);
  // set AudioMode to normal
  syslog(LOG_INFO, "name %s, value %s\n", mode_names[0], mode_values[0]);
  if (!strcmp(mode_names[0], "AudioMode") &&
      !strcmp(mode_values[0], "normal"))
    {
      syslog(LOG_INFO, "name %s, value %s\n", mode_names[0],
             mode_values[0]);
      if (0 != test_criterion(mode_names[0], mode_values, 1, 1))
        goto error_out;
      if (0 != test_criterion(alarm[0], alarm_value, 1, 0))
        goto error_out;
      if (0 != media_policy_exclude(mode_names2[0], mode_values2[0], 1))
        goto error_out;
    }

  // set to phone
  if (!strcmp(mode_names[0], "AudioMode") &&
      !strcmp(mode_values[0], "phone"))
    {
      syslog(LOG_INFO, "name %s, value %s\n", mode_names[0],
             mode_values[0]);
      if (0 != test_criterion(mode_names[0], mode_values, 1, 1))
        goto error_out;
    }

  // set to both
  if (!strcmp(mode_names[0], "AudioMode") &&
      !strcmp(mode_values[0], "both"))
    {
      syslog(LOG_INFO, "name %s, value %s\n", mode_names[0],
             mode_values[0]);
      mode_values[0] = "normal";
      // strcpy(mode_values[0], "normal");
      if (0 != test_criterion(mode_names[0], mode_values, 1, 1))
        goto error_out;
      if (0 != media_policy_include(mode_names2[0], mode_values2[0], 1))
        goto error_out;
      if (0 != test_criterion(mode_names2[0], mode_values2, 1, 1))
        goto error_out;
      if (0 != media_policy_increase(alarm[0], 100))
        goto error_out;
    }

  // set to a2dp

  if (!strcmp(mode_names[0], "AudioMode") &&
      !strcmp(mode_values[0], "a2dp"))
    {
      syslog(LOG_INFO, "name %s, value %s\n", mode_names[0],
             mode_values[0]);
      mode_values[0] = "normal";

      // strcpy(mode_values[0], "normal");

      if (0 != test_criterion(mode_names[0], mode_values, 1, 1))
        goto error_out;
      if (0 != test_criterion(mode_names2[0], mode_values2, 1, 1))
        goto error_out;
      if (0 != test_criterion(alarm[0], alarm_value, 1, 0))
        goto error_out;
    }

  // set to default

  if (!strcmp(mode_names[0], "AudioMode") &&
      !strcmp(mode_values[0], "default"))
    {
      syslog(LOG_INFO, "name %s, value %s\n", mode_names[0],
             mode_values[0]);
      if (0 != test_criterion(mode_names[0], mode_values, 1, 1))
        goto error_out;
    }

  sleep(10);
  FUN_CHECK(mediatest_common_stop(media[0]), media[0],
            mediatest_common_close, 1, "FAIL ! media stop fail\n");
  FUN_CHECK(mediatest_common_stop(media[1]), media[1],
            mediatest_common_close, 1, "FAIL ! media stop fail\n");
  FUN_CHECK(mediatest_common_close(media[0]), media[0],
            mediatest_common_close, 0, "FAIL! media close fail.\n");
  FUN_CHECK(mediatest_common_close(media[1]), media[1],
            mediatest_common_close, 0, "FAIL! media close fail.\n");

  /*
      FUN_CHECK(mediatest_common_close(media[1], 0), media[2],
     mediatest_common_close, 0, "FAIL! media close fail.\n");
  */

  syslog(LOG_INFO, "test success\n");
  return 0;

error_out:
  FUN_CHECK(mediatest_common_stop(media[0]), media[0],
            mediatest_common_close, 1, "FAIL ! media stop fail\n");
  FUN_CHECK(mediatest_common_stop(media[1]), media[1],
            mediatest_common_close, 1, "FAIL ! media stop fail\n");
  FUN_CHECK(mediatest_common_close(media[0]), media[0],
            mediatest_common_close, 0, "FAIL! media close fail.\n");
  FUN_CHECK(mediatest_common_close(media[1]), media[1],
            mediatest_common_close, 0, "FAIL! media close fail.\n");
  /*
      FUN_CHECK(mediatest_common_close(media[1], 0), media[2],
     mediatest_common_close, 0, "FAIL! media close fail.\n");
  */
  syslog(LOG_INFO, "test fail\n");
  return 1;
}
