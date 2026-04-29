/**
 * @file rive_api_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define USE_ANIM_VIEW_PLAYER 1
#if USE_ANIM_VIEW_PLAYER
#include <uikit/uikit_anim_view_player.h>
#endif
#include "rive_api_test.h"
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <uv.h>
/*********************
 *      DEFINES
 *********************/

#define SHOW_SIZE 70
#define SHOW_MAX_COUNT_PER_LINE 3 // max count of rive files in one line, must be positive
#define DROP_PAD 10

#define PASS 1
#define FAIL 0

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t* rive_screen;
    lv_obj_t* main_page;
    lv_obj_t* rive_picker;
    lv_obj_t* show_page;
    lv_obj_t* translatex_dropdown;
    lv_obj_t* translatey_dropdown;
    lv_obj_t* scale_dropdown;
    lv_obj_t* rotate_dropdown;
    lv_style_t style_transform;
    const char* rive_path;
    lv_ll_t rive_files_list;
    int selected_count;
    uv_loop_t* loop;
} rive_demo_ctx_t;

typedef struct {
    rive_demo_ctx_t* ctx;
    const char* name;
    bool selected;
} rive_file_item_t;

typedef void(walk_dir_cb)(struct dirent* ent, void* userdata);

/**********************
 * GLOBAL VARIABLES
 **********************/
int frame = 0;
int action =0;
float test_number_value = 0.0;
bool test_bool_value = false;
int32_t test_width = 50;
int32_t test_height = 50;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_show_button(rive_demo_ctx_t* ctx);
static void create_rive_screen(rive_demo_ctx_t* ctx);
static void create_rive_picker(rive_demo_ctx_t* ctx);
static void create_transform_picker(rive_demo_ctx_t* ctx);
static void rive_show_handler(lv_event_t* e);
static void rive_reload_handler(lv_event_t* e);
static void rive_test_handler(lv_event_t* e);
static void rive_register_handler(lv_event_t* e);
static void rive_unregister_handler(lv_event_t* e);
static void rive_item_event_handler(lv_event_t* e);
static void parse_rive_list(rive_demo_ctx_t* ctx);
static void rive_demo_parse_cmd(rive_demo_ctx_t* ctx, char* info[], int size);
static int walk_dir(const char* dirname, walk_dir_cb cb, void* userdata);
static void append_rive_file(struct dirent* ent, void* userdata);
static void rive_transform_style_update(rive_demo_ctx_t* ctx);
static int read_rive_file(const char* file_path, uint8_t** rive_data, uint32_t* size);
static void rive_player_create(rive_demo_ctx_t* ctx, const char* filename);
static void rive_testplayer_create(rive_demo_ctx_t* ctx, const char* filename);
static void create_rive_list(rive_demo_ctx_t* ctx);
static void refresh_rive_list(rive_demo_ctx_t* ctx);

