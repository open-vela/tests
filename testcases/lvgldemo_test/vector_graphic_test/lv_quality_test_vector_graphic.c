/**
 * @file lv_quality_test_vector_graphic.c
 *
 */

/*********************
 * INCLUDES
 *********************/
#include "lv_demo_vector_graphic_test.h"
#include "sys/time.h"
#include "time.h"
#include "unistd.h"
#include <math.h>
#include <stdlib.h>

#if CONFIG_VECTOR_GRAPHIC_TEST

typedef struct {
    lv_obj_t* obj;
    lv_timer_t* anim_timer;
    int iterations;
} lv_vector_demo_t;

static int g_caseId;
static lv_obj_t* g_canvas;
static lv_obj_t* g_list;

static void anim_timer_cb(lv_timer_t* timer);
static void vector_demo_constructor(lv_obj_t* obj);
static void vector_demo_destructor(lv_obj_t* obj);
static void vector_demo_event_cb(lv_event_t* e);
static void draw_vector(lv_layer_t* layer, int iterations);

/*********************
 * DEFINES
 *********************/
#define WIDTH 640
#define HEIGHT 480

/**********************
 * TYPEDEFS
 **********************/

/**********************
 * STATIC PROTOTYPES
 **********************/

static void draw_performance_path_reuse(lv_vector_dsc_t* ctx, lv_vector_path_t* path, int iterations)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t pc = { 100, 100 };
    lv_vector_path_append_circle(path, &pc, 50, 50);

    for (int i = 0; i < iterations; i++) {
        lv_color_t color = lv_color_make(rand() % 255, rand() % 255, rand() % 255);

        lv_matrix_t matrix;
        lv_matrix_identity(&matrix);
        float scale = 0.5f + (rand() % 150) / 100.0f;
        lv_matrix_scale(&matrix, scale, scale);
        float x = rand() % 640;
        float y = rand() % 480;
        lv_matrix_translate(&matrix, x, y);
        lv_vector_dsc_set_transform(ctx, &matrix);

        lv_vector_dsc_set_stroke_color(ctx, color);
        lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
        lv_vector_dsc_set_stroke_width(ctx, 2.0f);
        lv_vector_dsc_set_fill_color(ctx, color);
        lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);

        lv_vector_dsc_add_path(ctx, path);
    }
}

static void draw_performance_dsc_reuse(lv_vector_dsc_t* ctx, lv_vector_path_t* path, int iterations)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xA8, 0xF3));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);

    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xA8, 0xF3));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_stroke_width(ctx, 2.0f);

    for (int i = 0; i < iterations; i++) {
        lv_vector_path_clear(path);
        lv_vector_dsc_identity(ctx);

        int points_num = 3 + rand() % 6;
        int start_x = rand() % (WIDTH - 50);
        int start_y = rand() % (400 - 50);

        lv_fpoint_t pts1 = { start_x, start_y };
        lv_vector_path_move_to(path, &pts1);

        for (int j = 0; j < points_num; j++) {
            int x = start_x + rand() % 50;
            int y = start_y + rand() % 50;
            lv_fpoint_t pts2 = { x, y };
            lv_vector_path_line_to(path, &pts2);
        }
        lv_vector_path_close(path);
        lv_vector_dsc_add_path(ctx, path);
    }
}

static void anim_timer_cb(lv_timer_t* timer)
{
    lv_vector_demo_t* demo = (lv_vector_demo_t*)timer->user_data;
    lv_obj_invalidate(demo->obj);
}

static void vector_demo_constructor(lv_obj_t* obj)
{
    lv_vector_demo_t* demo = (lv_vector_demo_t*)lv_obj_get_user_data(obj);
    demo->obj = obj;
    demo->anim_timer = lv_timer_create(anim_timer_cb, 16, demo);
}

static void vector_demo_destructor(lv_obj_t* obj)
{
    lv_vector_demo_t* demo = (lv_vector_demo_t*)lv_obj_get_user_data(obj);
    lv_timer_delete(demo->anim_timer);
}

static void vector_demo_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_current_target(e);
    lv_vector_demo_t* demo = (lv_vector_demo_t*)lv_obj_get_user_data(obj);

    if (code == LV_EVENT_DRAW_MAIN) {
        lv_layer_t* layer = lv_event_get_layer(e);
        draw_vector(layer, demo->iterations);
    }
}

static void draw_vector(lv_layer_t* layer, int iterations)
{
    lv_vector_dsc_t* ctx = lv_vector_dsc_create(layer);
    lv_vector_path_t* path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    switch (g_caseId) {
    case 1:
        draw_performance_path_reuse(ctx, path, iterations);
        break;
    case 2:
        draw_performance_dsc_reuse(ctx, path, iterations);
        break;
    default:
        break;
    }

    lv_draw_vector(ctx);

    lv_vector_path_delete(path);
    lv_vector_dsc_delete(ctx);
}

static void delete_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_current_target(e);
    lv_draw_buf_t* draw_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_destroy(draw_buf);
    vector_demo_destructor(obj);
}

static void click_event_cb(lv_event_t* e)
{
    lv_obj_add_flag(g_canvas, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(g_list, LV_OBJ_FLAG_HIDDEN);
}

static void event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target_obj(e);
    if (code == LV_EVENT_CLICKED) {
        LV_UNUSED(obj);
        lv_obj_t* list = lv_event_get_user_data(e);
        const char* txt = lv_list_get_button_text(list, obj);
        if (strcmp("performance path reuse", txt) == 0) {
            g_caseId = 1;
        } else if (strcmp("performance desc reuse", txt) == 0) {
            g_caseId = 2;
        }
        lv_obj_remove_flag(g_canvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);

        LV_LOG_USER("Clicked: %s", lv_list_get_button_text(list, obj));
    }
}

static void testcase_list_create(void)
{
    /*Create a list*/
    lv_obj_t* list;
    list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, 400, 400);
    lv_obj_center(list);

    /*Add buttons to the list*/
    lv_obj_t* btn;
    lv_list_add_text(list, "vector cases");
    btn = lv_list_add_button(list, LV_SYMBOL_FILE, "performance path reuse");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_DIRECTORY, "performance desc reuse");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);

    g_list = list;
}

/**********************
 * STATIC VARIABLES
 **********************/

/**********************
 * MACROS
 **********************/

/**********************
 * GLOBAL FUNCTIONS
 **********************/

void lv_quality_test_vector_graphic(void* param)
{
    LV_ASSERT_NULL(param);
    int iterations = *(int*)param;
    testcase_list_create();

    lv_draw_buf_t* draw_buf = lv_draw_buf_create(WIDTH, HEIGHT, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    lv_draw_buf_clear(draw_buf, NULL);

    lv_obj_t* canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_draw_buf(canvas, draw_buf);

    lv_vector_demo_t* demo = (lv_vector_demo_t*)lv_malloc(sizeof(lv_vector_demo_t));
    demo->iterations = iterations > 0 ? iterations : 1000; // Default to 1000 if not specified

    lv_obj_set_user_data(canvas, demo);
    vector_demo_constructor(canvas);

    lv_obj_add_event_cb(canvas, vector_demo_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(canvas, delete_event_cb, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(canvas, click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_add_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);

    g_canvas = canvas;
}

/**********************
 * STATIC FUNCTIONS
 **********************/
#else

void lv_quality_test_vector_graphic(void* param)
{
    LV_UNUSED(param);
#if LV_USE_LABEL != 0
    /*fallback for online examples*/
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Vector graphics is not enabled");
    lv_obj_center(label);
#else
    LV_LOG_WARN("Vector graphics is not enabled");
#endif
}

#endif
