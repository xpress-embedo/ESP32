/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: --bpp 1 --size 18 --font D:/Projects/Embedded/ESP32/ESP-IDF/ESP32_TrafficController/square_line/assets/Seven Segment.ttf -o D:/Projects/Embedded/ESP32/ESP-IDF/ESP32_TrafficController/square_line/assets\ui_font_SevenSegment18.c --format lvgl -r 0x30-0x39 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_SEVENSEGMENT18
#define UI_FONT_SEVENSEGMENT18 1
#endif

#if UI_FONT_SEVENSEGMENT18

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x7d, 0x6, 0xc, 0x18, 0x30, 0x40, 0x41, 0x83,
    0x6, 0xc, 0x17, 0xc0,

    /* U+0031 "1" */
    0xfd, 0xf8,

    /* U+0032 "2" */
    0x7c, 0x4, 0x8, 0x10, 0x20, 0x5f, 0x40, 0x81,
    0x2, 0x4, 0x7, 0xc0,

    /* U+0033 "3" */
    0xf8, 0x10, 0x41, 0x4, 0x1f, 0x81, 0x4, 0x10,
    0x41, 0xf8,

    /* U+0034 "4" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x5f, 0x1, 0x2,
    0x4, 0x8, 0x10, 0x20,

    /* U+0035 "5" */
    0x7d, 0x2, 0x4, 0x8, 0x10, 0x1f, 0x1, 0x2,
    0x4, 0x8, 0x17, 0xc0,

    /* U+0036 "6" */
    0x7d, 0x2, 0x4, 0x8, 0x10, 0x1f, 0x41, 0x83,
    0x6, 0xc, 0x17, 0xc0,

    /* U+0037 "7" */
    0xf8, 0x10, 0x41, 0x4, 0x10, 0x1, 0x4, 0x10,
    0x41, 0x4,

    /* U+0038 "8" */
    0x7d, 0x6, 0xc, 0x18, 0x30, 0x5f, 0x41, 0x83,
    0x6, 0xc, 0x17, 0xc0,

    /* U+0039 "9" */
    0x7d, 0x6, 0xc, 0x18, 0x30, 0x5f, 0x1, 0x2,
    0x4, 0x8, 0x17, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 12, .adv_w = 53, .box_w = 1, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 26, .adv_w = 131, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 36, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 48, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 60, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 131, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 145, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_SevenSegment18 = {
#else
lv_font_t ui_font_SevenSegment18 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 13,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -6,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_SEVENSEGMENT18*/