/**********************
 *  STATIC VARIABLES
 **********************/


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
//int anim_rive_api_test(int argc, char *agrv[])
void anim_rive_api_test(char* info[], int size, void* params)
{
    printf("input anim_rive_api_test function \n");
    //test_api_function();
    
    //int size;
    //void* params;
    //char* info[];

    //initialize
    rive_demo_ctx_t* ctx = lv_malloc(sizeof(rive_demo_ctx_t));
    lv_memzero(ctx, sizeof(rive_demo_ctx_t));
    _lv_ll_init(&ctx->rive_files_list, sizeof(rive_file_item_t));
    lv_style_init(&ctx->style_transform);

    ctx->loop = (uv_loop_t*)params;
    // parse command line
    rive_demo_parse_cmd(ctx, info, size);

    // create rive screen
    create_rive_screen(ctx);

    // create transform picker
    create_transform_picker(ctx);

    // create rive picker
    create_rive_picker(ctx);

    // create show button
    create_show_button(ctx);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void rive_demo_parse_cmd(rive_demo_ctx_t* ctx, char* info[], int size)
{
    char ch;
    while ((ch = getopt(size, info, "hd:")) != -1) {
        switch (ch) {
        case 'd':
            if (optarg) {
                ctx->rive_path = optarg;
            }
            break;
        case 'h':
            LV_LOG("\nUsage:  animdemo %s [-h] -d <dir>\n", info[0]);
            LV_LOG("-h             help\n");
            LV_LOG("-d <dir>       the directory that you want to open\n");
            break;
        default:
            return;
        }
    }
}

static int walk_dir(const char* dirname, walk_dir_cb cb, void* userdata)
{
    DIR* dir;
    struct dirent* ent;

    dir = opendir(dirname);
    if (!dir) {
        /* could not open directory */
        LV_LOG_ERROR("opendir failed!");
        return -1;
    }

    /* print all the files and directories within directory */
    ent = readdir(dir);

    while (ent) {
        if (ent->d_type == DT_REG && cb)
            cb(ent, userdata);
        ent = readdir(dir);
    }

    closedir(dir);
    return 0;
}

static void append_rive_file(struct dirent* ent, void* userdata)
{
    if (lv_strcmp(lv_fs_get_ext(ent->d_name), "riv") != 0) {
        LV_LOG_USER("Skipping non-rive file %s", ent->d_name);
        return;
    }

    LV_LOG_USER("Appending rive to list %s", ent->d_name);
    rive_demo_ctx_t* ctx = (rive_demo_ctx_t*)userdata;

    rive_file_item_t* rive_file_item = _lv_ll_ins_tail(&ctx->rive_files_list);
    rive_file_item->name = strdup(ent->d_name);
    rive_file_item->selected = false;
    rive_file_item->ctx = ctx;

    if (!rive_file_item->name) {
        LV_LOG_ERROR("OOM: failed to dup rive name");
    }
}

static void parse_rive_list(rive_demo_ctx_t* ctx)
{
    /* access to the file systems and list all the rive files */

    _lv_ll_clear(&ctx->rive_files_list);

    if (ctx->rive_path == NULL) {
        ctx->rive_path = "/data/rive";
    }

    int ret = walk_dir(ctx->rive_path, append_rive_file, ctx);
    if (ret < 0) {
        LV_LOG_ERROR("Failed to walk dir");
        return;
    }
}

static void rive_transform_style_update(rive_demo_ctx_t* ctx)
{
    if (ctx->style_transform.prop_cnt > 0) {
        lv_style_reset(&ctx->style_transform);
    }

    // get transform and set style
    char translatex_buf[16];
    lv_dropdown_get_selected_str(ctx->translatex_dropdown, translatex_buf, sizeof(translatex_buf));
    if (atoi(translatex_buf) != 0) {
        lv_style_set_translate_x(&ctx->style_transform, (int32_t)atoi(translatex_buf));
        LV_LOG_USER("translatex: %s", translatex_buf);
    }

    char translatey_buf[16];
    lv_dropdown_get_selected_str(ctx->translatey_dropdown, translatey_buf, sizeof(translatey_buf));
    if (atoi(translatey_buf) != 0) {
        lv_style_set_translate_y(&ctx->style_transform, (int32_t)atoi(translatey_buf));
        LV_LOG_USER("translatey: %s", translatey_buf);
    }

    char scale_buf[16];
    lv_dropdown_get_selected_str(ctx->scale_dropdown, scale_buf, sizeof(scale_buf));
    int32_t scale_value = (int32_t)(atof(scale_buf) * LV_SCALE_NONE);
    if (scale_value != LV_SCALE_NONE) {
        lv_style_set_transform_scale_x(&ctx->style_transform, scale_value);
        lv_style_set_transform_scale_y(&ctx->style_transform, scale_value);
        LV_LOG_USER("scale: %s", scale_buf);
    }

    char rotate_buf[16];
    lv_dropdown_get_selected_str(ctx->rotate_dropdown, rotate_buf, sizeof(rotate_buf));
    if (atoi(rotate_buf) != 0) {
        lv_style_set_transform_rotation(&ctx->style_transform, (int32_t)atoi(rotate_buf));
        LV_LOG_USER("rotate: %s", rotate_buf);
    }
}

static int read_rive_file(const char* file_path, uint8_t** rive_data, uint32_t* size)
{
    lv_fs_file_t file;
    uint32_t br;

    /* open rive file */
    lv_fs_res_t res = lv_fs_open(&file, file_path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to open file: %s", file_path);
        return -1;
    }

    /* get rive file size */
    lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    res = lv_fs_tell(&file, size);

    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("can't get file size");
        goto failed;
    }

    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    *rive_data = lv_malloc(*size);
    LV_ASSERT_MALLOC(*rive_data);
    if (!*rive_data) {
        LV_LOG_ERROR("OOM: failed to allocate memory for rive data");
        goto failed;
    }

    /* read rive file */
    res = lv_fs_read(&file, *rive_data, *size, &br);
    if (res != LV_FS_RES_OK || br != *size) {
        LV_LOG_ERROR("read file failed");
        goto failed;
    }
    lv_fs_close(&file);
    return 0;

failed:
    lv_fs_close(&file);

    if (*rive_data) {
        lv_free(*rive_data);
        *rive_data = NULL;
    }

    return -1;
}

