/**
 * @file lv_blur_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_blur_test.h"
#include "lvgl/lvgl.h"

/**********************
 *      DEFINES
 *********************/
#define BUF_PADDING_HEIGHT 100

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_buf_t* ori_buf;
    lv_draw_buf_t* blur_buf;
    lv_obj_t* img;
    const char* path;
    int scale_pct;
    int blur_radius;
    uint32_t blur_time;
    lv_obj_t* cont;
} blur_test_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 *********************/

static void update_blur(blur_test_ctx_t* ctx, int scale_pct, int blur_radius);
static void ctrl_cont_create(blur_test_ctx_t* ctx);

/**********************
 *  STATIC VARIABLES
 *********************/

static blur_test_ctx_t g_ctx = { 0 };

/***********************
 *  MACROS
 ***********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int lv_blur_normal_test(int argc, char* argv[])
{
    if (argc < 2) {
        LV_LOG_ERROR("Please specify the image path");
        return -1;
    }

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    g_ctx.img = lv_image_create(lv_screen_active());
    g_ctx.path = argv[2];
    lv_obj_center(g_ctx.img);

    update_blur(&g_ctx, 1000, 0);
    ctrl_cont_create(&g_ctx);
    return 0;
}

int lv_blur_perf_test(int argc, char* argv[])
{
    if (argc < 2) {
        LV_LOG_ERROR("Please specify the image path");
        return -1;
    }

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    g_ctx.img = lv_image_create(lv_screen_active());
    g_ctx.path = argv[2];
    lv_obj_center(g_ctx.img);

    const int repeat_count = 10;

    for (int blur_radius = 0; blur_radius <= 50; blur_radius += 5) {
        uint32_t sum = 0;
        for (int i = 0; i < repeat_count; i++) {
            /* force update blur */
            g_ctx.blur_radius = -1;
            update_blur(&g_ctx, 1000, blur_radius);
            sum += g_ctx.blur_time;
        }

        uint32_t avg_time = sum / repeat_count;
        LV_LOG_USER("blur radius: %dpx, avg time: %" LV_PRIu32 "ms", blur_radius, avg_time);
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_buf_t* snapshot_take(float scale, const char* path)
{
    lv_obj_t* cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(cont);

    lv_obj_t* img = lv_image_create(cont);
    lv_image_set_src(img, path);

    lv_obj_update_layout(img);

    LV_ASSERT_FORMAT_MSG(lv_obj_get_width(img) > 0 && lv_obj_get_height(img) > 0,
        "Invalid image size, please check the image path: %s", path);

    lv_obj_set_size(cont, lv_obj_get_width(img) * scale, lv_obj_get_height(img) * scale);

    lv_obj_set_style_transform_scale(img, LV_SCALE_NONE * scale, 0);

    lv_draw_buf_t* buf = lv_snapshot_take(cont, LV_COLOR_FORMAT_XRGB8888);
    LV_ASSERT_NULL(buf);

    lv_obj_delete(cont);

    return buf;
}

