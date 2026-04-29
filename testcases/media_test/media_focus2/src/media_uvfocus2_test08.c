#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

int media_utils_uvfocus2_test08_init(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[0].handle = media_uv_focus_request2(
      priv->loop, priv->chain[0].stream, focus2_uv_change_reply_overtime_callback, 0,
      (void *)&priv->chain[0]);
  return 0;
}

int media_utils_uvfocus2_test08_third(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[1].handle = media_uv_focus_request2(
      priv->loop, priv->chain[1].stream, focus2_uv_change_reply_callback, 0,
      (void *)&priv->chain[1]);
  return 0;
}

/*Interrupt + reply   A: hand reply + Timeout B: handle*/
void test_media_uvfocus2_08(FAR void **state)
{
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)*state;
  priv->chain[0].stream = MEDIA_STREAM_MUSIC;
  priv->chain[1].stream = MEDIA_STREAM_ALARM;
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test08_init, priv);
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test08_third, priv);

  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].ready)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }

  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[1].ready)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  priv->app_id = 1;
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[1].isabandoned)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  pthread_mutex_unlock(&priv->mutex);
  priv->chain[1].handle = NULL;
  priv->app_id = 0;
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].isabandoned)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  pthread_mutex_unlock(&priv->mutex);
  priv->chain[0].handle = NULL;
  assert_true(priv->chain[0].nb_suggests == 3);
  assert_true(priv->chain[1].nb_suggests == 1);
  assert_true(priv->chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);

  assert_true(priv->chain[1].suggests[0].suggest == MEDIA_FOCUS_PLAY);
  assert_true(priv->chain[0].suggests[1].suggest == MEDIA_FOCUS_PAUSE);
  assert_true(priv->chain[0].suggests[2].suggest == MEDIA_FOCUS_PLAY);
  assert_true(imaxabs(priv->chain[1].suggests[0].timestamp -
                      priv->chain[0].suggests[1].timestamp) > 0);
}