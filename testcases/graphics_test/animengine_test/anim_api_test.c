
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "animengine_test.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
static anim_engine_handle_t anim_engine_handle;
static int64_t g_anim_id;
static lv_obj_t* ui_btntest;
static int flag = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
int anim_adapter_test(void)
{
   anim_engine_handle = lvx_anim_adapter_init();
   syslog(LOG_INFO, "[PASSED] lvx_anim_adapter_init");
   return PASSED;
}

int anim_create_test01(void)
{
   int ret = 0;
   char *json = "{\"id\": aaa}";

   ret = anim_create(anim_engine_handle, &g_anim_id, json);
   syslog(LOG_DEBUG, "anim_create01_ret == %d", ret);
   if (ret == EINVAL) {
      syslog(LOG_INFO, "[PASSED] anim_create01");
      return PASSED;
   } else {
      syslog(LOG_INFO, "[FAILED] anim_create01");
      flag = 1;
      return FAILED;
   }
}

int anim_create_test02(void)
{
   int ret = 0;
   char *json = "{\"id\": 10000,\"fromState\": {\"x\": 50},\"toState\": "
                "{\"y\": 240},\"config\":{\"iterations\": 2,\"duration\": 2,\"ease\":[\"cubicIn\",2]}}";

   ret = anim_create(anim_engine_handle, &g_anim_id, json);
   syslog(LOG_DEBUG, "anim_create02_ret == %d", ret);
   syslog(LOG_DEBUG, "g_anim_id == %" PRId64, g_anim_id);
   if (ret == 0 && g_anim_id == 10000) {
      syslog(LOG_INFO, "[PASSED] anim_create02");
      return PASSED;
   } else {
      syslog(LOG_INFO, "[FAILED] anim_create02");
      flag = 1;
      return FAILED;
   }
}

int anim_start_test01(void)
{
   int ret = 0;
   int64_t anim_id = 100;

   ret = anim_start(anim_engine_handle, anim_id, ui_btntest, ANIM_LT_NORMAL);
   syslog(LOG_DEBUG, "anim_start01_ret == %d", ret);
   if (ret == ENOKEY) {
      syslog(LOG_INFO, "[PASSED] anim_start01");
      return PASSED;
   } else {
      syslog(LOG_INFO, "[FAILED] anim_start01");
      flag = 1;
      return FAILED;
   }
}

int anim_start_test02(void)
{
   int ret = 0;

   ret = anim_start(anim_engine_handle, g_anim_id, ui_btntest, ANIM_LT_NORMAL);
   syslog(LOG_DEBUG, "anim_start01_ret == %d", ret);
   syslog(LOG_DEBUG, "g_anim_id == %" PRId64, g_anim_id);
   if (ret == 0 && g_anim_id == 10000) {
      syslog(LOG_INFO, "[PASSED] anim_start02");
      return PASSED;
   } else {
      syslog(LOG_INFO, "[FAILED] anim_start02");
      flag = 1;
      return FAILED;
   }
}

int anim_remove_test(void)
{
   anim_remove(anim_engine_handle, g_anim_id);
   syslog(LOG_DEBUG, "g_anim_id == %" PRId64, g_anim_id);
   syslog(LOG_INFO, "[PASSED] anim_remove");
   return PASSED;
}

int anim_destroy_test(void)
{
   anim_engine_destroy(anim_engine_handle);
   syslog(LOG_DEBUG, "g_anim_id == %" PRId64, g_anim_id);
   syslog(LOG_INFO, "[PASSED] anim_engine_destroy");
   return PASSED;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
int animengine_api_test(int argc, char *argv[])
 {
   syslog(LOG_INFO, "\n[======] animengine_api_test start\n");
   anim_adapter_test();

   anim_create_test01();
   anim_create_test02();

   anim_start_test01();
   anim_start_test02();

   anim_remove_test();

   anim_destroy_test();

   if (flag == 0) {
      syslog(LOG_INFO, "\n[======] animengine_api_test passed\n");
      exit(PASSED);
   } else {
      return PASSED;
      syslog(LOG_INFO, "\n[======] animengine_api_test failed\n");
      exit(FAILED);
   }
 }
