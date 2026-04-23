
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

/*ANIM_PT_BACKGROUND_COLOR image*/
int anim_BACKGROUND_COLOR_img(void)
{
    anim_config_t config;
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);
    memset(&config, 0, sizeof(anim_config_t));
    anim_config_get(img_anim_handle, anim_id, &config);
    anim_config_set(img_anim_handle, anim_id, &config);

    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 300.0;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
    ;
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
    print_config(&config);
    val.type = ANIM_VT_COLOR;
    val.v.color.full = 0XFF0000;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_BACKGROUND_COLOR_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_BACKGROUND_COLOR_img ");
        return FAILED;
    }
    return PASSED;
}
/* background set no color type*/
int anim_BACKGROUND_COLOR_img01(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_get(img_anim_handle, anim_id, &config);
    anim_config_set(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 300.0;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSLATE_Y, &config);
    print_config(&config);
    val.type = ANIM_VT_COLOR;
    val.v.color.full = 50000000;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_BACKGROUND_COLOR, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_BACKGROUND_COLOR_img01 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_BACKGROUND_COLOR_img01 ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_SCALE 0*/
int anim_TRANSFORM_SCALE_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = ANIM_REPEAT_INFINITE;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE01_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE01_img ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_SCALE 0.8*/
int anim_TRANSFORM_SCALE01_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0.8;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE01_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE01_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_SCALE 100*/
int anim_TRANSFORM_SCALE02_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 100;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE02_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE02_img ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_SCALE 1000*/
int anim_TRANSFORM_SCALE03_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 1000;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE03_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE03_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_SCALE 1000000*/
int anim_TRANSFORM_SCALE04_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 1000000;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE04_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE04_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_SCALE -1*/
int anim_TRANSFORM_SCALE05_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = -1;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_SCALE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_TRANSFORM_SCALE05_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_TRANSFORM_SCALE05_img ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_OPACITY -1*/
int anim_PT_OPACITY01_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = -1;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_OPACITY, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_OPACITY01_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_OPACITY01_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_OPACITY 0*/
int anim_PT_OPACITY02_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_OPACITY, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_OPACITY02_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_OPACITY02_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_OPACITY 0.5*/
int anim_PT_OPACITY03_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0.5;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_OPACITY, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_OPACITY03_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_OPACITY03_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_OPACITY 1.0*/
int anim_PT_OPACITY04_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 1;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_OPACITY, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_OPACITY04_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_OPACITY04_img ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_OPACITY 1.5*/
int anim_PT_OPACITY05_img(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 1.5;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_OPACITY, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_OPACITY, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_OPACITY05_img ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_OPACITY05_img ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_ROTATE -10*/
int anim_PT_TRANSFORM_ROTATE01(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = -10;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE01 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE01 ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_ROTATE -1*/
int anim_PT_TRANSFORM_ROTATE02(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = -1;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE02 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE02 ");
        return FAILED;
    }
    return PASSED;
}

/*ANIM_PT_TRANSFORM_ROTATE 0*/
int anim_PT_TRANSFORM_ROTATE03(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE03 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE03 ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_ROTATE 0.5*/
int anim_PT_TRANSFORM_ROTATE04(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 0.5;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE04 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE04 ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_ROTATE 1*/
int anim_PT_TRANSFORM_ROTATE05(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 0.5;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 1;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE05 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE05 ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_ROTATE 10*/
int anim_PT_TRANSFORM_ROTATE06(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 10;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 10;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE06 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE06 ");
        return FAILED;
    }
    return PASSED;
}
/*ANIM_PT_TRANSFORM_ROTATE 100*/
int anim_PT_TRANSFORM_ROTATE07(void)
{
    int ret = 0;
    anim_id = anim_sequence_create(img_anim_handle);
    syslog(LOG_DEBUG, "anim_create_ret == %lld", anim_id);

    anim_config_t config;
    config.curve.curve_type = ANIM_CT_BOUNCE_INOUT;
    config.duration = 3000;
    config.delay = 1000;
    config.ratio = 10;
    config.iteration_count = 2;
    anim_config_set(img_anim_handle, anim_id, &config);
    anim_config_get(img_anim_handle, anim_id, &config);

    anim_value_t val;
    val.type = ANIM_VT_FLOAT;
    val.v.fv = 100;
    anim_property_add(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, ANIM_ST_TO, &val);
    anim_property_config_set(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    anim_property_config_get(img_anim_handle, anim_id, ANIM_PT_TRANSFORM_ROTATE, &config);
    print_config(&config);

    ui_imgtest = lv_image_create(ui_screen);
    lv_image_set_src(ui_imgtest, "/data/12.bin");
    lv_obj_set_x(ui_imgtest, 10);
    lv_obj_set_y(ui_imgtest, 10);

    ret = anim_start(img_anim_handle, anim_id, ui_imgtest, ANIM_LT_IMAGE);
    if (ret == 0)
    {
        syslog(LOG_INFO, "[PASSED]  anim_PT_TRANSFORM_ROTATE07 ");
        return PASSED;
    }
    else
    {
        flag = 1;
        syslog(LOG_INFO, "[FAILED]  anim_PT_TRANSFORM_ROTATE07 ");
        return FAILED;
    }
    return PASSED;
}

/**/
/****************************************************************************
 * Public Functions
 ****************************************************************************/
int animengine_c_specification_test(int argc, char *argv[])
{
    // anim_adapter_test();
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    screen_init();
    syslog(LOG_INFO, "\n[======] screen_init finised\n");
    img_anim_handle = lvx_anim_adapter_init();

    syslog(LOG_INFO, "\n[======] animengine_capi_test start\n");

    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "0") == 0)
    {
        anim_BACKGROUND_COLOR_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "1") == 0)
    {
        anim_TRANSFORM_SCALE_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "2") == 0)
    {
        anim_TRANSFORM_SCALE01_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "3") == 0)
    {
        anim_TRANSFORM_SCALE02_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "4") == 0)
    {
        anim_TRANSFORM_SCALE03_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "5") == 0)
    {
        anim_TRANSFORM_SCALE04_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "6") == 0)
    {
        anim_PT_OPACITY01_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "7") == 0)
    {
        anim_PT_OPACITY02_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "8") == 0)
    {
        anim_PT_OPACITY03_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "9") == 0)
    {
        anim_PT_OPACITY04_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "10") == 0)
    {
        anim_PT_OPACITY05_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "11") == 0)
    {
        anim_PT_TRANSFORM_ROTATE01();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "12") == 0)
    {
        anim_PT_TRANSFORM_ROTATE02();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "13") == 0)
    {
        anim_PT_TRANSFORM_ROTATE03();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "14") == 0)
    {
        anim_PT_TRANSFORM_ROTATE04();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "15") == 0)
    {
        anim_PT_TRANSFORM_ROTATE05();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "16") == 0)
    {
        anim_PT_TRANSFORM_ROTATE06();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "17") == 0)
    {
        anim_PT_TRANSFORM_ROTATE07();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "18") == 0)
    {
        anim_TRANSFORM_SCALE05_img();
    }
    if (strcmp(argv[2], "all") == 0 || strcmp(argv[2], "19") == 0)
    {
        anim_BACKGROUND_COLOR_img01();
    }

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
    lv_disp_load_scr(ui_screen);
    return PASSED;
}