void callback1(lv_obj_t* obj, const char* name, void* user_data)
{
    printf("vg_anim_player_foreach_text_names_api_Call Successful %s\n", name);
}

void callback2(lv_obj_t* obj, const char* name, void* user_data)
{
    printf("vg_anim_player_foreach_input_names_api_Call Successful %s\n", name);
}

void callback3(int callback_action)
{
    if(callback_action == 0)
    {
        printf("action_api_now is  VG_ANIM_PLAYER_PLAY\n");
    }else if(callback_action == 1)
    {
        printf("action_api_now is  VG_ANIM_PLAYER_PAUSE\n");
    }else if(callback_action == 2)
    {
        printf("action_api_now is  VG_ANIM_PLAYER_RESUME\n");
    }else if(callback_action == 3)
    {
        printf("action_api_now is  VG_ANIM_PLAYER_RESET\n");
    }
}

void text_update(lv_timer_t* tmr)
{
    lv_obj_t* player = lv_timer_get_user_data(tmr);

    static char* g_usernames[5] = { "Jay Chou", "Eason Chan", "Jay Chou", "Angela Chang", "Faye Wong" };
    static char* g_messages[5] = { "Qi Li Xiang", "Elevation", "Dandelion's Promise", "Invisible Wings", "April Days in the World" };

    static int g_index = 0;
    //const char* str = NULL;

    //g_index = (++g_index) % 5;
    int temp = g_index % 5;
    g_index = temp;


    vg_anim_player_set_text_value(player, "text.username", g_usernames[g_index]);
    vg_anim_player_set_text_value(player, "text.messag", g_messages[g_index]);

    //test_vg_anim_player_foreach_text_names api
    vg_anim_player_foreach_text_names(player, callback1, NULL);
    vg_anim_player_foreach_input_names(player, callback2, NULL);


    //test set_text_value && get_text_value api
    printf("vg_anim_player_set_text_value(player, \"text.username\") = %s\n", g_usernames[g_index]);
    const char* str1 = vg_anim_player_get_text_value(player, "text.username");
    printf("input_right_name_parameter_vg_anim_player_get_text_value(player, \"text.username\") = %s\n", str1);
    const char* str2 = vg_anim_player_get_text_value(player, "text.user");
    printf("input_error_name_parameter_vg_anim_player_get_text_value(player, \"text.username\") = %s\n", str2);
    

    //VG_ANIM_PLAYER_PLAY, VG_ANIM_PLAYER_PAUSE, VG_ANIM_PLAYER_RESUME, VG_ANIM_PLAYER_STOP
    //test vg_anim_player_action_api
    printf("*************test_vg_anim_player_action_api_start************************* \n");
    if( frame >= 10)
    {
        //boundary 480*480
        vg_anim_player_resize(player,test_width,test_height);
        test_width = test_width + 100;
        test_width = test_width % 480;
        test_height = test_height + 100;
        test_height = test_height % 480;


        vg_anim_player_action(player, action);//action function is not run
        action++;
        action = action % 4;
        frame = 0;
        callback3(action);

        //test vg_anim_player_set_input_number && input_bool && input_trigger_api
        if(test_number_value != 100)
        {
            vg_anim_player_set_input_number(player, "input.number", test_number_value);
        }
        if(test_bool_value == false)    
        {
            vg_anim_player_set_input_bool(player, "input.bool", true);
        }
        vg_anim_player_set_input_trigger(player, "input.trigger");

    }
    frame++;
    g_index++;

    
}

