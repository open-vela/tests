/**
 * @file lv_api_test_vector_graphic.c
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
} lv_vector_demo_t;

static int g_caseId;
static lv_obj_t* g_canvas;
static lv_obj_t* g_list;

static void anim_timer_cb(lv_timer_t* timer);
static void vector_demo_constructor(lv_obj_t* obj);
static void vector_demo_destructor(lv_obj_t* obj);
static void vector_demo_event_cb(lv_event_t* e);

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
static void draw_cubic_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 250, 300 }, { 350, 150 } };

    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_cubic_to(path, &pts[1], &pts[2], &pts[3]);

    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_stroke_width(ctx, 8.0f);

    float dashes[] = { 10, 15, 20, 12 };
    lv_vector_dsc_set_stroke_dash(ctx, dashes, 4);

    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_basic_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Test 1: Basic line Different line widths Dashed line
    lv_vector_path_clear(path);
    lv_fpoint_t pts1[] = { { 50, 50 }, { 400, 50 } };
    lv_vector_path_move_to(path, &pts1[0]);
    lv_vector_path_line_to(path, &pts1[1]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_set_stroke_width(ctx, 1.0f);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_fpoint_t pts2[] = { { 50, 80 }, { 400, 80 } };
    lv_vector_path_move_to(path, &pts2[0]);
    lv_vector_path_line_to(path, &pts2[1]);
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_fpoint_t pts3[] = { { 50, 120 }, { 400, 120 } };
    lv_vector_path_move_to(path, &pts3[0]);
    lv_vector_path_line_to(path, &pts3[1]);
    float dashes[] = { 10, 5 };
    lv_vector_dsc_set_stroke_dash(ctx, dashes, 2);
    lv_vector_dsc_add_path(ctx, path);

    /* Test dash_count=3 */
    lv_vector_path_clear(path);
    lv_fpoint_t pts7[] = { { 50, 140 }, { 400, 140 } };
    lv_vector_path_move_to(path, &pts7[0]);
    lv_vector_path_line_to(path, &pts7[1]);
    float dashes3[] = { 10, 5, 5 };
    lv_vector_dsc_set_stroke_dash(ctx, dashes3, 3);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_line_caps(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_set_stroke_width(ctx, 1.0f);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);

    lv_fpoint_t pts4[] = { { 50, 100 }, { 400, 100 } };
    lv_vector_path_move_to(path, &pts4[0]);
    lv_vector_path_line_to(path, &pts4[1]);

    lv_vector_dsc_set_stroke_width(ctx, 10.0f);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_BUTT);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_fpoint_t pts5[] = { { 50, 200 }, { 400, 200 } };
    lv_vector_path_move_to(path, &pts5[0]);
    lv_vector_path_line_to(path, &pts5[1]);

    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_SQUARE);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_fpoint_t pts6[] = { { 50, 300 }, { 400, 300 } };
    lv_vector_path_move_to(path, &pts6[0]);
    lv_vector_path_line_to(path, &pts6[1]);

    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_ROUND);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_line_joins(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_set_stroke_width(ctx, 1.0f);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);

    lv_vector_dsc_set_stroke_width(ctx, 10.0f);

    // Miter join
    lv_fpoint_t miter_pts[] = { { 50, 200 }, { 100, 150 }, { 150, 250 }, { 200, 200 } };
    lv_vector_path_move_to(path, &miter_pts[0]);
    lv_vector_path_line_to(path, &miter_pts[1]);
    lv_vector_path_line_to(path, &miter_pts[2]);
    lv_vector_path_line_to(path, &miter_pts[3]);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_MITER);
    lv_vector_dsc_add_path(ctx, path);

    // Round join
    lv_vector_path_clear(path);
    lv_vector_dsc_set_stroke_width(ctx, 10.0f);

    lv_fpoint_t round_pts[] = { { 50, 300 }, { 100, 250 }, { 150, 350 }, { 200, 300 } };
    lv_vector_path_move_to(path, &round_pts[0]);
    lv_vector_path_line_to(path, &round_pts[1]);
    lv_vector_path_line_to(path, &round_pts[2]);
    lv_vector_path_line_to(path, &round_pts[3]);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_ROUND);
    lv_vector_dsc_add_path(ctx, path);

    // Bevel join
    lv_vector_path_clear(path);
    lv_vector_dsc_set_stroke_width(ctx, 10.0f);

    lv_fpoint_t bevel_pts[] = { { 50, 400 }, { 100, 350 }, { 150, 450 }, { 200, 400 } };
    lv_vector_path_move_to(path, &bevel_pts[0]);
    lv_vector_path_line_to(path, &bevel_pts[1]);
    lv_vector_path_line_to(path, &bevel_pts[2]);
    lv_vector_path_line_to(path, &bevel_pts[3]);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_BEVEL);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_miter_limit_comparison(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t miter_pts1[] = { { 50, 200 }, { 100, 250 }, { 50, 300 } };

    // Default miter limit (4.0)
    lv_vector_path_move_to(path, &miter_pts1[0]);
    lv_vector_path_line_to(path, &miter_pts1[1]);
    lv_vector_path_line_to(path, &miter_pts1[2]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xff, 0x00, 0x00)); // Red
    lv_vector_dsc_set_stroke_width(ctx, 10.0f);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_MITER);
    lv_vector_dsc_set_stroke_miter_limit(ctx, 4.0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_add_path(ctx, path);

    // Small miter limit (1.0)
    lv_vector_path_clear(path);
    lv_fpoint_t miter_pts2[] = { { 150, 200 }, { 200, 250 }, { 150, 300 } };
    lv_vector_path_move_to(path, &miter_pts2[0]);
    lv_vector_path_line_to(path, &miter_pts2[1]);
    lv_vector_path_line_to(path, &miter_pts2[2]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0x00)); // Green
    lv_vector_dsc_set_stroke_miter_limit(ctx, 1.0);
    lv_vector_dsc_add_path(ctx, path);

    // Large miter limit (10.0)
    lv_vector_path_clear(path);
    lv_fpoint_t miter_pts3[] = { { 250, 200 }, { 300, 250 }, { 250, 300 } };
    lv_vector_path_move_to(path, &miter_pts3[0]);
    lv_vector_path_line_to(path, &miter_pts3[1]);
    lv_vector_path_line_to(path, &miter_pts3[2]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0x00, 0xff)); // Blue
    lv_vector_dsc_set_stroke_miter_limit(ctx, 10.0);
    lv_vector_dsc_add_path(ctx, path);
}

