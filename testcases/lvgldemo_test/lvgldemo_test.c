/**
 * @file lvgldemo_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgldemo_test.h"
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
#include <uv.h>
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_DEMOS_COUNT (sizeof(demos_entry_info) / sizeof(demo_entry_info_t) - 1)

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*demo_method_cb)(void* param);

typedef struct {
    const char* name;
    demo_method_cb entry_cb;
} demo_entry_info_t;

static bool lvgldemo_test_create(char* info[], int size);
static void lvgldemo_test_show_help(void);
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
static void* lv_nuttx_uv_loop_init(uv_loop_t* loop, lv_nuttx_result_t* result);
static void lv_nuttx_uv_loop_run(uv_loop_t* loop, void* data);
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const demo_entry_info_t demos_entry_info[] = {

#if CONFIG_VECTOR_GRAPHIC_TEST
    { "vector_api_test", .entry_cb = lv_api_test_vector_graphic },
    { "vector_performance_test", .entry_cb = lv_quality_test_vector_graphic },
#endif

    { "", .entry_cb = NULL }
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
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

bool lvgldemo_test_create(char* info[], int size)
{
    const int demos_count = LV_DEMOS_COUNT;

    if (demos_count <= 0) {
        LV_LOG_ERROR("Please enable some lv_demos firstly!");
        return false;
    }

    const demo_entry_info_t* entry_info = NULL;
    if (size <= 0) { /* default: first demo*/
        entry_info = &demos_entry_info[0];
    } else if (entry_info == NULL && info) {
        const char* name = info[0];
        for (int i = 0; i < demos_count; i++) {
            if (lv_strcmp(name, demos_entry_info[i].name) == 0) {
                entry_info = &demos_entry_info[i];
            }
        }
    }

    if (entry_info == NULL) {
        LV_LOG_ERROR("lv_demos create(%s) failure!", size > 0 && info ? info[0] : "");
        return false;
    }

    if (entry_info->entry_cb) {
        int iterations = atoi(info[1]);
        entry_info->entry_cb(&iterations);
        return true;
    }

    return false;
}

void lvgldemo_test_show_help(void)
{
    int i;
    const int demos_count = LV_DEMOS_COUNT;

    if (demos_count == 0) {
        LV_LOG("lv_demos: no demo available!\n");
        return;
    }

    LV_LOG("\nUsage: lv_demos demo [parameters]\n");
    LV_LOG("\ndemo list:\n");

    for (i = 0; i < demos_count; i++) {
        LV_LOG("     %s \n", demos_entry_info[i].name);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char* argv[])
{
    lv_nuttx_dsc_t info;
    lv_nuttx_result_t result;

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    uv_loop_t ui_loop = { .data = NULL };
#endif

    lv_init();

    lv_nuttx_dsc_init(&info);
    lv_nuttx_init(&info, &result);

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    void* data = lv_nuttx_uv_loop_init(&ui_loop, &result);
#endif

    if (result.disp == NULL) {
        LV_LOG_ERROR("lv_demos initialization failure!");
        return 1;
    }

    if (!lvgldemo_test_create(argv + 1, argc - 1)) {
        lvgldemo_test_show_help();

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
