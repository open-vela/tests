
/**
 * @file markdown_test.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "uikit/uikit.h"

#if CONFIG_MARKDOWN_TEST

#include "markdown_test.h"
#include <cmark-gfm.h>
#include <latexmath.h>
#include <lvgl/src/lvgl_private.h>
#include <table.h>

static lv_font_t* fonts[6] = { 0 };

static const char* markdown_txt = "\n"
                                  "___\n"
                                  "# 标题验证\n"
                                  "\n"
                                  "# 未添加嵌套标题"
                                  "\n"
                                  "# 段落嵌套标题"
                                  "\n"
                                  "\n"
                                  "# 段落嵌套标题+标题嵌套 _斜体_"
                                  "\n"
                                  "\n"
                                  "### 段落嵌套标题+**标题嵌套粗体**"
                                  "\n"
                                  "\n"
                                  "###### 段落嵌套标题+~~标题嵌套删除线~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 段落验证\n"
                                  "\n"
                                  "\n"
                                  "不添加嵌套段落1"
                                  "\n"
                                  "\n"
                                  "不添加嵌套段落2\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 斜体验证\n"
                                  "\n"
                                  "_未添加嵌套斜体_"
                                  "\n"
                                  "_段落嵌套斜体_"
                                  "\n"
                                  "\n"
                                  "_段落嵌套斜体_+ _多次_ 定义 _斜体_"
                                  "\n"
                                  "\n"
                                  "_段落嵌套斜体+**斜体嵌套粗体**_"
                                  "\n"
                                  "\n"
                                  "_段落嵌套斜体+~~斜体嵌套删除线~~_\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 粗体验证\n"
                                  "\n"
                                  "**不添加嵌套粗体**"
                                  "\n"
                                  "**段落嵌套粗体**"
                                  "\n"
                                  "\n"
                                  "**段落嵌套粗体**+**多次**定义**粗体**"
                                  "\n"
                                  "\n"
                                  "**段落嵌套粗体+~~粗体嵌套删除线~~**"
                                  "\n"
                                  "\n"
                                  "**段落嵌套粗体+ _粗体嵌套斜体_**\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 删除线验证\n"
                                  "\n"
                                  "~~不填加嵌套删除线~~"
                                  "\n"
                                  "~~段落嵌套删除线~~"
                                  "\n"
                                  "\n"
                                  "~~段落嵌套删除线~~+~~多次~~定义~~删除线~~"
                                  "\n"
                                  "\n"
                                  "~~段落嵌套删除线+**删除线嵌套粗体**~~"
                                  "\n"
                                  "\n"
                                  "段落嵌套~~删除线+ _删除线嵌套斜体_~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 无序列表验证1\n"
                                  "\n"
                                  "- 不添嵌套，无序列表写法1"
                                  "\n"
                                  "- 段落嵌套无序列表写法1"
                                  "\n"
                                  "+ 不添嵌套，无序列表写法2"
                                  "\n"
                                  "+ 段落嵌套无序列表写法2"
                                  "\n"
                                  "* 不添嵌套，无序列表写法3"
                                  "\n"
                                  "* 段落嵌套无序列表写法3"
                                  "\n"
                                  "\n"
                                  "- 段落嵌套无序列表+ _无序列表嵌套斜体1_ \n"
                                  "    - 多级无序列表\n"
                                  "    - 多级无序列表\n"
                                  "    - 多级无序列表\n"
                                  "\n"
                                  "\n"
                                  "+ 段落嵌套无序列表+ _无序列表嵌套斜体2_"
                                  "\n"
                                  "\n"
                                  "* 段落嵌套无序列表+ _无序列表嵌套斜体3_"
                                  "\n"
                                  "\n"
                                  "- 段落嵌套无序列表+**无序列表嵌套粗体1**"
                                  "\n"
                                  "\n"
                                  "+ 段落嵌套无序列表+**无序列表嵌套粗体2**"
                                  "\n"
                                  "\n"
                                  "* 段落嵌套无序列表+**无序列表嵌套粗体3**"
                                  "\n"
                                  "\n"
                                  "- 段落嵌套无序列表+~~无序列表嵌套删除线1~~"
                                  "\n"
                                  "\n"
                                  "+ 段落嵌套无序列表+~~无序列表嵌套删除线2~~"
                                  "\n"
                                  "\n"
                                  "* 段落嵌套无序列表+~~无序列表嵌套删除线3~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 有序列表验证1\n"
                                  "\n"
                                  "1. 不添加嵌套，有序列表"
                                  "\n"
                                  "1. 段落嵌套有序列表"
                                  "\n"
                                  "\n"
                                  "1. 段落嵌套有序列表+ _有序列表嵌套斜体_\n"
                                  "    1. 多级有序列表\n"
                                  "    2. 多级有序列表\n"
                                  "    3. 多级有序列表\n"
                                  "\n"
                                  "\n"
                                  "1. 段落嵌套有序列表+**有序列表嵌套粗体**"
                                  "\n"
                                  "\n"
                                  "1. 段落嵌套有序列表+~~有序列表嵌套删除线~~"
                                  "\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 无序列表验证2\n"
                                  "\n"
                                  "- 多个无序列表"
                                  "\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 有序列表验证2\n"
                                  "\n"
                                  "1. 多个有序列表\n"
                                  "1. 多个有序列表\n"
                                  "1. 多个有序列表\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 链接验证\n"
                                  "\n"
                                  "[](https://www.baidu.com/)不添加嵌套链接/不添加自定义\n"
                                  "\n"
                                  "[百度](https://www.baidu.com/)段落嵌套链接/添加自定义为文本\n"
                                  "\n"
                                  "[百度](https://www.baidu.com/)不添加嵌套链接/添加自定义为文本\n"
                                  "\n"
                                  "[段落嵌套链接+ _链接嵌套斜体_](https://www.baidu.com/)\n"
                                  "\n"
                                  "\n"
                                  "[段落嵌套链接+**链接嵌套粗体**](https://www.baidu.com/)"
                                  "\n"
                                  "\n"
                                  "[段落嵌套链接+~~链接嵌删除线~~](https://www.baidu.com/)\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 图验证\n"
                                  "\n"
                                  "![](/data/markdown.png)有效url不添加嵌套图片/不添加自定义\n"
                                  "![](/data/markdown.png)![](/data/markdown.png)![](/data/markdown.png)有效url不添加嵌套图片/不添加自定义+多次定义图片\n"
                                  "\n"
                                  "![我的图片](/data/markdown.png)有效url段落嵌套图片/添加自定义文本"
                                  "\n"
                                  "\n"
                                  "![我的图片](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![我的图片+无效+ _图片嵌套斜体_](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![我的图片+无效+**图片嵌套粗体**](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![我的图片+无效+~~图片嵌套删除线~~](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 分割线验证\n"
                                  "\n"
                                  "___"
                                  "___"
                                  "\n"
                                  "___"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 多层级验证\n"
                                  "\n"
                                  "\n"
                                  "_markdown编辑器，**给您所见即所得的，~~编辑体验~~**_"
                                  "\n"
                                  "\n"
                                  "_markdown编辑器，~~给您所见即所得的，**编辑体验**~~_"
                                  "\n"
                                  "\n"
                                  "**markdown编辑器，_给您所见即所得的，~~编辑体验~~_**"
                                  "\n"
                                  "\n"
                                  "**markdown编辑器，~~给您所见即所得的，_编辑体验_~~**"
                                  "\n"
                                  "\n"
                                  "~~markdown编辑器，**给您所见即所得的，_编辑体验_**~~"
                                  "\n"
                                  "\n"
                                  "~~markdown编辑器，_给您所见即所得的，**编辑体验**_~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# 其他验证\n"
                                  "\n"
                                  "\n"
                                  "```"
                                  "shell:~$ sudo apt-get install libgtest-dev"
                                  "```"
                                  "\n"
                                  ">注释代码"
                                  "\n"
                                  "   |Header| \n"
                                  "   |------| \n"
                                  "   |Hello |"
                                  "\n"
                                  "\n"
                                  "   |Header| \n"
                                  "   |------| \n"
                                  "   |Hello |"
                                  "\n"
                                  "\n"
                                  "`aaaaaaaaaaaaaaaaaaaaaa`"
                                  "\n"
                                  "\n"
                                  "$a+b-c$"
                                  "\n"
                                  "\n"
                                  "$a+b-c$"
                                  "\n"
                                  "\n"
                                  " $leq$, $geq$  $arcsin x, arccos x, arctan x, arccot x$  $iint, iint_D$  $alpha, beta, gamma$  "
                                  "\n"
                                  "\n"
                                  " $leq$, $geq$  $arcsin x, arccos x, arctan x, arccot x$  $iint, iint_D$  $alpha, beta, gamma$  "
                                  "\n";

static void markdown_heading_style_cb(lv_style_t* style, int32_t level);
static void markdown_paragraph_style_cb(lv_style_t* style);
static void markdown_thematic_break_style_cb(lv_style_t* style);
static void markdown_text_deco_style_cb(lv_style_t* style, vg_markdown_decor_t decor);
static void markdown_list_marker_cb(char* buff, uint32_t size, int32_t level, int32_t index, bool ordered);
// static void markdown_list_marker_width(lv_obj_t * obj, int32_t width);
static void markdown_url_create_cb(lv_span_t* span, const char* src, const char* title, const char* alt);
static lv_obj_t* markdown_image_create_cb(lv_obj_t* parent, const char* src, const char* title, const char* alt, int32_t width_hint);
static void markdown_unsupported_cb(char* buff, uint32_t size, lv_style_t* style, int32_t type);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void timer_cb(lv_timer_t* timer)
{
    lv_obj_t* obj = lv_timer_get_user_data(timer);

    static uint32_t ofs = 0;
    const uint32_t str_len = strlen(markdown_txt);

    uint32_t count = lv_rand(2, 10);
    while (count-- > 0) {
        if (lv_text_encoded_next(markdown_txt, &ofs) == 0) {
            lv_timer_delete(timer);
            break;
        }
    }

    ofs = (ofs > str_len) ? str_len : ofs;

    vg_markdown_set_data(obj, markdown_txt, ofs);
    const int32_t bottom = lv_obj_get_scroll_bottom(obj);
    if (bottom > 0) {
        lv_obj_scroll_by(obj, 0, -bottom, LV_ANIM_OFF);
    }
}

// MiSans-Regular.ttf
void test_uikit_markdown(char* info[], int size, void* param)
{
    fonts[0] = vg_font_create("MiSans-Regular", 48, LV_FREETYPE_FONT_STYLE_NORMAL);
    fonts[1] = vg_font_create("MiSans-Regular", 32, LV_FREETYPE_FONT_STYLE_NORMAL);
    fonts[2] = vg_font_create("MiSans-Regular", 24, LV_FREETYPE_FONT_STYLE_NORMAL);
    fonts[3] = vg_font_create("MiSans-Regular", 22, LV_FREETYPE_FONT_STYLE_NORMAL);
    fonts[4] = vg_font_create("MiSans-Regular", 18, LV_FREETYPE_FONT_STYLE_NORMAL);
    fonts[5] = vg_font_create("MiSans-Regular", 16, LV_FREETYPE_FONT_STYLE_NORMAL);

    lv_obj_t* markdown_widget = vg_markdown_create(lv_screen_active());
    vg_markdown_set_heading_style_cb(markdown_widget, markdown_heading_style_cb);
    vg_markdown_set_paragraph_style_cb(markdown_widget, markdown_paragraph_style_cb);
    vg_markdown_set_thematic_break_style_cb(markdown_widget, markdown_thematic_break_style_cb);
    vg_markdown_set_text_deco_style_cb(markdown_widget, markdown_text_deco_style_cb);
    vg_markdown_set_list_marker_width(markdown_widget, 100);
    vg_markdown_set_list_marker_cb(markdown_widget, markdown_list_marker_cb);
    vg_markdown_set_url_create_cb(markdown_widget, markdown_url_create_cb);
    vg_markdown_set_image_create_cb(markdown_widget, markdown_image_create_cb);
    vg_markdown_set_unsupported_cb(markdown_widget, markdown_unsupported_cb);

    int32_t width = lv_display_get_horizontal_resolution(NULL);
    int32_t height = lv_display_get_horizontal_resolution(NULL);
    lv_obj_set_size(markdown_widget, width, height);

    lv_timer_create(timer_cb, 100, markdown_widget);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void markdown_paragraph_style_cb(lv_style_t* style)
{
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_bg_color(style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(style, LV_OPA_COVER);
}

static void markdown_thematic_break_style_cb(lv_style_t* style)
{
    lv_style_set_bg_color(style, lv_color_hex(0x808080));
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_radius(style, 2);
    lv_style_set_border_color(style, lv_palette_main(LV_PALETTE_BLUE));
}

static void markdown_url_create_cb(lv_span_t* span, const char* src, const char* title, const char* alt)
{
    LV_UNUSED(src);
    LV_UNUSED(title);

    lv_span_set_text(span, alt);
    lv_style_set_text_color(&span->style, lv_color_hex(0xFFB6C1));
}

static lv_obj_t* markdown_image_create_cb(lv_obj_t* parent, const char* src, const char* title, const char* alt, int32_t width_hint)
{
    LV_UNUSED(title);
    LV_UNUSED(alt);
    LV_UNUSED(width_hint);

    /**
     * If you just want to display the image url, you can use this code instead of creating an image object.
     *
     * lv_obj_t * label = lv_label_create(parent);
     * lv_label_set_text(label, src);
     * lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
     * lv_obj_set_width(label, width_hint);
     *
     * return label;
     */

    lv_obj_t* image = lv_image_create(parent);
    lv_image_set_src(image, src);

    return image;
}

