      
#include "physics_engine_test.h"
#include "animengine/anim_physics.h"
#include "animengine/lvx_animengine_adapter.h"
#include "data_def_test.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
/***************************************************************************/

/****************************************************************************/

/************************************************************************/

static ANIMID body_anchor = 0;
static ANIMID body_box = 0;

LV_IMG_DECLARE(week);
LV_IMG_DECLARE(time_desc);

static lv_timer_t *anim_timer = NULL;
static lv_obj_t *btn_reset = NULL;
static lv_obj_t *checkbox_debug = NULL;
static anim_engine_handle_t anim_instance = NULL;

#define BODY_NUM 2
static physics_node_t icon_nodes[BODY_NUM];

static int body_num = 0;

static int g_index = 0;
static int g_num = 0;
static bool is_debug = false;
static int joint_flag = 1;
//static int distance_ret = 0;
static int revolute_ret = 0;
//static inline void create_distance_joint(void);
static inline void create_revolute_joint(void);

/************************************************************************/


static inline void create_anchor(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        Create an empty rigid body as an anchor
    */

    // return anchor_body;
    node->body_info.position.x = 232.0;
    node->body_info.position.y = 182.0;
    node->body_info.allow_sleep = false;
    // node->body_info.type = ANIM_BODY_DYNAMIC;
    node->body_info.type = ANIM_BODY_STATIC;

    node->body = anim_create_body(instance, &(node->body_info));

    node->material.restitution = 0.3f;
    node->material.friction = 0.5f;
    body_anchor = node->body;

    anim_set_material(instance, node->body, &(node->material));
}

/* create box*/
static inline void create_distance_box_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        create a box stiffness body
    */
    node->body_info.position.x = 232.0;
    node->body_info.position.y = 282.0;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_DYNAMIC;
    // node->body_info.type = ANIM_BODY_STATIC;

    anim_size_t size;
    size.x = 40.0f;
    size.y = 40.0f;
    node->body = anim_create_body_box(instance, &(node->body_info), &size);

    node->material.restitution = 0.3f;
    node->material.friction = 1.0f;
    body_box = node->body;

    anim_set_material(instance, node->body, &(node->material));
}


static inline void create_image_and_body(physics_node_t *src, const int index)
{
    lv_obj_t *obj = lv_image_create(lv_screen_active());
    lv_image_set_src(obj, src->img_src);
    lv_obj_set_width(obj, LV_SIZE_CONTENT);
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    lv_obj_set_align(obj, LV_ALIGN_DEFAULT);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_image_set_scale(obj, 256);

    lv_obj_set_style_x(obj, 240, LV_PART_MAIN);
    lv_obj_set_style_y(obj, 240, LV_PART_MAIN);
    src->obj = obj;

    src->create_func(anim_instance, src);
}
// Random number in range [-1,1]
static int RAND_LIMIT = 32767;
static inline float random_float(void)
{
    float r = (float)(rand() & (RAND_LIMIT));
    r /= RAND_LIMIT;
    r = 2.0f * r - 1.0f;
    return r;
}

static inline void anim_timer_cb(lv_timer_t *param)
{
    g_num++;
    if (g_num > 15)
    {
        g_num = 0;

        if (body_num < BODY_NUM)
        {
            create_image_and_body(&icon_nodes[body_num], body_num);
            body_num++;
        }
        if (body_num == 2 && joint_flag == 1)
        {
           
            create_revolute_joint();
            printf("Null");
            joint_flag--;
        }
    }

    if (body_num == BODY_NUM)
    {
        ++g_index;

        if (g_index > 10)
        {
            g_index = 0;
            static int g_gravity_index = 0;
            srand((unsigned)time(NULL));

            anim_vector2f_t vPos;
            // vPos.x = random_float() * 10.0f;
            // vPos.y = random_float() * 10.0f;
            vPos.x = 0.0f;
            vPos.y = 9.0f;
            anim_set_gravity(anim_instance, vPos);
            g_gravity_index++;
        }
    }
}

static inline ANIMID create_chain_shape(anim_engine_handle_t instance, float density)
{
    /* create background*/

    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232.0f;
    info.position.y = 462.0f;

    // Define ground face, straight line
    anim_vector2f_t size;
    size.x = 470.0f;
    size.y = 20.0f;

    ANIMID body = anim_create_body_box(instance, &info, &size);
    anim_physics_material_t material_background;
    anim_get_material(anim_instance, body, &material_background);
    material_background.density = 1.0f;
    material_background.restitution = 1.0f;
    anim_set_material(instance, body, &material_background);

    return body;
}

