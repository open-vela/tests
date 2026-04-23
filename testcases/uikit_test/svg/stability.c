/**
 * @file tiger.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "svg_test.h"

#if LV_USE_VECTOR_GRAPHIC || LV_USE_VECTOR_GRAPHIC_OPTIMIZE

/*********************
 *      DEFINES
 *********************/

#define SVG_PATH "/data/svg/tiger.svg"

#define MY_CLASS &lv_tiger_class1

/*********************
 *      TYPEDEFS
 *********************/

typedef struct {
    lv_obj_t obj;
    lv_timer_t* anim_timer;
    float scale;
    uint32_t reload_count;
    lv_svg_render_obj_t* draw_list;
} lv_tiger_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t* lv_tiger_create(lv_obj_t* parent);

static void lv_tiger_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj);
static void lv_tiger_destructor(const lv_obj_class_t* class_p, lv_obj_t* obj);
static void lv_tiger_event(const lv_obj_class_t* class_p, lv_event_t* e);
static void tiger_anim_timer_cb(lv_timer_t* timer);
static void tiger_reload_timer_cb(lv_timer_t* timer);
static void* alloc_file(const char* filename, uint32_t* size);
static void lv_tiger_load_file(lv_obj_t* obj, const char* path);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_tiger_class1 = {
    .constructor_cb = lv_tiger_constructor,
    .destructor_cb = lv_tiger_destructor,
    .event_cb = lv_tiger_event,
    .instance_size = sizeof(lv_tiger_t),
    .base_class = &lv_obj_class,
    .name = "tiger",
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void uikit_draw_test_tiger(char* info[], int size, void* param)
{
    lv_obj_t* tiger_widget = lv_tiger_create(lv_scr_act());
    lv_obj_set_size(tiger_widget, LV_PCT(100), LV_PCT(100));
    lv_timer_create(tiger_reload_timer_cb, 1000, tiger_widget);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tiger_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    lv_tiger_t* tiger = (lv_tiger_t*)obj;

    lv_tiger_load_file(obj, SVG_PATH);
    tiger->anim_timer = lv_timer_create(tiger_anim_timer_cb, 16, tiger);
}

static void lv_tiger_destructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    lv_tiger_t* tiger = (lv_tiger_t*)obj;
    lv_svg_render_delete(tiger->draw_list);
    lv_timer_delete(tiger->anim_timer);
}

static void lv_tiger_load_file(lv_obj_t* obj, const char* path)
{
    LV_LOG_USER("path: %s", path);
    lv_tiger_t* tiger = (lv_tiger_t*)obj;
    if (path == NULL && tiger->draw_list == NULL)
        return;
    if (tiger->draw_list != NULL) {
        lv_svg_render_delete(tiger->draw_list);
        tiger->draw_list = NULL;
    }
    if (path == NULL)
        return;
    uint32_t size = 0;
    void* svg_data = alloc_file(path, &size);
    LV_ASSERT_NULL(svg_data);

    uint32_t start = lv_tick_get();
    lv_svg_node_t* svg_doc = lv_svg_load_data(svg_data, size);
    LV_ASSERT_NULL(svg_doc);
    tiger->draw_list = lv_svg_render_create(svg_doc);
    tiger->scale = 1.0f;
    LV_LOG_USER("load svg file use %" LV_PRIu32 " ms", lv_tick_elaps(start));

    lv_svg_node_delete(svg_doc);
    lv_free(svg_data);
    LV_LOG_USER("RELOAD SVG FILE IS %s", path);
}

static void draw_tiger_svg(lv_obj_t* obj, lv_layer_t* layer)
{
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_tiger_t* t = (lv_tiger_t*)obj;

    lv_vector_dsc_t* ctx = lv_vector_dsc_create(layer);
    lv_area_t rect = { 0, 0, w, h };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect); // clear screen

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, 240, 240);
    lv_matrix_scale(&matrix, t->scale, t->scale);
    lv_matrix_translate(&matrix, -240, -240);

    lv_vector_dsc_set_transform(ctx, &matrix);
    lv_draw_svg_render(ctx, t->draw_list);

    lv_draw_vector(ctx); // submit draw
    lv_vector_dsc_delete(ctx);
}

static void draw_main(lv_obj_t* obj, lv_event_t* e)
{
    lv_tiger_t* t = (lv_tiger_t*)obj;
    lv_layer_t* layer = lv_event_get_layer(e);
    uint32_t start = lv_tick_get();

    draw_tiger_svg(obj, layer);

    /* Print draw time when draw list is not empty */
    if(t->draw_list) {
        LV_LOG_USER("draw frame use %" LV_PRIu32 " ms", lv_tick_elaps(start));
    }
}

static void lv_tiger_event(const lv_obj_class_t* class_p, lv_event_t* e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if (res != LV_RESULT_OK)
        return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_current_target(e);
    if (code == LV_EVENT_DRAW_MAIN) {
        draw_main(obj, e);
    } else if (code == LV_EVENT_CLICKED) {
        lv_tiger_t* tiger = (lv_tiger_t*)obj;
        tiger->scale *= 0.5f;

        if (tiger->scale < 0.125f) {
            tiger->scale = 1.0f;
        }
    }
}

static void tiger_anim_timer_cb(lv_timer_t* timer)
{
    lv_obj_t* obj = lv_timer_get_user_data(timer);
    lv_obj_invalidate(obj);
}

static void tiger_reload_timer_cb(lv_timer_t* timer)
{
    lv_obj_t* tiger_widget = lv_timer_get_user_data(timer);
    lv_tiger_t* tiger = (lv_tiger_t*)tiger_widget;
    if (tiger->draw_list == NULL) {
        lv_tiger_load_file(tiger_widget, SVG_PATH);
    } else {
        lv_tiger_load_file(tiger_widget, NULL);
    }

    if (tiger->reload_count > 10) {
        lv_obj_delete(tiger_widget);
        tiger_widget = lv_tiger_create(lv_scr_act());
        lv_obj_set_size(tiger_widget, LV_PCT(100), LV_PCT(100));
        lv_timer_set_user_data(timer, tiger_widget);
    } else {
        tiger->reload_count++;
    }
}

static lv_obj_t* lv_tiger_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

static void* alloc_file(const char* filename, uint32_t* size)
{
    uint8_t* data = NULL;
    lv_fs_file_t f;
    uint32_t data_size;
    uint32_t rn;
    lv_fs_res_t res;

    *size = 0;

    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_WARN("can't open %s", filename);
        return NULL;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if (res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_tell(&f, &data_size);
    if (res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
    if (res != LV_FS_RES_OK) {
        goto failed;
    }

    /*Read file to buffer*/
    data = lv_malloc(data_size);
    if (data == NULL) {
        LV_LOG_WARN("malloc failed for data size %" LV_PRIu32, data_size);
        goto failed;
    }

    res = lv_fs_read(&f, data, data_size, &rn);

    if (res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    } else {
        LV_LOG_WARN("read file failed");
        lv_free(data);
        data = NULL;
    }

failed:
    lv_fs_close(&f);
    return data;
}

#else

void uikit_draw_test_tiger(char* info[], int size, void* param)
{
    /*fallback for online examples*/
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Vector graphics is not enabled");
    lv_obj_center(label);
}

#endif