
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
                                  "# Heading Verify\n"
                                  "\n"
                                  "# Not Add nested heading"
                                  "\n"
                                  "# Paragraph nested heading"
                                  "\n"
                                  "\n"
                                  "# Paragraph nested heading + heading nested _italic_"
                                  "\n"
                                  "\n"
                                  "### Paragraph nested heading + **heading nested bold**"
                                  "\n"
                                  "\n"
                                  "###### Paragraph nested heading + ~~heading nested DeleteLine~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Paragraph Verify\n"
                                  "\n"
                                  "\n"
                                  "Not Add nested Paragraph1"
                                  "\n"
                                  "\n"
                                  "Not Add nested Paragraph2\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Italic Verify\n"
                                  "\n"
                                  "_Not Add nested italic_"
                                  "\n"
                                  "_Paragraph nested italic_"
                                  "\n"
                                  "\n"
                                  "_Paragraph nested italic_ + _multiple_ Define _italic_"
                                  "\n"
                                  "\n"
                                  "_Paragraph nested italic + **italic nested bold**_"
                                  "\n"
                                  "\n"
                                  "_Paragraph nested italic + ~~italic nested DeleteLine~~_\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Bold Verify\n"
                                  "\n"
                                  "**Not Add nested bold**"
                                  "\n"
                                  "**Paragraph nested bold**"
                                  "\n"
                                  "\n"
                                  "**Paragraph nested bold** + **multiple** Define **bold**"
                                  "\n"
                                  "\n"
                                  "**Paragraph nested bold + ~~bold nested DeleteLine~~**"
                                  "\n"
                                  "\n"
                                  "**Paragraph nested bold + _bold nested italic_**\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# DeleteLine Verify\n"
                                  "\n"
                                  "~~Not add nested DeleteLine~~"
                                  "\n"
                                  "~~Paragraph nested DeleteLine~~"
                                  "\n"
                                  "\n"
                                  "~~Paragraph nested DeleteLine~~ + ~~multiple~~ Define ~~DeleteLine~~"
                                  "\n"
                                  "\n"
                                  "~~Paragraph nested DeleteLine + **DeleteLine nested bold**~~"
                                  "\n"
                                  "\n"
                                  "Paragraph nested ~~DeleteLine + _DeleteLine nested italic_~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Unordered List Verify1\n"
                                  "\n"
                                  "- Not add nested, unordered List style 1"
                                  "\n"
                                  "- Paragraph nested unordered List style 1"
                                  "\n"
                                  "+ Not add nested, unordered List style 2"
                                  "\n"
                                  "+ Paragraph nested unordered List style 2"
                                  "\n"
                                  "* Not add nested, unordered List style 3"
                                  "\n"
                                  "* Paragraph nested unordered List style 3"
                                  "\n"
                                  "\n"
                                  "- Paragraph nested unordered List + _unordered List nested italic1_ \n"
                                  "    - Multi-level unordered List\n"
                                  "    - Multi-level unordered List\n"
                                  "    - Multi-level unordered List\n"
                                  "\n"
                                  "\n"
                                  "+ Paragraph nested unordered List + _unordered List nested italic2_"
                                  "\n"
                                  "\n"
                                  "* Paragraph nested unordered List + _unordered List nested italic3_"
                                  "\n"
                                  "\n"
                                  "- Paragraph nested unordered List + **unordered List nested bold1**"
                                  "\n"
                                  "\n"
                                  "+ Paragraph nested unordered List + **unordered List nested bold2**"
                                  "\n"
                                  "\n"
                                  "* Paragraph nested unordered List + **unordered List nested bold3**"
                                  "\n"
                                  "\n"
                                  "- Paragraph nested unordered List + ~~unordered List nested DeleteLine1~~"
                                  "\n"
                                  "\n"
                                  "+ Paragraph nested unordered List + ~~unordered List nested DeleteLine2~~"
                                  "\n"
                                  "\n"
                                  "* Paragraph nested unordered List + ~~unordered List nested DeleteLine3~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Ordered List Verify1\n"
                                  "\n"
                                  "1. Not Add nested, ordered List"
                                  "\n"
                                  "1. Paragraph nested ordered List"
                                  "\n"
                                  "\n"
                                  "1. Paragraph nested ordered List + _ordered List nested italic_\n"
                                  "    1. Multi-level ordered List\n"
                                  "    2. Multi-level ordered List\n"
                                  "    3. Multi-level ordered List\n"
                                  "\n"
                                  "\n"
                                  "1. Paragraph nested ordered List + **ordered List nested bold**"
                                  "\n"
                                  "\n"
                                  "1. Paragraph nested ordered List + ~~ordered List nested DeleteLine~~"
                                  "\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Unordered List Verify2\n"
                                  "\n"
                                  "- Multiple unordered List"
                                  "\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Ordered List Verify2\n"
                                  "\n"
                                  "1. Multiple ordered List\n"
                                  "1. Multiple ordered List\n"
                                  "1. Multiple ordered List\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Link Verify\n"
                                  "\n"
                                  "[](https://www.baidu.com/)Not Add nested link / Not Add self Define\n"
                                  "\n"
                                  "[Baidu](https://www.baidu.com/)Paragraph nested link / Add self Define as text\n"
                                  "\n"
                                  "[Baidu](https://www.baidu.com/)Not Add nested link / Add self Define as text\n"
                                  "\n"
                                  "[Paragraph nested link + _link nested italic_](https://www.baidu.com/)\n"
                                  "\n"
                                  "\n"
                                  "[Paragraph nested link + **link nested bold**](https://www.baidu.com/)"
                                  "\n"
                                  "\n"
                                  "[Paragraph nested link + ~~link nested DeleteLine~~](https://www.baidu.com/)\n"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Graph Verify\n"
                                  "\n"
                                  "![](/data/markdown.png)Valid url Not Add nested Graph / Not Add self Define\n"
                                  "![](/data/markdown.png)![](/data/markdown.png)![](/data/markdown.png)Valid url Not Add nested Graph / Not Add self Define + multiple Define Graph\n"
                                  "\n"
                                  "![My Graph](/data/markdown.png)Valid url Paragraph nested Graph / Add self Define text"
                                  "\n"
                                  "\n"
                                  "![My Graph](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![My Graph + invalid + _Graph nested italic_](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![My Graph + invalid + **Graph nested bold**](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "![My Graph + invalid + ~~Graph nested DeleteLine~~](/datas/markdown.png)"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Split Line Verify\n"
                                  "\n"
                                  "___"
                                  "___"
                                  "\n"
                                  "___"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Multi-level Verify\n"
                                  "\n"
                                  "\n"
                                  "_markdown Editor, **give you WYSIWYG, ~~Edit experience~~**_"
                                  "\n"
                                  "\n"
                                  "_markdown Editor, ~~give you WYSIWYG, **Edit experience**~~_"
                                  "\n"
                                  "\n"
                                  "**markdown Editor, _give you WYSIWYG, ~~Edit experience~~_**"
                                  "\n"
                                  "\n"
                                  "**markdown Editor, ~~give you WYSIWYG, _Edit experience_~~**"
                                  "\n"
                                  "\n"
                                  "~~markdown Editor, **give you WYSIWYG, _Edit experience_**~~"
                                  "\n"
                                  "\n"
                                  "~~markdown Editor, _give you WYSIWYG, **Edit experience**_~~"
                                  "\n"
                                  "\n"
                                  "___\n"
                                  "# Other Verify\n"
                                  "\n"
                                  "\n"
                                  "```"
                                  "shell:~$ sudo apt-get install libgtest-dev"
                                  "```"
                                  "\n"
                                  ">Comment code"
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
        type_str = "Graph";
    } else if (type == CMARK_NODE_CODE) {
        type_str = "Code";
    } else if (type == CMARK_NODE_CODE_BLOCK) {
        type_str = "Code block";
    } else if (type == CMARK_NODE_HTML_BLOCK) {
        type_str = "HTML";
    } else if (type == CMARK_NODE_HTML_INLINE) {
        type_str = "HTML inline";
    } else if (type == CMARK_NODE_CUSTOM_BLOCK) {
        type_str = "Block";
    } else if (type == CMARK_NODE_BLOCK_QUOTE) {
        type_str = "Reference block";
    } else if (type == CMARK_NODE_TABLE) {
        type_str = "Table";
    } else if (type == CMARK_NODE_LATEX_MATH) {
        type_str = "Formula";
    } else {
        type_str = "Block";
    }
    lv_snprintf(buff, size, "%s not supported", type_str);
    lv_style_set_text_color(style, lv_palette_main(LV_PALETTE_GREY));
}

#endif