static inline void lvx_create_button(lv_obj_t *ui_btnreset, int32_t x, int32_t y)
{
    lv_obj_set_width(ui_btnreset, 59);
    lv_obj_set_height(ui_btnreset, 33);
    lv_obj_set_x(ui_btnreset, x);
    lv_obj_set_y(ui_btnreset, y);
    lv_obj_set_align(ui_btnreset, LV_ALIGN_TOP_LEFT);
    lv_obj_add_flag(ui_btnreset, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_btnreset, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_btnreset, lv_color_hex(0xFFFFFF),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_btnreset, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static inline void lvx_create_checkbox(lv_obj_t *ui_boxscale1, const char *text, int32_t x, int32_t y)
{
    lv_checkbox_set_text(ui_boxscale1, text);
    lv_obj_set_width(ui_boxscale1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_boxscale1, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_boxscale1, x);
    lv_obj_set_y(ui_boxscale1, y);
    lv_obj_set_align(ui_boxscale1, LV_ALIGN_TOP_LEFT);
    lv_obj_add_flag(ui_boxscale1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_color(ui_boxscale1, lv_color_hex(0xFBFBFB),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_boxscale1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static inline void init_lvgl_image(physics_node_t *img, const lv_img_dsc_t *src,
                                   create_shape_func_t shape, float angle, float desity, anim_vector2f_t linear_v)
{
    img->obj = NULL;
    img->body = 0;
    img->img_src = src;
    img->create_func = shape;

    img->material.density = desity;

    anim_body_init(&(img->body_info));

    img->body_info.angle = angle;
    img->body_info.linear_velocity = linear_v;
}


static inline void create_revolute_joint(void)
{
    anim_vector2f_t l1;
    anim_vector2f_t l2;
    // l1.x = 232.0f;
    // l1.y = 100.0f;
    // l2.x = 232.0f;
    // l2.y = 100.0f;
    l1.x = 232.0f;
    l1.y = 232.0f;
    l2.x = 232.0f;
    l2.y = 152.0f;
    printf("body_anchor: : %lld\n", body_anchor);
    printf("body_box: %lld\n", body_box);
    anim_joint_t joint_body;
    joint_body.type = ANIM_JOINT_REVOLUTE;
    joint_body.body1 = body_anchor;
    joint_body.body2 = body_box;
    joint_body.user_data = NULL;
    joint_body.anchor1 = l1;
    joint_body.anchor2 = l2;

    static anim_revolute_joint_t revolute_joint_data;
    revolute_joint_data.base = joint_body;
    printf("l1.x = %f\n", l1.x);
    printf("l1.y = %f\n", l1.y);
    printf("l2.x = %f\n", l2.x);
    printf("l2.y = %f\n", l2.y);
    revolute_joint_data.lower_angle = -90.0f;
    revolute_joint_data.upper_angle = 270.0f;
    revolute_joint_data.motor_speed = 90.0f;
    revolute_joint_data.max_motor_torque = 500.0f;
    revolute_joint_data.enable_motor = true;
    revolute_ret = anim_create_joint_revolute(anim_instance, &revolute_joint_data);
    printf("revolute_ret = %d\n", revolute_ret);
}

static inline void init_lv_image_obj(void)
{
    ;
}

static void init_physics_engine(void)
{
    if (anim_instance != NULL)
    {
        anim_engine_destroy(anim_instance);
        anim_instance = NULL;
    }

    anim_instance = lvx_anim_adapter_init();

    create_chain_shape(anim_instance, 1.0);
}

static inline void ui_event_btnreset_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {

        body_num = 0;
        g_index = 0;
        g_num = 0;
        joint_flag = 1;
        for (size_t i = 0; i < BODY_NUM; i++)
        {
            if (icon_nodes[i].obj != NULL)
            {
                lv_obj_delete(icon_nodes[i].obj);
                icon_nodes[i].obj = NULL;
            }

            icon_nodes[i].body = 0;
        }

        init_physics_engine();
        anim_timer = lv_timer_create(anim_timer_cb, 16, NULL);
    }
}

static inline void ui_event_checkbox_cb(lv_event_t *e)
{
    lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);

    if ((lv_obj_get_state(target) & LV_STATE_CHECKED) && !is_debug)
    {
        anim_set_debug_model(anim_instance, ANIM_DRAW_SHAPE | ANIM_DRAW_CENTER_OF_MASS | ANIM_DRAW_JOINT);
        // anim_set_debug_model(anim_instance, ANIM_DRAW_JOINT);
        is_debug = true;
    }
    else if (!(lv_obj_get_state(target) & LV_STATE_CHECKED) && is_debug)
    {
        anim_set_debug_model(anim_instance, ANIM_DRAW_NONE);
        is_debug = false;
    }
}

int anim_physics_revolute_joint_test(int argc, char *agrv[])
{
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(
        dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    /*Initialize Physics engine*/
    init_physics_engine();
    /*Initialize screen*/
    anim_vector2f_t linear_v_anchor;
    linear_v_anchor.x = 0.0f;
    linear_v_anchor.y = 0.0f;
    anim_vector2f_t linear_v;
    linear_v.x = 10.0f;
    linear_v.y = 1000.6f;

    init_lvgl_image(&icon_nodes[0], NULL, create_anchor, 0, 30.0, linear_v_anchor);
    init_lvgl_image(&icon_nodes[1], NULL, create_distance_box_rigid_body, 90, 3.0, linear_v);

    /*Create debug button*/
    static int SLIDE_NUM = 1;
    btn_reset = lv_btn_create(lv_screen_active());
    lvx_create_button(btn_reset, 232, 80);
    lv_obj_add_event_cb(btn_reset, ui_event_btnreset_cb, LV_EVENT_CLICKED, NULL);

    checkbox_debug = lv_checkbox_create(lv_screen_active());
    lvx_create_checkbox(checkbox_debug, "debug", 232, 100 + 50 * SLIDE_NUM);
    lv_obj_add_event_cb(checkbox_debug, ui_event_checkbox_cb, LV_EVENT_ALL, NULL);

    /*CollisionCallback*/
    // anim_register_collision_listener
    // anim_timer = lv_timer_create(anim_timer_cb, 16, NULL);

    return 0;
}

    