
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
static lv_obj_t* ui_screen;
static lv_obj_t* ui_btntest;
static int flag = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void get_anim_status(anim_layer_t* layer, const anim_status_type_t status)
{
   // syslog(LOG_INFO, "status = %d; property_type = %d; layer_type = %d",
   //        status, layer->property_type, layer->layer_type);
   if (status == ANIM_ST_END){
   syslog(LOG_INFO,  "It's OK, status == %d", ANIM_ST_END);
      // anim_remove(anim_engine_handle, g_anim_id);
   }
}

static void get_anim_update(anim_layer_t* layer, const anim_value_t* value)
{
   // syslog(LOG_INFO, "value_type = %d", value->type);
}

static void get_user_data(anim_layer_t* layer, const anim_value_t* value)
{
}


static void screen_init(void)
{
   ui_screen = lv_obj_create(NULL);
   lv_obj_clear_flag(ui_screen, LV_OBJ_FLAG_SCROLLABLE);
   lv_obj_set_style_bg_color(ui_screen, lv_color_hex(0x375830),
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

int animengine_display(const char* config)
{
   int ret = 0;

   ret = anim_create(anim_engine_handle, &g_anim_id, config);
   syslog(LOG_DEBUG, "anim_create_ret == %d", ret);
   if (ret == 0) {
      syslog(LOG_INFO, "g_anim_id == %" PRId64, g_anim_id);
      syslog(LOG_INFO, "anim_create successfully");
   } else {
      syslog(LOG_INFO, "anim_create failed");
      return FAILED;
   }

   anim_listener(anim_engine_handle, g_anim_id, get_anim_status, get_anim_update, get_user_data);

   ret = anim_start(anim_engine_handle, g_anim_id, ui_btntest, ANIM_LT_NORMAL);
   syslog(LOG_DEBUG, "anim_start_ret == %d", ret);
   if (ret == 0) {
      syslog(LOG_INFO, "anim_start successfully");
   } else {
      syslog(LOG_INFO, "anim_start failed");
      return FAILED;
   }

   return PASSED;
}

int fromState_toState(void)
{
   char *json = "{\"fromState\":{\"x\":50,\"y\":50,\"scale\":0.5,\"rotate\":180,"
                "\"width\":100,\"height\":100,\"opacity\":0.5,\"bg_color\":\"#f0f\"},"
                "\"toState\":{\"x\":200,\"y\":200,\"scale\":1,\"rotate\":360,\"width\":30,"
                "\"height\":60,\"opacity\":0,\"bg_color\":\"#ff001100\"},\"config\":{\"ease\":[\"linear\",4],"
                "\"delay\":2,\"duration\":4,\"ratio\":1,\"iterations\":\"infinite\",\"direction\":\"normal\"},\"id\":1}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int fromState_byState(void)
{
   char *json = "{\"fromState\":{\"x\":50,\"y\":50,\"scale\":0.5,\"rotate\":180,"
                "\"width\":100,\"height\":100,\"opacity\":0.5,\"bg_color\":\"#fff\"},"
                "\"byState\":{\"x\":200,\"y\":200,\"scale\":1,\"rotate\":360,\"width\":200,"
                "\"height\":200,\"opacity\":1.0,\"bg_color\":\"#000000\"},\"config\":{\"ease\":[\"cubic-bezier\",3],"
                "\"duration\":3.3,\"ratio\":1.5,\"iterations\":\"infinite\",\"direction\":\"reverse\"},\"id\":2}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int byState_toState(void)
{
   char *json = "{\"byState\":{\"x\":50,\"y\":50,\"scale\":0.5,\"rotate\":180,"
                "\"width\":100,\"height\":100,\"opacity\":0.1,\"bg_color\":\"#E066FF\"},"
                "\"toState\":{\"x\":200,\"y\":200,\"scale\":1,\"rotate\":360,\"width\":200,\"height\":200,"
                "\"opacity\":0.7,\"bg_color\":\"#BC8F8F\"},\"config\":{\"ease\":[\"backIn\",3,5],"
                "\"duration\":3.3,\"ratio\":1.5,\"iterations\":\"infinite\",\"direction\":\"alternate\"},\"id\":3}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int fromState(void)
{
   char *json = "{\"fromState\":{\"x\":50,\"scale\":0.5,\"rotate\":180,\"width\":100,\"height\":100,"
                "\"opacity\":0.1,\"bg_color\":\"#33ff00\"},\"config\":{\"ease\":[\"accelerate\",1,5],"
                "\"ratio\":3,\"iterations\":\"infinite\",\"direction\":\"alternate-reverse\"},\"id\":4}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int toState(void)
{
   char *json = "{\"toState\":{\"y\":200,\"scale\":1,\"rotate\":5,\"width\":30,\"bg_color\":\"#0066ff\"},"
                "\"config\":{\"ease\":[\"bounceIn\"],\"delay\":2,\"duration\":4,\"ratio\":0.2,\"direction\":\"reverse\"},\"id\":5}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int byState(void)
{
   char *json = "{\"byState\":{\"x\":50,\"y\":50,\"scale\":0.5,\"rotate\":180,\"width\":100,"
                "\"height\":100,\"opacity\":0.5,\"bg_color\":\"#f0f\"},\"config\":{\"ease\":[\"bounceInOut\",4.56],"
                "\"delay\":2,\"ratio\":1,\"iterations\":\"infinite\",\"direction\":\"alternate\"},\"id\":6}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int allState(void)
{
   char *json = "{\"fromState\":{},\"byState\":{},\"toState\":{},\"config\":{},\"id\":7}";

   if (animengine_display(json) == FAILED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int noState(void)
{
   char *json = "{\"config\":{\"ease\":[\"circlularInOut\",3.3333],\"iterations\":5,\"direction\":\"alternate\"},\"id\":8}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int keyFramesandState(void)
{
   char *json = "{\"keyFrames\":{},\"fromState\":{},\"config\":{},\"id\":9}";

   if (animengine_display(json) == FAILED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}

int keyFrames(void)
{
   char *json = "{\"id\":10,\"keyFrames\":{\"x\":[{\"value\":0,\"time\":0,\"ease\":[\"linear\"]},"
                "{\"value\":200,\"time\":1.5,\"ease\":[\"elasticIn\"]},"
                "{\"value\":300,\"time\":2.5,\"ease\":[\"sineOut\"]},"
                "{\"value\":249,\"time\":3.5,\"ease\":[\"bounceOut\"]},"
                "{\"value\":300,\"time\":4.5}]}}";

   if (animengine_display(json) == PASSED){
      return PASSED;
   } else {
      flag = 1;
      return FAILED;
   }
}


/****************************************************************************
 * Public Functions
 ****************************************************************************/
int animengine_specification_test(int argc, char *argv[])
{
   lv_disp_t* dispp = lv_disp_get_default();
   lv_theme_t* theme = lv_theme_default_init(
        dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        false, LV_FONT_DEFAULT);
   lv_disp_set_theme(dispp, theme);

   screen_init();

   syslog(LOG_INFO, "\n[======] animengine_specification_test start\n");

   anim_engine_handle = lvx_anim_adapter_init();

   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "0") == 0 ) {
      fromState_toState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "1") == 0 ) {
      fromState_byState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "2") == 0 ) {
      byState_toState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "3") == 0 ) {
      fromState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "4") == 0 ) {
      toState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "5") == 0 ) {
      byState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "6") == 0 ) {
      keyFrames();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "7") == 0 ) {
      noState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "8") == 0 ) {
      allState();
   }
   if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "9") == 0 ) {
      keyFramesandState();
   }

   if (flag == 0) {
      syslog(LOG_INFO, "\n[======] animengine_specification_test passed\n");
   } else {
      syslog(LOG_INFO, "\n[======] animengine_specification_test failed\n");
   }

   lv_disp_load_scr(ui_screen);

   return PASSED;
}

int animengine_stability(int argc, char *argv[])
{
   lv_disp_t* dispp = lv_disp_get_default();
   lv_theme_t* theme = lv_theme_default_init(
        dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        false, LV_FONT_DEFAULT);
   lv_disp_set_theme(dispp, theme);

   screen_init();

   syslog(LOG_INFO, "\n[======] animengine_stability start\n");
   anim_engine_handle = lvx_anim_adapter_init();
   lv_disp_load_scr(ui_screen);

   int a = 0;
   while (a < 20) {
      fromState_toState();
      fromState_byState();
      toState();
      byState();
      keyFrames();
      fromState_byState();
      byState();
      fromState_toState();
      keyFrames();
      toState();
      syslog(LOG_INFO, "a: %d\n", a);
      a++;
      sleep(1);
   }
   syslog(LOG_INFO, "\n[======] animengine_stability passed\n");
   return PASSED;
}