static void draw_quad_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 250, 300 }, { 350, 150 } };

    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_quad_to(path, &pts[1], &pts[3]);

    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_stroke_width(ctx, 12.0f);

    float dashes[] = { 10, 15, 20, 12 };
    lv_vector_dsc_set_stroke_dash(ctx, dashes, 4);

    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_arc_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t p1 = { 200, 200 }; /* Center */
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xFF, 0x0, 0x0));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);
    lv_vector_path_append_arc(path, &p1, 100, -90, 90, false);
    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_fpoint_t p2 = { 250, 250 };
    lv_vector_path_append_circle(path, &p2, 100, 60);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_rect_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 50, 50, 100, 200 };
    lv_vector_path_append_rect(path, &rect, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x1e, 0x8F));
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_copy_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 50, 200 } };
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);

    lv_vector_path_t* path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_copy(path2, path);

    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xFF, 0x00, 0x00));
    lv_vector_dsc_add_path(ctx, path2); // draw a path

    lv_vector_path_delete(path2);

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
}

static void draw_append_path(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 50, 200 }, { 200, 50 } };
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);

    lv_vector_path_t* path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[3]);
    lv_vector_path_close(path2);

    lv_vector_path_append_path(path, path2);

    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);

    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xFF, 0x00));

    lv_vector_dsc_add_path(ctx, path); // draw a path

    // lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xFF, 0x00));
    // lv_vector_dsc_add_path(ctx, path2); // draw a path

    lv_vector_path_delete(path2);
}

