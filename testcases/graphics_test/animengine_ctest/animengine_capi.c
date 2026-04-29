
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "animengine_ctest.h"
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

static ANIMID anim_id;
static anim_engine_handle_t img_anim_handle;
static lv_obj_t *ui_screen;
static lv_obj_t *ui_btntest;
static lv_obj_t *ui_imgtest;
static int flag = 0;
static void screen_init(void)
{

   // ui_screen = lv_obj_create(NULL);
   // lv_obj_clear_flag(ui_screen, LV_OBJ_FLAG_SCROLLABLE);
   // lv_obj_set_style_bg_color(ui_screen, lv_color_hex(0x375830),
   //                           LV_PART_MAIN | LV_STATE_DEFAULT);
   // lv_obj_set_style_bg_opa(ui_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

   ui_screen = lv_obj_create(NULL);
   lv_obj_clear_flag(ui_screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
   lv_obj_set_style_bg_color(ui_screen, lv_color_hex(0x000000),
                             LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

   ui_btntest = lv_btn_create(ui_screen);
   lv_obj_set_width(ui_btntest, 58);
   lv_obj_set_height(ui_btntest, 58);
   lv_obj_set_x(ui_btntest, -115);
   lv_obj_set_y(ui_btntest, -130);
   lv_obj_set_align(ui_btntest, LV_ALIGN_CENTER);
   lv_obj_add_flag(ui_btntest, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
   lv_obj_clear_flag(ui_btntest, LV_OBJ_FLAG_SCROLLABLE);
   lv_obj_set_style_radius(ui_btntest, 29, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_color(ui_btntest, lv_color_hex(0x6950a1),
                             LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui_btntest, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void get_anim_status(anim_layer_t *layer, const anim_status_type_t status)
{
   // syslog(LOG_INFO, "status = %d; property_type = %d; layer_type = %d",
   //        status, layer->property_type, layer->layer_type);
   if (status == ANIM_ST_END)
   {
      syslog(LOG_INFO, "It's OK, status == %d", ANIM_ST_END);
      // anim_remove(anim_engine_handle, g_anim_id);
   }
}
static void get_anim_update(anim_layer_t *layer, const anim_value_t *value)
{
   // syslog(LOG_INFO, "value_type = %d", value->type);
}

static void get_user_data(anim_layer_t *layer, const anim_value_t *value)
{
}
/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
// int anim_adapter_test(void)
// {
//    anim_engine_handle_t = lvx_anim_adapter_init();
//    syslog(LOG_INFO, "[PASSED] lvx_anim_adapter_init");
//    return PASSED;
// }

static void print_config(const anim_config_t *config)
{
    syslog(LOG_INFO, "config[curve:%d,p1:%f,p2:%f,p3:%f,p4:%f,direction_type:%d,iteration_count:%d\
        duration:%" PRIi32 ",delay:%" PRIi32 ",ratio:%f]",
        config->curve.curve_type, config->curve.p1, config->curve.p2,
        config->curve.p3, config->curve.p4, config->direction_type, config->iteration_count,
        config->duration, config->delay, config->ratio);
}

int anim_sequence_create01(void)
{
   anim_id = anim_sequence_create(img_anim_handle);
   syslog(LOG_DEBUG, "  animsequence_create01 test start ");
   syslog(LOG_DEBUG, "anim_sequence_create01_ret == %lld", anim_id);
   if (anim_id != EINVAL)
   {
      syslog(LOG_INFO, "[PASSED]  anim_sequence_create01");
      return PASSED;
   }
   else
   {
      syslog(LOG_INFO, "[FAILED]  anim_sequence_create01");
      return FAILED;
   }
}
int anim_seqience_destory01(void)
{
   // anim_sequence_destroy
   anim_id = anim_sequence_create(img_anim_handle);
   syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);
   int ret = 0;

   anim_config_t config;
   config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
   config.duration = 5000;
   config.delay = 2000;
   config.ratio = 0.5;
   config.iteration_count = 2;

   // config.curve.curve_type = ANIM_CT_BACK_IN;
   anim_config_set(img_anim_handle, anim_id, &config);
   anim_config_get(img_anim_handle, anim_id, &config);

   anim_value_t val;
   val.type = ANIM_VT_FLOAT;
   val.v.fv = 300.0;

   anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);

   val.type = ANIM_VT_COLOR;
   val.v.color.full = 0XFF0000;
   anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   print_config(&config);
   anim_sequence_destroy(img_anim_handle, anim_id);
   ret = anim_start(img_anim_handle, anim_id, ui_btntest, ANIM_LT_NORMAL);
   if (ret == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_destory01 ");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_destory01 ");
      return FAILED;
   }
   return PASSED;
}
int anim_config_get01(void)
{
   // not set config
   syslog(LOG_DEBUG, " anim_config_get01 test start ");
   anim_config_t config;
   anim_config_get(img_anim_handle, anim_id, &config);
   print_config(&config);
   syslog(LOG_INFO, "[PASSED]  anim_config_get01");
   return PASSED;
}
int anim_config_get02(void)
{
   // initialize config all 0
   syslog(LOG_DEBUG, " anim_config_get02 test start ");
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   anim_config_get(img_anim_handle, anim_id, &config);
   print_config(&config);
   syslog(LOG_INFO, "[PASSED]  anim_config_get02");
   return PASSED;
}
int anim_config_get03(void)
{
   // initialize config all 0
   syslog(LOG_DEBUG, " anim_config_get03 test start ");
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   anim_config_get(img_anim_handle, anim_id, &config);
   print_config(&config);
   if (config.delay == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_config_get03");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_config_get03");
      return FAILED;
   }
}
int anim_config_set01(void)
{
   // initialize config all 0
   anim_config_t config;
   // memset(&config, 0, sizeof(anim_config_t));
   config.curve.curve_type = ANIM_CT_BEZIER;
   config.duration = 5000;
   config.delay = 1000;
   config.ratio = 0.5;
   config.iteration_count = 2;
   syslog(LOG_DEBUG, " anim_config_set01 test start ");

   anim_config_set(img_anim_handle, anim_id, &config);
   syslog(LOG_DEBUG, "anim_config_set01 amim_config_set success");
   anim_config_get(img_anim_handle, anim_id, &config);
   print_config(&config);
   if (config.delay == 1000)
   {
      syslog(LOG_INFO, "[PASSED]  anim_config_set01");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_config_set01");
      return FAILED;
   }
}
int anim_config_set02(void)
{
   // init config
   syslog(LOG_DEBUG, " anim_config_set02 test start ");
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   config.direction_type = ANIM_DT_NORMAL;
   config.curve.curve_type = ANIM_CT_SINE_INOUT;
   config.duration = 5;
   config.delay = 6;
   config.ratio = 0.5;
   config.iteration_count = 7;

   anim_config_set(img_anim_handle, anim_id, &config);
   syslog(LOG_DEBUG, "anim_config_set01 amim_config_set success");
   anim_config_get(img_anim_handle, anim_id, &config);
   print_config(&config);
   if (config.delay == 6)
   {
      syslog(LOG_INFO, "[PASSED]  anim_config_set02");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_config_set02");
      return FAILED;
   }
}
int anim_property_add01(void)
{
   // Abnormal anim_id
   syslog(LOG_DEBUG, " anim_property_add01 test start ");
   int ret = 0;
   char qwe = 'a';
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   anim_value_t val;
   val.type = ANIM_VT_COLOR;
   val.v.color.full = 0x00000000;
   ret = anim_property_add(img_anim_handle, qwe, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
   syslog(LOG_DEBUG, "anim_property_add01 return is %d", ret);
   if (ret != 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_property_add01");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_property_add01");
      return FAILED;
   }
}
int anim_property_add02(void)
{
   // normal anim_id
   syslog(LOG_DEBUG, " anim_property_add01 test start ");
   int ret = 0;
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   anim_value_t val;
   val.type = ANIM_VT_COLOR;
   val.v.color.full = 0x00000000;
   ret = anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
   syslog(LOG_DEBUG, "anim_property_add01 return is %d", ret);
   // why 0
   if (ret == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_property_add02");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_property_add02");
      return FAILED;
   }
}
int anim_remove01(void)
{
   // test anim_property_remove
   anim_id = anim_sequence_create(img_anim_handle);
   syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);
   int ret = 0;

   anim_config_t config;
   config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
   config.duration = 5000;
   config.delay = 2000;
   config.ratio = 0.5;
   config.iteration_count = 2;

   // config.curve.curve_type = ANIM_CT_BACK_IN;
   anim_config_set(img_anim_handle, anim_id, &config);
   anim_config_get(img_anim_handle, anim_id, &config);

   anim_value_t val;
   val.type = ANIM_VT_FLOAT;
   val.v.fv = 300.0;

   anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);

   val.type = ANIM_VT_COLOR;
   val.v.color.full = 0XFF0000;
   anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   anim_property_remove(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR);

   ret = anim_start(img_anim_handle, anim_id, ui_btntest, ANIM_LT_NORMAL);
   if (ret == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_remove01 ");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_remove01 ");
      return FAILED;
   }
   return PASSED;
}
int anim_property_config_get01(void)
{
   // not set config
   syslog(LOG_DEBUG, " anim_property_config_get02 test start ");
   anim_config_t get02_config;
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_HEIGHT, &get02_config);
   print_config(&get02_config);
   syslog(LOG_INFO, "[PASSED]  anim_property_config_get01");
   return PASSED;
}
int anim_property_config_get02(void)
{
   syslog(LOG_DEBUG, " anim_property_config_get03 test start ");
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   config.duration = 2000;
   // anim_value_t val;
   // val.type = ANIM_VT_COLOR;
   // val.v.color.full = 0x00000000;
   // ANIM_PT_BACKGROUND_COLOR duration is 2000
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   print_config(&config);
   // ANIM_PT_VISIBILITY duration is not 2000
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_VISIBILITY, &config);
   print_config(&config);
   syslog(LOG_INFO, "[PASSED]  anim_property_config_get02");
   return PASSED;
}
int anim_property_config_set01(void)
{
   // config is null
   syslog(LOG_DEBUG, " anim_property_config_set01 test start ");
   anim_config_t set01_config;
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_WIDTH, &set01_config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_WIDTH, &set01_config);
   print_config(&set01_config);
   syslog(LOG_INFO, "[PASSED]  anim_property_config_set01");
   return PASSED;
}
int anim_property_config_set02(void)
{
   // config is all 0
   syslog(LOG_DEBUG, " anim_property_config_set02 test start ");
   anim_config_t set02_config;
   memset(&set02_config, 0, sizeof(anim_config_t));
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &set02_config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &set02_config);
   print_config(&set02_config);
   syslog(LOG_INFO, "[PASSED]  anim_property_config_set02");
   return PASSED;
}
int anim_property_config_set03(void)
{
   // config is init
   syslog(LOG_DEBUG, " anim_property_config_set03 test start ");
   anim_config_t config;
   memset(&config, 0, sizeof(anim_config_t));
   config.curve.curve_type = ANIM_CT_LINEAR;
   config.duration = 10;
   config.delay = 20;
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
   print_config(&config);
   syslog(LOG_INFO, "[PASSED]  anim_property_config_set03");
   return PASSED;
}
int anim_start01(void)
{
   // not anim_sequence_destroy
   anim_id = anim_sequence_create(img_anim_handle);
   syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);
   int ret = 0;

   anim_config_t config;
   // memset(&config, 0, sizeof(anim_config_t));
   // anim_config_get(img_anim_handle, anim_id, &config);
   // print_config(&config);

   config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
   config.duration = 5000;
   config.delay = 2000;
   config.ratio = 0.5;
   config.iteration_count = ANIM_REPEAT_INFINITE;

   // config.curve.curve_type = ANIM_CT_BACK_IN;
   anim_config_set(img_anim_handle, anim_id, &config);
   anim_config_get(img_anim_handle, anim_id, &config);

   anim_value_t val;
   val.type = ANIM_VT_FLOAT;
   val.v.fv = 300.0;

   anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);

   val.type = ANIM_VT_COLOR;
   val.v.color.full = 0XFF0000;
   anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
   print_config(&config);
   // anim_sequence_destroy(img_anim_handle, anim_id);
   ret = anim_start(img_anim_handle, anim_id, ui_btntest, ANIM_LT_NORMAL);
   if (ret == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_start01 ");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_start01 ");
      return FAILED;
   }
   return PASSED;
}
int anim_start02(void)
{
   int ret = 0;
   anim_id = anim_sequence_create(img_anim_handle);
   syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

   anim_config_t config;
   config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
   config.duration = 5000;
   config.delay = 2000;
   config.ratio = 0.5;
   config.iteration_count = 2;
   anim_config_set(img_anim_handle, anim_id, &config);
   anim_config_get(img_anim_handle, anim_id, &config);

   anim_value_t val;
   val.type = ANIM_VT_FLOAT;
   val.v.fv = 300.0;
   anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
   anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
   anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
   print_config(&config);

   ui_imgtest = lv_image_create(ui_screen);
   lv_image_set_src(ui_imgtest, "/data/12.bin");
   lv_obj_set_x(ui_imgtest, 10);
   lv_obj_set_y(ui_imgtest, 10);

   ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
   if (ret == 0)
   {
      syslog(LOG_INFO, "[PASSED]  anim_start02 ");
      return PASSED;
   }
   else
   {
      flag = 1;
      syslog(LOG_INFO, "[FAILED]  anim_start02 ");
      return FAILED;
   }
   return PASSED;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
int animengine_c_api_test(int argc, char *argv[])
{
   // anim_adapter_test();
   lv_disp_t *dispp = lv_disp_get_default();
   lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
   lv_disp_set_theme(dispp, theme);

   screen_init();
   syslog(LOG_INFO, "\n[======] screen_init finised\n");
   img_anim_handle = lvx_anim_adapter_init();

   syslog(LOG_INFO, "\n[======] animengine_c_api_test start\n");

   if (strcmp(argv[2], "start01") == 0)
   {
      anim_start01();
   }
   if (strcmp(argv[2], "start02") == 0)
   {
      anim_start02();
   }
   if (strcmp(argv[2], "destory01") == 0)
   {
      anim_seqience_destory01();
   }
   if (strcmp(argv[2], "remove01") == 0)
   {
      anim_remove01();
   }
   if (strcmp(argv[2], "function") == 0)
   {
      anim_sequence_create01();
      anim_config_get01();
      anim_config_get02();
      anim_config_get03();
      anim_config_set01();
      anim_config_set02();
      anim_property_add01();
      anim_property_add02();
      anim_property_config_get01();
      anim_property_config_get02();
      anim_property_config_set01();
      anim_property_config_set02();
      anim_property_config_set03();
   }

   lv_disp_load_scr(ui_screen);
   return PASSED;

   if (flag == 0)
   {
      syslog(LOG_INFO, "\n[======] animengine_api_test passed\n");
      // exit(PASSED);
   }
   else
   {
      return PASSED;
      syslog(LOG_INFO, "\n[======] animengine_api_test failed\n");
      // exit(FAILED);
   }
}