lv_text_decor_t lv_style_get_text_decor(const lv_style_t* style)
{
    lv_text_decor_t decor = LV_TEXT_DECOR_NONE;
    lv_style_value_t value;
    lv_result_t res = lv_style_get_prop(style, LV_STYLE_TEXT_DECOR, &value);
    if (res != LV_RESULT_OK) {
        LV_LOG_WARN("Failed to get text decor property");
    } else {
        decor = (int32_t)value.num;
    }
    return decor;
}

static void markdown_text_deco_style_cb(lv_style_t* style, vg_markdown_decor_t decor)
{
    lv_text_decor_t current_decor = lv_style_get_text_decor(style);

    switch (decor) {
    case VG_MARKDOWN_DECOR_STRIKETHROUGH:
        current_decor |= LV_TEXT_DECOR_STRIKETHROUGH;
        break;
    case VG_MARKDOWN_DECOR_EM:
        current_decor |= LV_TEXT_DECOR_UNDERLINE;
        break;
    case VG_MARKDOWN_DECOR_STRONG:
        lv_style_set_text_color(style, lv_color_hex(0xFF0000));
        break;
    case VG_MARKDOWN_DECOR_NONE:
        current_decor = LV_TEXT_DECOR_NONE;
        lv_style_set_text_font(style, lv_font_default());
        break;
    default:
        break;
    }

    lv_style_set_text_decor(style, current_decor);
}