static void test_draw_fill_only(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 1: Fill only
    lv_vector_path_clear(path);
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_stroke_only(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect2 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect2, 0, 0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_and_stroke(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect3 = { 350, 50, 450, 150 };
    lv_vector_path_append_rect(path, &rect3, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0xff, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_linear_gradient(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 1: Linear gradient
    lv_vector_path_clear(path);
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);

    lv_gradient_stop_t stops[2];
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x0000ff);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_vector_dsc_set_fill_linear_gradient(ctx, 50, 50, 150, 150);
    lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_radial_gradient(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 2: Radial gradient
    lv_area_t rect2 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect2, 0, 0);

    lv_gradient_stop_t stops[2];
    stops[0].color = lv_color_hex(0x00ff00);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;

    stops[1].color = lv_color_hex(0x0000ff);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_vector_dsc_set_fill_radial_gradient(ctx, 250, 100, 50);
    lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_gradient_with_trabsparency(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 3: Gradient with transparency
    lv_area_t rect3 = { 350, 50, 450, 150 };
    lv_vector_path_append_rect(path, &rect3, 0, 0);

    lv_gradient_stop_t stops[2];
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_50;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x0000ff);
    stops[1].opa = LV_OPA_50;
    stops[1].frac = 255;

    lv_vector_dsc_set_fill_linear_gradient(ctx, 350, 50, 450, 150);
    lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_stroke_linear_gradient(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Common gradient stops
    lv_gradient_stop_t stops[2] = {
        { .color = lv_color_hex(0xFF0000), .opa = LV_OPA_COVER, .frac = 0 },
        { .color = lv_color_hex(0x0000FF), .opa = LV_OPA_COVER, .frac = 255 }
    };

    /* Test linear gradient with all spread types in one snapshot */
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);

    // Left: PAD spread
    lv_area_t linear_rect1 = { 50, 50, 100, 150 };
    lv_vector_path_append_rect(path, &linear_rect1, 10, 10);
    lv_vector_dsc_set_stroke_linear_gradient(ctx, 50, 50, 100, 100);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_vector_dsc_set_stroke_width(ctx, 30.0f);
    lv_vector_dsc_add_path(ctx, path);

    // Middle: REPEAT spread
    lv_vector_path_clear(path);
    lv_area_t linear_rect2 = { 160, 50, 210, 150 };
    lv_vector_path_append_rect(path, &linear_rect2, 10, 10);
    lv_vector_dsc_set_stroke_linear_gradient(ctx, 160, 50, 210, 100);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REPEAT);
    lv_vector_dsc_add_path(ctx, path);

    // Right: REFLECT spread
    lv_vector_path_clear(path);
    lv_area_t linear_rect3 = { 270, 50, 320, 150 };
    lv_vector_path_append_rect(path, &linear_rect3, 10, 10);
    lv_vector_dsc_set_stroke_linear_gradient(ctx, 270, 50, 320, 100);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REFLECT);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_stroke_radial_gradient(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Common gradient stops
    lv_gradient_stop_t stops[2] = {
        { .color = lv_color_hex(0xFF0000), .opa = LV_OPA_COVER, .frac = 0 },
        { .color = lv_color_hex(0x0000FF), .opa = LV_OPA_COVER, .frac = 255 }
    };

    /* Test radial gradient with all spread types in one snapshot */
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);

    // Top: PAD spread
    lv_area_t radial_rect1 = { 50, 200, 100, 250 };
    lv_vector_path_append_rect(path, &radial_rect1, 10, 10);
    lv_vector_dsc_set_stroke_radial_gradient(ctx, 100, 250, 40);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_vector_dsc_set_stroke_width(ctx, 30.0f);
    lv_vector_dsc_add_path(ctx, path);

    // Middle: REPEAT spread
    lv_vector_path_clear(path);
    lv_area_t radial_rect2 = { 160, 200, 210, 250 };
    lv_vector_path_append_rect(path, &radial_rect2, 10, 10);
    lv_vector_dsc_set_stroke_radial_gradient(ctx, 210, 250, 40);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REPEAT);
    lv_vector_dsc_add_path(ctx, path);

    // Bottom: REFLECT spread
    lv_vector_path_clear(path);
    lv_area_t radial_rect3 = { 270, 200, 320, 250 };
    lv_vector_path_append_rect(path, &radial_rect3, 10, 10);
    lv_vector_dsc_set_stroke_radial_gradient(ctx, 320, 250, 40);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REFLECT);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_basic_pattern(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 1: Basic pattern
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);

    // Create checkerboard pattern

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    img_dsc.header = test_image_cogwheel_argb8888.header;
    img_dsc.src = &test_image_cogwheel_argb8888;

    lv_vector_dsc_set_fill_image(ctx, &img_dsc);

    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_image_fill_with_transform(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_area_t rect2 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect2, 0, 0);

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    img_dsc.header = test_image_cogwheel_argb8888.header;
    img_dsc.src = &test_image_cogwheel_argb8888;

    lv_vector_dsc_set_fill_image(ctx, &img_dsc);

    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);

    lv_matrix_t mt;
    lv_matrix_identity(&mt);
    lv_matrix_scale(&mt, 0.5f, 0.5f);
    lv_vector_dsc_set_fill_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_units(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);

    /* Test USERSPACE units */
    lv_vector_dsc_set_fill_units(ctx, LV_VECTOR_FILL_UNITS_USER_SPACE_ON_USE);

    /* Draw with USERSPACE units */
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);
    img_dsc.header = test_image_cogwheel_argb8888.header;
    img_dsc.src = &test_image_cogwheel_argb8888;
    lv_vector_dsc_set_fill_image(ctx, &img_dsc);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_add_path(ctx, path);

    /* Test BOUNDING_BOX units */
    lv_vector_path_clear(path);
    lv_vector_dsc_set_fill_units(ctx, LV_VECTOR_FILL_UNITS_OBJECT_BOUNDING_BOX);

    /* Draw with BOUNDING_BOX units */
    lv_area_t rect2 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect2, 0, 0);
    lv_vector_dsc_set_fill_image(ctx, &img_dsc);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_fill_rounded_rect(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_vector_dsc_set_fill_color32(ctx, lv_color32_make(0x00, 0xff, 0x00, 0xff)); // RGBA format
    lv_vector_dsc_set_stroke_color32(ctx, lv_color32_make(0xff, 0x00, 0x00, 0xff));

    /* Test stroke transform */
    lv_matrix_t stroke_xform;
    lv_matrix_identity(&stroke_xform);
    lv_matrix_rotate(&stroke_xform, 45.0f);
    lv_vector_dsc_set_stroke_transform(ctx, &stroke_xform);

    /* Test path operations */
    lv_area_t rect = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect, 10, 10);
    lv_vector_path_is_empty(path);

    /* Draw and snapshot */
    lv_vector_dsc_set_stroke_width(ctx, 3.0f);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_shapes_group(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test 1: Rectangle
    lv_vector_path_clear(path);
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);

    // Test 2: Rounded rectangle
    lv_vector_path_clear(path);
    lv_area_t rect2 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect2, 20, 20);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    // Test 3: Circle
    lv_vector_path_clear(path);
    lv_fpoint_t center = { 100, 250 };
    lv_vector_path_append_circle(path, &center, 50, 50);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_add_path(ctx, path);

    // Test 3: polygon
    lv_fpoint_t polygon[] = {
        { 200, 200 },
        { 300, 200 },
        { 350, 250 },
        { 300, 300 },
        { 200, 300 }
    };

    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &polygon[0]);
    for (int i = 1; i < 5; i++) {
        lv_vector_path_line_to(path, &polygon[i]);
    }
    lv_vector_path_close(path);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_set_stroke_width(ctx, 2.0f);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_combined_shapes(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    lv_vector_path_t* path1 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_t* path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Create first path (rectangle)
    lv_area_t rect1 = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path1, &rect1, 0, 0);

    // Create second path (circle)
    lv_fpoint_t center = { 150, 150 };
    lv_vector_path_append_circle(path2, &center, 100, 100);

    // Combine paths
    lv_vector_path_append_path(path1, path2);

    // Draw combined path
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x80, 0x80));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path1);

    lv_vector_path_delete(path1);
    lv_vector_path_delete(path2);

    // Test 3: Arc
    lv_vector_path_clear(path);
    lv_fpoint_t center1 = { 200, 150 };
    lv_vector_path_append_arc(path, &center1, 50, 0, 270, false);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_black());
    lv_vector_dsc_add_path(ctx, path);

    // Test 4: Combined path
    lv_vector_path_clear(path);
    lv_area_t rect2 = { 350, 50, 450, 150 };
    lv_vector_path_append_rect(path, &rect2, 0, 0);
    lv_fpoint_t circle_center = { 400, 200 };
    lv_vector_path_append_circle(path, &circle_center, 50, 50);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0xff, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_fill_rule(ctx, LV_VECTOR_FILL_EVENODD);
    lv_vector_dsc_add_path(ctx, path);

    /* Test pie chart (arc with pie=true) */
    lv_vector_path_clear(path);
    lv_fpoint_t pie_center = { 80, 350 };
    lv_vector_path_append_arc(path, &pie_center, 50, 30, 120, true);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    /* Test full circle pie (360 degrees) */
    lv_vector_path_clear(path);
    lv_fpoint_t pie_center2 = { 200, 350 };
    lv_vector_path_append_arc(path, &pie_center2, 60, 0, 360, true);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    /* Test combined rectangle and pie */
    lv_vector_path_clear(path);
    lv_area_t combined_rect = { 350, 300, 450, 400 };
    lv_vector_path_append_rect(path, &combined_rect, 0, 0);
    lv_fpoint_t combined_pie_center = { 400, 350 };
    lv_vector_path_append_arc(path, &combined_pie_center, 30, 45, 270, true);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_complex_fill_rules(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Test : Combined paths with different fill rules
    lv_vector_path_clear(path);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0x80, 0x80));
    lv_vector_dsc_set_stroke_width(ctx, 3.0f);
    lv_area_t rect1 = { 200, 50, 300, 150 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);
    lv_fpoint_t circle_center = { 250, 150 };
    lv_vector_path_append_circle(path, &circle_center, 50, 50);

    // Test even-odd rule
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x80, 0x00, 0x80));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_fill_rule(ctx, LV_VECTOR_FILL_EVENODD);
    lv_vector_dsc_add_path(ctx, path);

    // Test non-zero rule
    lv_vector_dsc_translate(ctx, 0, 200);
    lv_vector_dsc_set_fill_rule(ctx, LV_VECTOR_FILL_NONZERO);
    lv_vector_dsc_add_path(ctx, path);
}

