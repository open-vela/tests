#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/*focus2 test
 */

static int media_utils_uvfocus2_test11_init(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[0].handle = media_uv_focus_request2(
      priv->loop, priv->chain[0].stream, focus2_uv_change_callback, 1,
      (void *)&priv->chain[0]);
  return 0;
}

static int media_utils_uvfocus2_test11_init_s(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[1].handle = media_uv_focus_request2(
      priv->loop, priv->chain[1].stream, focus2_uv_change_callback, 1,
      (void *)&priv->chain[1]);
  return 0;
}

void test_media_uvfocus2_11(FAR void **state)
{
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)*state;
  priv->chain[0].stream = MEDIA_STREAM_MUSIC;
  priv->chain[1].stream = MEDIA_STREAM_MUSIC;

  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test11_init, priv);

  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].ready)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  priv->app_id = 0;
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test11_init_s, priv);
  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].isabandoned)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  pthread_mutex_unlock(&priv->mutex);

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

  assert_non_null(priv->chain[0].handle);
  priv->chain[0].handle = NULL;
  assert_non_null(priv->chain[1].handle);
  priv->chain[1].handle = NULL;
  assert_true(priv->chain[0].ret >= 0);
  assert_true(priv->chain[0].nb_suggests == 1);
  assert_true(priv->chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);
  assert_true(priv->chain[1].ret >= 0);
  assert_true(priv->chain[1].nb_suggests == 1);
  assert_true(priv->chain[1].suggests[0].suggest == MEDIA_FOCUS_PLAY);
}