static void markdown_list_marker_cb(char* buff, uint32_t size, int32_t level, int32_t index, bool ordered)
{
    LV_UNUSED(level);
    if (ordered)
        lv_snprintf(buff, size, "%d. ", index);
    else
        lv_snprintf(buff, size, level == 1 ? "+ " : "- ");
}

static void markdown_heading_style_cb(lv_style_t* style, int32_t level)
{
    const int32_t font_cnt = sizeof(fonts) / sizeof(fonts[0]);
    if (0 < level && level <= font_cnt) {
        if (fonts[level - 1])
            lv_style_set_text_font(style, fonts[level - 1]);
        else
            lv_style_set_text_font(style, lv_font_default());
    } else {
        lv_style_set_text_font(style, fonts[font_cnt - 1]);
    }
}

static void markdown_unsupported_cb(char* buff, uint32_t size, lv_style_t* style, int32_t type)
{
    const char* type_str;
    if (type == CMARK_NODE_IMAGE) {
        type_str = "图片";
    } else if (type == CMARK_NODE_CODE) {
        type_str = "代码";
    } else if (type == CMARK_NODE_CODE_BLOCK) {
        type_str = "代码块";
    } else if (type == CMARK_NODE_HTML_BLOCK) {
        type_str = "超文本语言";
    } else if (type == CMARK_NODE_HTML_INLINE) {
        type_str = "超文本语言块";
    } else if (type == CMARK_NODE_CUSTOM_BLOCK) {
        type_str = "块";
    } else if (type == CMARK_NODE_BLOCK_QUOTE) {
        type_str = "引用块";
    } else if (type == CMARK_NODE_TABLE) {
        type_str = "表格";
    } else if (type == CMARK_NODE_LATEX_MATH) {
        type_str = "公式";
    } else {
        type_str = "块";
    }
    lv_snprintf(buff, size, "%s不支持", type_str);
    lv_style_set_text_color(style, lv_palette_main(LV_PALETTE_GREY));
}

#endif