static void test_draw_blend_modes_comparison(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Draw 14 blend mode examples in 4x4 grid within {0,0,640,480} area
    const int unit_width = 70;
    const int unit_height = 70;
    const int cols = 4;
    const int rows = 4;
    const int total_units = 14;

    // Calculate spacing between units
    const int h_space = (640 - cols * unit_width * 3 / 2) / (cols + 1);
    const int v_space = (480 - rows * unit_height * 3 / 2) / (rows + 1);

    // Blend modes for each unit
    const lv_vector_blend_t blend_modes[] = {
        LV_VECTOR_BLEND_SRC_OVER,
        LV_VECTOR_BLEND_SCREEN,
        LV_VECTOR_BLEND_MULTIPLY,
        LV_VECTOR_BLEND_NONE,
        LV_VECTOR_BLEND_ADDITIVE,
        LV_VECTOR_BLEND_DARKEN,
        LV_VECTOR_BLEND_LIGHTEN,
        LV_VECTOR_BLEND_HARDLIGHT,
        LV_VECTOR_BLEND_SOFTLIGHT,
        LV_VECTOR_BLEND_OVERLAY,
        LV_VECTOR_BLEND_COLORBURN,
        LV_VECTOR_BLEND_COLORDODGE,
        LV_VECTOR_BLEND_DIFFERENCE,
        LV_VECTOR_BLEND_EXCLUSION
    };

    // Draw blend mode examples
    int unit_count = 0;
    for (int row = 0; row < rows && unit_count < total_units; row++) {
        for (int col = 0; col < cols && unit_count < total_units; col++) {
            // Calculate unit position
            int x = h_space + col * (unit_width * 3 / 2 + h_space);
            int y = v_space + row * (unit_height * 3 / 2 + v_space);

            // Draw blue rectangle (120x120 with 1/4 overlap)
            lv_vector_path_clear(path);
            lv_area_t blue_rect = {
                x + unit_width / 2,
                y + unit_width / 2,
                x + unit_width * 3 / 2,
                y + unit_width * 3 / 2
            };
            lv_vector_path_append_rect(path, &blue_rect, 0, 0);
            lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff)); // Blue
            lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
            lv_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SRC_OVER);
            lv_vector_dsc_add_path(ctx, path);

            // Draw green rectangle (full size)
            lv_vector_path_clear(path);
            lv_area_t green_rect = { x, y, x + unit_width, y + unit_height };
            lv_vector_path_append_rect(path, &green_rect, 0, 0);
            lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00)); // Green
            lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
            lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
            lv_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SRC_OVER);
            lv_vector_dsc_add_path(ctx, path);

            lv_vector_path_clear(path);
            lv_vector_path_append_rect(path, &blue_rect, 0, 0);
            lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff)); // Blue
            lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
            lv_vector_dsc_set_blend_mode(ctx, blend_modes[unit_count]);
            lv_vector_dsc_add_path(ctx, path);

            unit_count++;
        }
    }
}

