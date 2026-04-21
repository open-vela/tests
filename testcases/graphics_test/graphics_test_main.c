/*
 * @file graphics_test_main.c
 *
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "graphics_test.h"
#include <string.h>
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
#include <uv.h>
#endif

/****************************************************************************
 * Defines
 ****************************************************************************/
#define TESTCASES_COUNT (sizeof(tests_entry_info) / sizeof(test_entry_info_t) - 1)

/****************************************************************************
 *      TYPEDEFS
 ****************************************************************************/

typedef void (*void_method_cb)(char* info[], int size, void* params);
typedef void (*void_demo_method_cb)(void);
typedef int (*int_method_cb)(int argc, char* argv[]);

typedef struct
{
    const char* name;
    union {
        void_method_cb void_cb;
        void_demo_method_cb void_cb_parameterless;
        int_method_cb int_cb;
    } entry_cb;
} test_entry_info_t;

static bool graphics_test_create(char* info[], int size, void* params);
static void graphics_test_show_help(void);
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
static void* lv_nuttx_uv_loop_init(uv_loop_t* loop, lv_nuttx_result_t* result);
static void lv_nuttx_uv_loop_run(uv_loop_t* loop, void* data);
#endif

/****************************************************************************
 *  Static variables
 ****************************************************************************/
static const test_entry_info_t tests_entry_info[] = {
#ifdef CONFIG_ANIMATION_ENGINE_TEST
    { "animengine_api_test", .entry_cb.int_cb = animengine_api_test },
    { "animengine_specification_test", .entry_cb.int_cb = animengine_specification_test },
#endif

#ifdef CONFIG_ANIMATION_ENGINE_CTEST
    { "animengine_c_api_test", .entry_cb.int_cb = animengine_c_api_test },
    { "animengine_c_normal_test", .entry_cb.int_cb = animengine_c_normal_test },
    { "animengine_c_images_test", .entry_cb.int_cb = animengine_c_images_test },
    { "animengine_c_specification_test", .entry_cb.int_cb = animengine_c_specification_test },
#endif

#ifdef CONFIG_ANIM_PHYSICS_ENGINE_TEST
    { "physics", .entry_cb.int_cb = anim_physics_engine_test },
    { "gravity", .entry_cb.int_cb = anim_physics_gravity_test },
    { "body", .entry_cb.int_cb = anim_physics_body_test },
    { "distance_joint", .entry_cb.int_cb = anim_physics_distance_joint_test },
    { "collision", .entry_cb.int_cb = anim_physics_collision_test },
    { "restitution", .entry_cb.int_cb = anim_physics_restitution_test },
    { "friction", .entry_cb.int_cb = anim_physics_friction_test },
    { "material", .entry_cb.int_cb = anim_physics_material_test },
    { "fmaterial", .entry_cb.int_cb = anim_physics_first_material },
    { "revolute", .entry_cb.int_cb = anim_physics_revolute_joint_test },
    { "create", .entry_cb.int_cb = anim_physics_create_test },
#endif

#ifdef CONFIG_RIVE_TEST
    { "rive_test", .entry_cb.void_cb = anim_rive_api_test },
#endif

#ifdef CONFIG_GESTURES_TEST
    {"gestures_test", .entry_cb.void_cb_parameterless = test_example_gestures},
#endif

#ifdef CONFIG_LV_BLUR_TEST
    {"blur_normal_test", .entry_cb.int_cb = lv_blur_normal_test},
    {"blur_perf_test", .entry_cb.int_cb = lv_blur_perf_test},
#endif

    { "", .entry_cb.int_cb = NULL }

};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
static void* lv_nuttx_uv_loop_init(uv_loop_t* loop, lv_nuttx_result_t* result)
{
    lv_nuttx_uv_t uv_info;
    lv_memset(&uv_info, 0, sizeof(uv_info));

    uv_loop_init(loop);
    uv_info.loop = loop;
    uv_info.disp = result->disp;
    uv_info.indev = result->indev;
#ifdef CONFIG_UINPUT_TOUCH
    uv_info.uindev = result->utouch_indev;
#endif

    return lv_nuttx_uv_init(&uv_info);
}

static void lv_nuttx_uv_loop_run(uv_loop_t* loop, void* data)
{
    if (data == NULL) {
        return;
    }
    uv_run(loop, UV_RUN_DEFAULT);
    lv_nuttx_uv_deinit(&data);
}
#endif

bool graphics_test_create(char* info[], int size, void* params)
{
    const int count = TESTCASES_COUNT;

    if (count <= 0) {
        syslog(LOG_ERR, "Please enable some testcases firstly!");
        return false;
    }

    const test_entry_info_t* entry_info = NULL;
    if (size <= 0) { /* default: first demo*/
        entry_info = &tests_entry_info[0];
    } else if (entry_info == NULL && info) {
        const char* name = info[1];
        for (int i = 0; i < count; i++) {
            if (lv_strcmp(name, tests_entry_info[i].name) == 0) {
                entry_info = &tests_entry_info[i];
                break;
            }
        }
    }

    if (entry_info == NULL) {
        syslog(LOG_ERR, "graphics_test create(%s) failure!", size > 0 ? info[0] : "");
        return false;
    }

    if (strcmp(entry_info->name, "rive_test") == 0) {
        syslog(LOG_INFO, "Using void_cb");
        entry_info->entry_cb.void_cb(info, size, params);
        return true;
    } else {
        syslog(LOG_INFO, "Using int_cb");
        entry_info->entry_cb.int_cb(size, info);
        return true;
    }

    return false;
}

void graphics_test_show_help(void)
{
    int i;
    const int count = TESTCASES_COUNT;

    if (count == 0) {
        syslog(LOG_ERR, "graphics_test: no testcase available!\n");
        return;
    }

    syslog(LOG_INFO, "\nUsage: graphics_test [parameters]\n");
    syslog(LOG_INFO, "\ntestcases list:\n");

    for (i = 0; i < count; i++) {
        syslog(LOG_INFO, "     %s \n", tests_entry_info[i].name);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char* argv[])
{
    lv_nuttx_dsc_t info;
    lv_nuttx_result_t result;
    void* params = NULL;

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    uv_loop_t ui_loop = { .data = NULL };
#endif

    lv_init();

    lv_nuttx_dsc_init(&info);
    lv_nuttx_init(&info, &result);

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    void* data = lv_nuttx_uv_loop_init(&ui_loop, &result);
    params = &ui_loop;
#endif

    if (result.disp == NULL) {
        LV_LOG_ERROR("lv_demos initialization failure!");
        return 1;
    }

    if (!graphics_test_create(argv, argc - 1, params)) {
        graphics_test_show_help();

        /* we can add custom demos here */

        goto demo_end;
    }

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    lv_nuttx_uv_loop_run(&ui_loop, data);
#else
    while (1) {
        lv_timer_handler();
        usleep(10 * 1000);
    }
#endif

demo_end:
    lv_disp_remove(result.disp);
    lv_deinit();

    return 0;
}