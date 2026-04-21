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

int media_utils_uvfocus2_test12_init(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[0].handle = media_uv_focus_request2(
      priv->loop, priv->chain[0].stream, focus2_uv_multi_reply_callback, 0,
      (void *)&priv->chain[0]);
  return 0;
}

void test_media_uvfocus2_12(FAR void **state)
{
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)*state;
  priv->chain[0].stream = MEDIA_STREAM_MUSIC;

  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test12_init, priv);

  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].ready)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  priv->app_id = 0;
  UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
  pthread_mutex_lock(&priv->mutex);
  while (!priv->chain[0].isabandoned)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  pthread_mutex_unlock(&priv->mutex);
  assert_non_null(priv->chain[0].handle);
  priv->chain[0].handle = NULL;
  assert_true(priv->chain[0].ret >= 0);
  assert_true(priv->chain[0].nb_suggests == 1);
  assert_true(priv->chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);
}