static void test_draw_transforms(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Base shape (rectangle)
    lv_area_t rect1 = { 50, 50, 100, 100 };
    lv_vector_path_append_rect(path, &rect1, 0, 0);

    // Original shape
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_set_transform(ctx, &matrix);
    lv_vector_dsc_add_path(ctx, path);

    // Translated
    lv_matrix_t mt;
    lv_matrix_identity(&mt);
    lv_matrix_translate(&mt, 150, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_set_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);

    // Rotated
    lv_matrix_identity(&mt);
    lv_matrix_translate(&mt, 300, 0);
    lv_matrix_rotate(&mt, 45);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_set_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);

    // Scaled
    lv_matrix_identity(&mt);
    lv_matrix_translate(&mt, 0, 150);
    lv_matrix_scale(&mt, 1.5f, 0.5f);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0xff, 0x00));
    lv_vector_dsc_set_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);

    // Combined transforms
    lv_matrix_identity(&mt);
    lv_matrix_translate(&mt, 150, 150);
    lv_matrix_rotate(&mt, 30);
    lv_matrix_scale(&mt, 1.2f, 1.2f);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0xff));
    lv_vector_dsc_set_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);

    /* Verify bounds */
    lv_area_t bounds;
    lv_vector_path_get_bounding(path, &bounds);
}