static lv_draw_buf_t* lv_draw_buf_dup_with_padding(const lv_draw_buf_t* draw_buf)
{
    LV_PROFILER_DRAW_BEGIN;
    const lv_image_header_t* header = &draw_buf->header;
    lv_draw_buf_t* new_buf = lv_draw_buf_create(header->w, header->h + BUF_PADDING_HEIGHT * 2, header->cf, header->stride);
    if (new_buf == NULL) {
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    void* padding_top = lv_draw_buf_goto_xy(new_buf, 0, 0);
    lv_memset(padding_top, 0x55, header->stride * BUF_PADDING_HEIGHT);

    void* padding_bottom = lv_draw_buf_goto_xy(new_buf, 0, BUF_PADDING_HEIGHT + header->h);
    lv_memset(padding_bottom, 0x55, header->stride * BUF_PADDING_HEIGHT);

    /* move the data to the center of the new buffer */
    new_buf->data = lv_draw_buf_goto_xy(new_buf, 0, BUF_PADDING_HEIGHT);

    /* update the height */
    new_buf->header.h = header->h;

    LV_PROFILER_DRAW_END;
    return new_buf;
}

static void draw_buf_check_padding(lv_draw_buf_t* draw_buf)
{
    bool has_changed = false;
    const lv_image_header_t* header = &draw_buf->header;
    const uint8_t* padding_top = lv_draw_buf_goto_xy(draw_buf, 0, 0);
    padding_top -= BUF_PADDING_HEIGHT * header->stride;

    size_t padding_size = header->stride * BUF_PADDING_HEIGHT;
    for (size_t i = 0; i < padding_size; i++) {
        if (padding_top[i] != 0x55) {
            LV_LOG_ERROR("Invalid padding top at %d, expected 0x55, got 0x%02x", i, padding_top[i]);
            has_changed = true;
        }
    }

    const uint8_t* padding_bottom = lv_draw_buf_goto_xy(draw_buf, 0, header->h);
    for (size_t i = 0; i < padding_size; i++) {
        if (padding_bottom[i] != 0x55) {
            LV_LOG_ERROR("Invalid padding bottom at %d, expected 0x55, got 0x%02x", i, padding_bottom[i]);
            has_changed = true;
        }
    }
    if (has_changed) {
        LV_LOG_ERROR("FAILED!");
    } else {
        LV_LOG_USER("SUCCESS!");
    }
}

static void update_blur(blur_test_ctx_t* ctx, int scale_pct, int blur_radius)
{
    if (scale_pct != ctx->scale_pct) {
        float scale = scale_pct / 1000.0f;
        LV_LOG_USER("scale: %0.2f", scale);
        lv_draw_buf_t* buf = snapshot_take(scale, ctx->path);

        lv_image_set_scale(ctx->img, LV_SCALE_NONE / scale);

        if (ctx->ori_buf) {
            lv_draw_buf_destroy(ctx->ori_buf);
        }

        ctx->ori_buf = buf;

        if (ctx->blur_buf) {
            lv_draw_buf_destroy(ctx->blur_buf);
        }

        ctx->blur_buf = lv_draw_buf_dup_with_padding(buf);

        lv_image_set_src(ctx->img, ctx->blur_buf);

        ctx->scale_pct = scale_pct;

        ctx->blur_radius = -1;
    }

    if (blur_radius != ctx->blur_radius) {
        lv_draw_buf_blur_args_t args;
        lv_draw_buf_blur_args_init(&args);
        args.radius = blur_radius;
        uint32_t start = lv_tick_get();
        lv_draw_buf_blur(ctx->blur_buf, ctx->ori_buf, &args);
        ctx->blur_time = lv_tick_elaps(start);

        draw_buf_check_padding(ctx->blur_buf);

        lv_obj_invalidate(ctx->img);

        ctx->blur_radius = blur_radius;
    }
}

static void on_scale_changed(lv_event_t* e)
{
    blur_test_ctx_t* ctx = (blur_test_ctx_t*)lv_event_get_user_data(e);
    lv_obj_t* slider = lv_event_get_current_target_obj(e);
    update_blur(ctx, lv_slider_get_value(slider) * 10, ctx->blur_radius);
}

static void on_blur_changed(lv_event_t* e)
{
    blur_test_ctx_t* ctx = (blur_test_ctx_t*)lv_event_get_user_data(e);
    lv_obj_t* slider = lv_event_get_current_target_obj(e);
    update_blur(ctx, ctx->scale_pct, lv_slider_get_value(slider));
}

static void ctrl_slider_value_changed(lv_event_t* e)
{
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    lv_obj_t* slider = lv_event_get_current_target_obj(e);
    lv_label_set_text_fmt(label, "%" LV_PRIu32, lv_slider_get_value(slider));
}

static void on_clicked(lv_event_t* e)
{
    blur_test_ctx_t* ctx = (blur_test_ctx_t*)lv_event_get_user_data(e);
    if (lv_obj_has_flag(ctx->cont, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_remove_flag(ctx->cont, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ctx->cont, LV_OBJ_FLAG_HIDDEN);
    }
}

static lv_obj_t* ctrl_slider_create(lv_obj_t* parent, const char* name, int32_t min, int32_t max, lv_event_cb_t event_cb, void* user_data)
{
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(90), 60);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* slider = lv_slider_create(cont);
    lv_slider_set_range(slider, min, max);
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, user_data);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text_static(label, name);

    label = lv_label_create(cont);
    lv_obj_add_event_cb(slider, ctrl_slider_value_changed, LV_EVENT_VALUE_CHANGED, label);

    return slider;
}

static void ctrl_cont_create(blur_test_ctx_t* ctx)
{
    lv_obj_t* cont = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_50, 0);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    ctx->cont = cont;
    lv_obj_add_flag(ctx->cont, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t* slider = ctrl_slider_create(cont, "Scale(%):", 5, 100, on_scale_changed, ctx);
    lv_slider_set_value(slider, 100, LV_ANIM_OFF);
    lv_obj_send_event(slider, LV_EVENT_VALUE_CHANGED, NULL);

    slider = ctrl_slider_create(cont, "Blur(px):", 0, 100, on_blur_changed, ctx);
    lv_slider_set_value(slider, 0, LV_ANIM_OFF);
    lv_obj_send_event(slider, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_remove_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(lv_layer_top(), on_clicked, LV_EVENT_CLICKED, ctx);
}
