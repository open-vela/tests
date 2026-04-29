
/**
 * @file tiger.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "svg_test.h"
#include <stdio.h>

#if LV_USE_VECTOR_GRAPHIC || LV_USE_VECTOR_GRAPHIC_OPTIMIZE
#include "time.h"
#include "unistd.h"
#include "sys/time.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

static inline suseconds_t get_time(void)
{
    struct timeval t;
    gettimeofday(&t, 0);
    suseconds_t t1 = (suseconds_t)(t.tv_sec * 1000  + t.tv_usec/1000);
    return t1;
}

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * lv_tiger_create(lv_obj_t * parent);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * g_svg_path;
static const char * g_image_path;

typedef struct {
    lv_obj_t obj;
    lv_timer_t * anim_timer;
    float scale;
    lv_svg_render_obj_t * draw_list;
} lv_tiger_t;

#define MY_CLASS &lv_tiger_class2

static void draw_animation(lv_obj_t * obj, lv_layer_t * layer)
{
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_tiger_t * t = (lv_tiger_t *)obj;

    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);
    lv_area_t rect = {0, 0, w, h};
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

static void widget_draw(lv_tiger_t * tiger_widget)
{
    lv_obj_t * obj = (lv_obj_t *)tiger_widget;
    lv_obj_invalidate(obj);
}

static void anim_timer_cb(lv_timer_t * param)
{
    lv_tiger_t * t = (lv_tiger_t *)param->user_data;
    widget_draw(t);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void lv_tiger_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tiger_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tiger_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_tiger_draw(lv_obj_t * obj, lv_event_t * e);

const lv_obj_class_t lv_tiger_class2  = {
    .constructor_cb = lv_tiger_constructor,
    .destructor_cb = lv_tiger_destructor,
    .event_cb = lv_tiger_event,
    .instance_size = sizeof(lv_tiger_t),
    .base_class = &lv_obj_class,
    .name = "tiger",
};

static lv_obj_t * lv_tiger_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}


static uint8_t * alloc_file(const char * filename, uint32_t * size)
{
    uint8_t * data = NULL;
    lv_fs_file_t f;
    uint32_t data_size;
    uint32_t rn;
    lv_fs_res_t res;

    *size = 0;

    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("can't open %s", filename);
        return NULL;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_tell(&f, &data_size);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    /*Read file to buffer*/
    data = lv_malloc(data_size);
    if(data == NULL) {
        LV_LOG_WARN("malloc failed for data size %" LV_PRIu32, data_size);
        goto failed;
    }

    res = lv_fs_read(&f, data, data_size, &rn);

    if(res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    }
    else {
        LV_LOG_WARN("read file failed");
        lv_free(data);
        data = NULL;
    }

failed:
    lv_fs_close(&f);
    return data;
}

static void lv_tiger_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_tiger_t * tiger = (lv_tiger_t *)obj;


    uint32_t size = 0;
    uint8_t * svg_data = alloc_file(g_svg_path, &size);
    suseconds_t t1 = get_time();
    lv_svg_node_t * svg_doc = lv_svg_load_data((char *)svg_data, size);
    tiger->draw_list = lv_svg_render_create(svg_doc);
    suseconds_t t2 = get_time();
    tiger->scale = 1.0f;


    LV_LOG_ERROR("load svg file use %.2f ms", (float)t2-t1);


    lv_svg_node_delete(svg_doc);
    lv_free(svg_data);

    tiger->anim_timer = lv_timer_create(anim_timer_cb, 16, tiger);
}

static void lv_tiger_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_tiger_t * tiger = (lv_tiger_t *)obj;
    lv_timer_delete(tiger->anim_timer);
}

static void lv_tiger_draw(lv_obj_t * obj, lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    // suseconds_t t1 = get_time();
    draw_animation(obj, layer);
    // suseconds_t t2 = get_time();

    // LV_LOG_WARN("draw frame use %.4f ms", (double)(t2-t1));
}

static void lv_tiger_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_DRAW_MAIN) {
        lv_tiger_draw(obj, e);
    }
    else if(code == LV_EVENT_CLICKED) {
        lv_tiger_t * tiger = (lv_tiger_t *)obj;
        tiger->scale *= 0.5f;

        if (tiger->scale < 0.125f) {
           tiger->scale = 1.0f;
        }
    }
}

static void load_image(const char * image_url, lv_draw_image_dsc_t * img_dsc)
{
    img_dsc -> header.w = 100;
    img_dsc -> header.h = 100;
    img_dsc->src = g_image_path;
 }

static const char * get_font_path(const char * font_family)
{
    LV_UNUSED(font_family);
    return "/data/NotoSansSC-Regular.ttf";
}

static lv_svg_render_hal_t hal = {
    .load_image = load_image,
    .get_font_path = get_font_path,
};

void uikit_draw_demo_tigers(char * info[], int size, void * param)
{
    if (size < 1) {
        printf("Usage: uikit_demo <command> [path]\n");
        return;
    }
    // g_svg_path = (size >= 2) ? info[1] : "/data/tiger.svg";
    if (size >= 2 && info[1] != NULL) {
        int value = atoi(info[1]);
        switch (value) {
            case 1:
                g_svg_path = "/data/svg.svg";
                break;
            case 2:
                g_svg_path = "/data/g_t.svg";
                break;
            case 3:
                g_svg_path = "/data/g_q.svg";
                g_image_path = "/data/svg.png";
                break;
            case 4:
                g_svg_path = "/data/use_t.svg";
                break;
            case 5:
                g_svg_path = "/data/defs_q.svg";
                break;
            case 6:
                g_svg_path = "/data/rect_t.svg";
                break;
            case 7:
                g_svg_path = "/data/circle_t.svg";
                break;
            case 8:
                g_svg_path = "/data/ellipse_t.svg";
                break;
            case 9:
                g_svg_path = "/data/line_t.svg";
                break;
            case 10:
                g_svg_path = "/data/polyline_t.svg";
                break;
            case 11:
                g_svg_path = "/data/polygon_t.svg";
                break;
            case 12:
                g_svg_path = "/data/path_t.svg";
                break;
            case 13:
                g_svg_path = "/data/solidcolor_t.svg";
                break;
            case 14:
                g_svg_path = "/data/linearGradiwnt_t.svg";
                break;
            case 15:
                g_svg_path = "/data/radialGradient_t.svg";
                break;
            case 16:
                g_svg_path = "/data/text_t.svg";
                break;
            case 17:
                g_svg_path = "/data/tspan_t.svg";
                break;
            case 180:
                g_svg_path = "/data/image_t.svg";
                g_image_path = "/data/svg.png";
                break;
            case 181:
                g_svg_path = "/data/image_t1.svg";
                g_image_path = "/data/svg.jpg";
                break;
            case 182:
                g_svg_path = "/data/image_t2.svg";
                g_image_path = "/data/svg.png";
                break;
            case 183:
                g_svg_path = "/data/image_t3.svg";
                g_image_path = "/data/svg.png";
                break;
        }
    } else {
        g_svg_path = "/data/tiger.svg";
    };
    printf("Loading SVG file from: %s\n", g_svg_path);
    printf("Loading image file from: %s\n", g_image_path);
    lv_svg_render_init(&hal);
    lv_obj_t * tiger_widget = lv_tiger_create(lv_scr_act());
    lv_obj_set_size(tiger_widget, LV_PCT(100), LV_PCT(100));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#else

void uikit_draw_demo_tigers(char* info[], int size, void* param)
{
    /*fallback for online examples*/
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Vector graphics is not enabled");
    lv_obj_center(label);
}

#endif