static void test_draw_matrix_operations(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    // Clear background
    lv_area_t rect = { 0, 0, 640, 480 };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    // Base rectangle
    lv_area_t base_rect = { 50, 50, 150, 150 };
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x80, 0x80));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
    lv_vector_dsc_add_path(ctx, path);

    // Scale transform
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_scale(ctx, 1.5f, 0.8f);
    lv_vector_dsc_translate(ctx, 200, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    // Rotate transform
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_rotate(ctx, 45.0f);
    lv_vector_dsc_translate(ctx, 400, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    // Translate transform
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_translate(ctx, 200, 200);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_add_path(ctx, path);

    // Skew transform
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_skew(ctx, 15.0f, 10.0f);
    lv_vector_dsc_translate(ctx, 400, 200);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0xff));
    lv_vector_dsc_add_path(ctx, path);

    // Combined transforms
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &base_rect, 0, 0);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_translate(ctx, 300, 300);
    lv_vector_dsc_rotate(ctx, 30.0f);
    lv_vector_dsc_scale(ctx, 1.2f, 0.8f);
    lv_vector_dsc_skew(ctx, 10.0f, 5.0f);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0xff, 0x00));
    lv_vector_dsc_add_path(ctx, path);
}

static void test_error_handling_null_params(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    /* Test invalid path operations */
    lv_fpoint_t point = { 0, 0 };
    lv_area_t rect = { 0, 0, 100, 100 };

    lv_vector_path_line_to(path, &point);
    lv_vector_path_quad_to(path, &point, &point);
    lv_vector_path_cubic_to(path, &point, &point, &point);
    lv_vector_path_close(path);
    lv_vector_path_append_rect(path, &rect, 0, 0);
    lv_vector_path_append_circle(path, &point, 20, 20);

    /* Test invalid dsc operations */
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);

    lv_vector_dsc_add_path(ctx, path);
    LV_LOG_USER("test_error_handling_null_params: Done");
}

static void test_error_handing_invalid_values(lv_vector_dsc_t* ctx, lv_vector_path_t* path)
{
    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);

    /* Test invalid stroke width */
    lv_vector_dsc_set_stroke_width(ctx, -1.0f);
    lv_vector_dsc_set_stroke_width(ctx, 1000.0f);

    /* Test invalid dash pattern */

    float invalid_dash[] = { -1.0f, 0.0f };
    lv_vector_dsc_set_stroke_dash(ctx, invalid_dash, 2);

    /* Test invalid path points */

    lv_vector_path_move_to(path, &(lv_fpoint_t) { NAN, NAN });
    lv_vector_path_line_to(path, &(lv_fpoint_t) { INFINITY, INFINITY });

    lv_vector_dsc_add_path(ctx, path);
    LV_LOG_USER("test_error_handing_invalid_values: Done");
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

