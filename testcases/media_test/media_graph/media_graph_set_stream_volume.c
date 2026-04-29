/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_set_stream_volume.c
 *
 * Name: media_graph_set_stream_volume
 * Example description:
 *  1. media_graph_set_stream_volume
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "media_graph_test.h"
#include <media_api.h>
#include <nuttx/config.h>
#include <stdlib.h>
#include <syslog.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  char stream_volume[28];
  struct mediatest_data *media;
  int open = 0;
  int value;
  media = (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  if (!media)
    {
      syslog(LOG_ERR, "media data malloc failed\n");
      goto out;
    }

  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  mediatest_getopt(argc, argv, media);

  RET_CHECK(mediatest_common_open(media), "FAIL! media open fail.\n");
  open = 1;
  RET_CHECK(mediatest_common_prepare(media),
            "FAIL! media prepare fail\n");
  ST_CHECK(PLAYER_PREPARED, media,
           "player: prepare event return failed\n");
  RET_CHECK(mediatest_common_start(media), "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");

  sleep(3);
  sprintf(stream_volume, "%s%s", media->stream_type, "Volume");
  syslog(LOG_INFO, "the type is %s\n", media->stream_type);
  syslog(LOG_INFO, "the voluem is %s\n", stream_volume);
  if (media_policy_set_int(stream_volume, media->volume, 100) < 0)
    {
      printf("stream_type %s\n", stream_volume);
      syslog(LOG_ERR, "media_policy_set_int failed\n");
      goto out;
    }

  media_policy_get_int(stream_volume, &value);
  if (value != media->volume)
    {
      syslog(LOG_ERR, "media_policy_get_int value not equal set\n");
      goto out;
    }

  syslog(LOG_INFO, "PASS ! media_graph_set_stream_volume pass \n");

out:
  if (open)
    {
      mediatest_common_close(media);
    }

  free(media);
  media = NULL;

  return 0;
}