static void on_player_event_cb(vg_anim_view_event_t* event, void* user_data)
{
    printf("Modify register&unregister event API\n");
}

static void rive_register_handler(lv_event_t* e)
{
    lv_obj_t* player = lv_event_get_user_data(e);

    vg_anim_view_event_listener_t listener = {
        .cb = on_player_event_cb,
        .user_data = player,
    };
    vg_anim_player_register_event(player, VG_VIEW_EVENT_GENERAL, &listener);
    vg_anim_player_register_event(player, VG_VIEW_EVENT_OPEN_URL, &listener);
}

static void rive_unregister_handler(lv_event_t* e)
{
    lv_obj_t* player = lv_event_get_user_data(e);

    vg_anim_view_event_listener_t listener = {
        .cb = on_player_event_cb,
        .user_data = player,
    };
    vg_anim_player_unregister_event(player, VG_VIEW_EVENT_GENERAL, &listener);
    vg_anim_player_unregister_event(player, VG_VIEW_EVENT_OPEN_URL, &listener);


}

static void on_delete_timer(lv_event_t* e)
{
    lv_timer_t* timer = lv_event_get_user_data(e);
    lv_timer_delete(timer);

    lv_obj_t* player = lv_event_get_current_target(e);

    vg_anim_view_event_listener_t listener = {
        .cb = on_player_event_cb,
        .user_data = player,
    };
    vg_anim_player_unregister_event(player, VG_VIEW_EVENT_GENERAL, &listener);
    vg_anim_player_unregister_event(player, VG_VIEW_EVENT_OPEN_URL, &listener);
}