static void draw_vector(lv_layer_t* layer);

static void vector_demo_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_DRAW_MAIN) {
        lv_layer_t* layer = lv_event_get_layer(e);
        draw_vector(layer);
    }
}

static void draw_vector(lv_layer_t* layer)
{
    lv_vector_dsc_t* ctx = lv_vector_dsc_create(layer);
    lv_vector_path_t* path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    // Clear background
    lv_area_t rect = { 0, 0, lv_area_get_width(&layer->_clip_area), lv_area_get_height(&layer->_clip_area) };
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    switch (g_caseId) {
    case 1:
        draw_append_path(ctx, path);
        break;
    case 2:
        draw_arc_path(ctx, path);
        break;
    case 3:
        draw_basic_path(ctx, path);
        break;
    case 4:
        test_draw_blend_modes_comparison(ctx, path);
        break;
    case 5:
        test_draw_combined_shapes(ctx, path);
        break;
    case 6:
        test_draw_complex_fill_rules(ctx, path);
        break;
    case 7:
        draw_copy_path(ctx, path);
        break;
    case 8:
        draw_cubic_path(ctx, path);
        break;
    case 9:
        test_draw_fill_and_stroke(ctx, path);
        break;
    case 10:
        test_draw_fill_linear_gradient(ctx, path);
        break;
    case 11:
        test_draw_fill_only(ctx, path);
        break;
    case 12:
        test_draw_fill_basic_pattern(ctx, path);
        break;
    case 13:
        test_draw_image_fill_with_transform(ctx, path);
        break;
    case 14:
        test_draw_fill_rounded_rect(ctx, path);
        break;
    case 15:
        test_draw_fill_gradient_with_trabsparency(ctx, path);
        break;
    case 16:
        test_draw_line_caps(ctx, path);
        break;
    case 17:
        test_draw_line_joins(ctx, path);
        break;
    case 18:
        test_draw_miter_limit_comparison(ctx, path);
        break;
    case 19:
        draw_quad_path(ctx, path);
        break;
    case 20:
        test_draw_fill_radial_gradient(ctx, path);
        break;
    case 21:
        draw_rect_path(ctx, path);
        break;
    case 22:
        test_draw_shapes_group(ctx, path);
        break;
    case 23:
        test_draw_stroke_linear_gradient(ctx, path);
        break;
    case 24:
        test_draw_stroke_only(ctx, path);
        break;
    case 25:
        test_draw_stroke_radial_gradient(ctx, path);
        break;
    case 26:
        test_draw_matrix_operations(ctx, path);
        break;
    case 27:
        test_draw_transforms(ctx, path);
        break;
    case 28:
        test_error_handling_null_params(ctx, path);
        break;
    case 29:
        test_error_handing_invalid_values(ctx, path);
        break;
    case 30:
        test_draw_fill_units(ctx, path);
        break;
    default:
        LV_LOG_ERROR("Error g_caseId: %d", g_caseId);
        break;
    }

    lv_draw_vector(ctx);

    lv_vector_path_delete(path);
    lv_vector_dsc_delete(ctx);
}