static void rive_player_create(rive_demo_ctx_t* ctx, const char* filename)
{
    char file_path[128];
    uint32_t data_size = 0;
    uint8_t* rive_data = NULL;

    lv_snprintf(file_path, sizeof(file_path), "%s/%s", ctx->rive_path, filename);
    LV_LOG_USER(" read begin");

    if (read_rive_file(file_path, &rive_data, &data_size) < 0) {
        LV_LOG_ERROR("Failed to read rive file");
        return;
    }
    LV_LOG_USER(" read end");

#if USE_ANIM_VIEW_PLAYER
    lv_obj_t* player = vg_anim_player_create(ctx->show_page);

    vg_anim_player_set_src_data(player, rive_data, data_size, "/data/rive/");
    vg_anim_player_set_topic_loop(player, ctx->loop);

    //register event
    lv_obj_t* register_btn = lv_btn_create(player);
    lv_obj_set_size(register_btn, lv_pct(12), lv_pct(12));
    lv_obj_add_event_cb(register_btn, rive_register_handler, LV_EVENT_CLICKED, player);

    lv_obj_t* label = lv_label_create(register_btn);
    lv_label_set_text(label, "register");
    lv_obj_center(label);
    lv_obj_align_to(register_btn, player, LV_ALIGN_LEFT_MID, 0, 0);

    //unregister event
    lv_obj_t* unregister_btn = lv_btn_create(player);
    lv_obj_set_size(unregister_btn, lv_pct(12), lv_pct(12));
    lv_obj_add_event_cb(unregister_btn, rive_unregister_handler, LV_EVENT_CLICKED, player);

    label = lv_label_create(unregister_btn);
    lv_label_set_text(label, "unregist");
    lv_obj_center(label);
    lv_obj_align_to(unregister_btn, register_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // timer for update
    lv_timer_t* timer = lv_timer_create(text_update, 1000, player);
    lv_timer_set_user_data(timer, player);
    lv_obj_add_event_cb(player, on_delete_timer, LV_EVENT_DELETE, timer);

#else
    lv_obj_t* player = lv_obj_create(ctx->show_page);
#endif

    int32_t size = (int32_t)(100 / SHOW_MAX_COUNT_PER_LINE);
    if (ctx->selected_count > 0 && ctx->selected_count < SHOW_MAX_COUNT_PER_LINE) {
        size = (int32_t)(100 / ctx->selected_count);
    }
    lv_obj_set_size(player, lv_pct(size), lv_pct(size));
    // lv_obj_set_style_outline_width(player, 2, 0);
    // lv_obj_set_style_outline_color(player, lv_color_black(), 0);
    // lv_obj_set_style_outline_opa(player, 255, 0);

    if (ctx->style_transform.prop_cnt > 0) {
        lv_obj_add_style(player, &ctx->style_transform, 0);
    }
}

static void rive_testplayer_create(rive_demo_ctx_t* ctx, const char* filename)
{
    char file_path[128];
    lv_snprintf(file_path, sizeof(file_path), "%s/%s", ctx->rive_path, filename);

#if USE_ANIM_VIEW_PLAYER
    lv_obj_t* player = vg_anim_player_create(ctx->show_page);
    printf("file_path = %s\n", file_path);
    vg_anim_player_set_src_file(player, file_path, "/data/rive/");
    vg_anim_player_set_topic_loop(player, ctx->loop);
#else
    lv_obj_t* player = lv_obj_create(ctx->show_page);
#endif

    int32_t size = (int32_t)(100 / SHOW_MAX_COUNT_PER_LINE);
    if (ctx->selected_count > 0 && ctx->selected_count < SHOW_MAX_COUNT_PER_LINE) {
        size = (int32_t)(100 / ctx->selected_count);
    }

    lv_obj_set_size(player, lv_pct(size), lv_pct(size));

    if (ctx->style_transform.prop_cnt > 0) {
        lv_obj_add_style(player, &ctx->style_transform, 0);
    }
}

static void show_page_return_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    rive_demo_ctx_t* ctx = lv_event_get_user_data(e);
    if (code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_RIGHT) {
            if (ctx->show_page == NULL)
                return;
            lv_obj_clear_flag(ctx->main_page, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ctx->rive_screen, LV_OBJ_FLAG_GESTURE_BUBBLE);
            lv_obj_delete(ctx->show_page);
            ctx->show_page = NULL;
            lv_obj_invalidate(ctx->rive_screen);
        }
    }
}

static void rive_show_handler(lv_event_t* e)
{
    rive_demo_ctx_t* ctx = lv_event_get_user_data(e);
    rive_transform_style_update(ctx);

    ctx->show_page = lv_obj_create(ctx->rive_screen);
    lv_obj_remove_style_all(ctx->show_page);
    lv_obj_set_size(ctx->show_page, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(ctx->show_page, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_add_flag(ctx->show_page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(ctx->main_page, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(ctx->rive_screen, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ctx->rive_screen, show_page_return_handler, LV_EVENT_ALL, ctx);

    lv_ll_t* l = &ctx->rive_files_list;
    rive_file_item_t* cur = _lv_ll_get_head(l);
    rive_file_item_t* next = NULL;

    if (cur == NULL) {
        LV_LOG_USER("No rive file found");
        return;
    }

    while (cur) {
        if (cur->selected) {
            LV_LOG_USER("Playing rive file: %s", cur->name);
            rive_player_create(ctx, cur->name);
        }
        next = _lv_ll_get_next(l, cur);
        cur = next;
    }

    if (ctx->selected_count == 0) {
        LV_LOG_USER("No rive file selected");
        return;
    }
}

static void rive_test_handler(lv_event_t* e)
{
    rive_demo_ctx_t* ctx = lv_event_get_user_data(e);
    rive_transform_style_update(ctx);

    ctx->show_page = lv_obj_create(ctx->rive_screen);
    lv_obj_remove_style_all(ctx->show_page);
    lv_obj_set_size(ctx->show_page, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(ctx->show_page, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_add_flag(ctx->show_page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(ctx->main_page, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(ctx->rive_screen, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ctx->rive_screen, show_page_return_handler, LV_EVENT_ALL, ctx);

    lv_ll_t* l = &ctx->rive_files_list;
    rive_file_item_t* cur = _lv_ll_get_head(l);
    rive_file_item_t* next = NULL;

    if (cur == NULL) {
        LV_LOG_USER("No rive file found");
        return;
    }

    while (cur) {
        if (cur->selected) {
            LV_LOG_USER("Playing rive file: %s", cur->name);
            rive_testplayer_create(ctx, cur->name);
        }
        next = _lv_ll_get_next(l, cur);
        cur = next;
    }

    if (ctx->selected_count == 0) {
        LV_LOG_USER("No rive file selected");
        return;
    }
}

static void rive_reload_handler(lv_event_t* e)
{
    rive_demo_ctx_t* ctx = lv_event_get_user_data(e);
    refresh_rive_list(ctx);
}

static void rive_item_event_handler(lv_event_t* e)
{
    lv_obj_t* item = lv_event_get_target(e);
    rive_file_item_t* rive_file_item = lv_event_get_user_data(e);

    if (lv_obj_has_state(item, LV_STATE_CHECKED)) {
        rive_file_item->selected = true;
        rive_file_item->ctx->selected_count++;
    } else {
        rive_file_item->selected = false;
        rive_file_item->ctx->selected_count--;
    }
}

static void create_rive_list(rive_demo_ctx_t* ctx)
{

    rive_file_item_t* cur = _lv_ll_get_head(&ctx->rive_files_list);
    rive_file_item_t* next = NULL;
    lv_obj_t* item = NULL;

    while (cur) {
        item = lv_checkbox_create(ctx->rive_picker);
        lv_checkbox_set_text(item, cur->name);
        lv_obj_add_event_cb(item, rive_item_event_handler, LV_EVENT_CLICKED, cur);

        next = _lv_ll_get_next(&ctx->rive_files_list, cur);
        cur = next;
    }
}

static void create_transform_picker(rive_demo_ctx_t* ctx)
{
    // line one
    // translate
    lv_obj_t* transform_picker_one = lv_obj_create(ctx->main_page);
    lv_obj_remove_style_all(transform_picker_one);
    lv_obj_set_size(transform_picker_one, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t* label = lv_label_create(transform_picker_one);
    lv_label_set_text(label, "Translate X & Y");
    ctx->translatex_dropdown = lv_dropdown_create(transform_picker_one);
    lv_obj_set_style_pad_all(ctx->translatex_dropdown, DROP_PAD, 0);
    lv_obj_set_size(ctx->translatex_dropdown, 100, LV_SIZE_CONTENT);
    lv_dropdown_set_options(ctx->translatex_dropdown, "0\n10\n20\n30\n40\n50");

    lv_obj_align_to(label, transform_picker_one, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(ctx->translatex_dropdown, label, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
    ctx->translatey_dropdown = lv_dropdown_create(transform_picker_one);
    lv_obj_set_style_pad_all(ctx->translatey_dropdown, DROP_PAD, 0);
    lv_obj_set_size(ctx->translatey_dropdown, 100, LV_SIZE_CONTENT);
    lv_dropdown_set_options(ctx->translatey_dropdown, "0\n10\n20\n30\n40\n50");
    lv_obj_align_to(ctx->translatey_dropdown, ctx->translatex_dropdown, LV_ALIGN_OUT_RIGHT_TOP, 2, 0);

    // line two
    // scale
    lv_obj_t* transform_picker_two = lv_obj_create(ctx->main_page);
    lv_obj_remove_style_all(transform_picker_two);
    lv_obj_set_size(transform_picker_two, lv_pct(100), LV_SIZE_CONTENT);
    label = lv_label_create(transform_picker_two);
    lv_label_set_text(label, "Scale");
    ctx->scale_dropdown = lv_dropdown_create(transform_picker_two);
    lv_dropdown_set_options(ctx->scale_dropdown, "1\n0.5\n2\n4\n");
    lv_obj_set_style_pad_all(ctx->scale_dropdown, DROP_PAD, 0);
    lv_obj_set_size(ctx->scale_dropdown, 100, LV_SIZE_CONTENT);
    lv_obj_align_to(label, transform_picker_two, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(ctx->scale_dropdown, label, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);

    // rotate
    label = lv_label_create(transform_picker_two);
    lv_label_set_text(label, "Rotate");
    ctx->rotate_dropdown = lv_dropdown_create(transform_picker_two);
    lv_dropdown_set_options(ctx->rotate_dropdown, "0\n50\n100\n300\n1200\n1800\n2700\n3600\n");
    lv_obj_set_style_pad_all(ctx->rotate_dropdown, DROP_PAD, 0);
    lv_obj_set_size(ctx->rotate_dropdown, 100, LV_SIZE_CONTENT);
    lv_obj_align_to(label, ctx->scale_dropdown, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_obj_align_to(ctx->rotate_dropdown, label, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
}

static void refresh_rive_list(rive_demo_ctx_t* ctx)
{
    if (lv_obj_get_child_count(ctx->rive_picker) > 0) {
        lv_obj_clean(ctx->rive_picker);
        ctx->selected_count = 0;
    }
    parse_rive_list(ctx);
    create_rive_list(ctx);
}

static void create_rive_picker(rive_demo_ctx_t* ctx)
{
    lv_obj_t* title = lv_label_create(ctx->main_page);
    lv_label_set_text(title, "Files Selection");

    ctx->rive_picker = lv_obj_create(ctx->main_page);

    lv_obj_set_size(ctx->rive_picker, lv_pct(100), lv_pct(60));
    lv_obj_add_flag(ctx->rive_picker, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_grow(ctx->rive_picker, 3);
    lv_obj_set_flex_flow(ctx->rive_picker, LV_FLEX_FLOW_COLUMN);
    refresh_rive_list(ctx);
}

static void create_rive_screen(rive_demo_ctx_t* ctx)
{
    ctx->rive_screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ctx->rive_screen, lv_pct(100), lv_pct(100));

    ctx->main_page = lv_obj_create(ctx->rive_screen);
    lv_obj_remove_style_all(ctx->main_page);
    lv_obj_set_size(ctx->main_page, lv_pct(SHOW_SIZE), lv_pct(SHOW_SIZE));
    lv_obj_set_flex_flow(ctx->main_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(ctx->main_page);
}

static void create_show_button(rive_demo_ctx_t* ctx)
{
    lv_obj_t* obj = lv_obj_create(ctx->main_page);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(15));

    lv_obj_t* show_btn = lv_btn_create(obj);
    lv_obj_set_size(show_btn, lv_pct(20), lv_pct(60));
    lv_obj_add_event_cb(show_btn, rive_show_handler, LV_EVENT_CLICKED, ctx);

    lv_obj_t* label = lv_label_create(show_btn);
    lv_label_set_text(label, "Show");
    lv_obj_center(label);
    lv_obj_align_to(show_btn, obj, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* reload_btn = lv_btn_create(obj);
    lv_obj_set_size(reload_btn, lv_pct(20), lv_pct(60));
    lv_obj_add_event_cb(reload_btn, rive_reload_handler, LV_EVENT_CLICKED, ctx);
    lv_obj_center(reload_btn);

    label = lv_label_create(reload_btn);
    lv_label_set_text(label, "Reload");
    lv_obj_center(label);
    lv_obj_align_to(reload_btn, show_btn, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t* test_btn = lv_btn_create(obj);
    lv_obj_set_size(test_btn, lv_pct(20), lv_pct(60));
    lv_obj_add_event_cb(test_btn, rive_test_handler, LV_EVENT_CLICKED, ctx);

    label = lv_label_create(test_btn);
    lv_label_set_text(label, "Test");
    lv_obj_center(label);
    lv_obj_align_to(test_btn, reload_btn, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

}