static void delete_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_draw_buf_t* draw_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_destroy(draw_buf);
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
        if (strcmp("test draw append path", txt) == 0) {
            g_caseId = 1;
        } else if (strcmp("test draw arc path", txt) == 0) {
            g_caseId = 2;
        } else if (strcmp("test draw basic path", txt) == 0) {
            g_caseId = 3;
        } else if (strcmp("test draw blend modes comparison", txt) == 0) {
            g_caseId = 4;
        } else if (strcmp("test draw combined shapes", txt) == 0) {
            g_caseId = 5;
        } else if (strcmp("test draw complex fill rules", txt) == 0) {
            g_caseId = 6;
        } else if (strcmp("test draw copy path", txt) == 0) {
            g_caseId = 7;
        } else if (strcmp("test draw cubic path", txt) == 0) {
            g_caseId = 8;
        } else if (strcmp("test draw fill and stroke", txt) == 0) {
            g_caseId = 9;
        } else if (strcmp("test draw fill linear gradient", txt) == 0) {
            g_caseId = 10;
        } else if (strcmp("test draw fill only", txt) == 0) {
            g_caseId = 11;
        } else if (strcmp("test draw fill basic pattern", txt) == 0) {
            g_caseId = 12;
        } else if (strcmp("test draw image fill with transform", txt) == 0) {
            g_caseId = 13;
        } else if (strcmp("test draw fill rounded rect", txt) == 0) {
            g_caseId = 14;
        } else if (strcmp("test draw fill gradient with trabsparency", txt) == 0) {
            g_caseId = 15;
        } else if (strcmp("test draw line caps", txt) == 0) {
            g_caseId = 16;
        } else if (strcmp("test draw line joins", txt) == 0) {
            g_caseId = 17;
        } else if (strcmp("test draw miter limit comparison", txt) == 0) {
            g_caseId = 18;
        } else if (strcmp("test draw quad path", txt) == 0) {
            g_caseId = 19;
        } else if (strcmp("test draw fill radial gradient", txt) == 0) {
            g_caseId = 20;
        } else if (strcmp("test draw rect path", txt) == 0) {
            g_caseId = 21;
        } else if (strcmp("test draw shapes group", txt) == 0) {
            g_caseId = 22;
        } else if (strcmp("test draw stroke linear gradient", txt) == 0) {
            g_caseId = 23;
        } else if (strcmp("test draw stroke only", txt) == 0) {
            g_caseId = 24;
        } else if (strcmp("test draw stroke radial gradient", txt) == 0) {
            g_caseId = 25;
        } else if (strcmp("test draw matrix operations", txt) == 0) {
            g_caseId = 26;
        } else if (strcmp("test draw transforms", txt) == 0) {
            g_caseId = 27;
        } else if (strcmp("test error handling null params", txt) == 0) {
            g_caseId = 28;
        } else if (strcmp("test error handing invalid values", txt) == 0) {
            g_caseId = 29;
        } else if (strcmp("test draw fill units", txt) == 0) {
            g_caseId = 30;
        } else {
            LV_LOG_USER("error case: %s", txt);
        }

        LV_LOG_USER("g_caseId: %d", g_caseId);

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
    lv_list_add_text(list, "vector api cases");
    btn = lv_list_add_button(list, LV_SYMBOL_FILE, "test draw append path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_DIRECTORY, "test draw arc path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_TRASH, "test draw basic path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_CHARGE, "test draw blend modes comparison");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_WIFI, "test draw combined shapes");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_GPS, "test draw complex fill rules");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_USB, "test draw copy path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BATTERY_FULL, "test draw cubic path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BELL, "test draw fill and stroke");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BLUETOOTH, "test draw fill linear gradient");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_GPS, "test draw fill only");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_WIFI, "test draw fill basic pattern");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_USB, "test draw image fill with transform");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BATTERY_3, "test draw fill rounded rect");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_CHARGE, "test draw fill gradient with trabsparency");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BELL, "test draw line caps");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_GPS, "test draw line joins");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_WIFI, "test draw miter limit comparison");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BLUETOOTH, "test draw quad path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_GPS, "test draw fill radial gradient");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_USB, "test draw rect path");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BATTERY_3, "test draw shapes group");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_CHARGE, "test draw stroke linear gradient");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BELL, "test draw stroke only");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_GPS, "test draw stroke radial gradient");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_USB, "test draw matrix operations");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BATTERY_3, "test draw transforms");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_CHARGE, "test error handling null params");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BELL, "test error handing invalid values");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list);
    btn = lv_list_add_button(list, LV_SYMBOL_BELL, "test draw fill units");
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

void lv_api_test_vector_graphic(void* param)
{
    (void)param;
    testcase_list_create();

    lv_draw_buf_t* draw_buf = lv_draw_buf_create(WIDTH, HEIGHT, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    lv_draw_buf_clear(draw_buf, NULL);

    lv_obj_t* canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_draw_buf(canvas, draw_buf);

    lv_vector_demo_t* demo = (lv_vector_demo_t*)lv_malloc(sizeof(lv_vector_demo_t));
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

#endif
