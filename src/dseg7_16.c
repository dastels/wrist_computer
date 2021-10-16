/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts:
 ******************************************************************************/

#include "lvgl.h"

#ifndef DSEG7_16
#define DSEG7_16 1
#endif

#if DSEG7_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+002E "." */
    0xf0,

    /* U+0030 "0" */
    0x7f, 0xa0, 0x18, 0x6, 0x1, 0x80, 0x60, 0x18,
    0x4, 0x0, 0x80, 0x60, 0x18, 0x6, 0x1, 0x80,
    0x60, 0x17, 0xf8,

    /* U+0031 "1" */
    0xfd, 0xfc,

    /* U+0032 "2" */
    0x7f, 0x80, 0x10, 0x4, 0x1, 0x0, 0x40, 0x10,
    0x7, 0xff, 0x80, 0x20, 0x8, 0x2, 0x0, 0x80,
    0x20, 0x7, 0xf8,

    /* U+0033 "3" */
    0x7f, 0x0, 0x40, 0x20, 0x10, 0x8, 0x4, 0x3,
    0xff, 0x0, 0x80, 0x40, 0x20, 0x10, 0x8, 0x5,
    0xfc,

    /* U+0034 "4" */
    0x80, 0x60, 0x18, 0x6, 0x1, 0x80, 0x60, 0x1f,
    0xfc, 0x1, 0x0, 0x40, 0x10, 0x4, 0x1, 0x0,
    0x40,

    /* U+0035 "5" */
    0x7f, 0xa0, 0x8, 0x2, 0x0, 0x80, 0x20, 0x8,
    0x3, 0xff, 0x0, 0x40, 0x10, 0x4, 0x1, 0x0,
    0x40, 0x17, 0xf8,

    /* U+0036 "6" */
    0x7f, 0xa0, 0x8, 0x2, 0x0, 0x80, 0x20, 0x8,
    0x3, 0xff, 0x80, 0x60, 0x18, 0x6, 0x1, 0x80,
    0x60, 0x17, 0xf8,

    /* U+0037 "7" */
    0x7f, 0xa0, 0x18, 0x6, 0x1, 0x80, 0x60, 0x18,
    0x4, 0x0, 0x0, 0x40, 0x10, 0x4, 0x1, 0x0,
    0x40, 0x10,

    /* U+0038 "8" */
    0x7f, 0xa0, 0x18, 0x6, 0x1, 0x80, 0x60, 0x18,
    0x7, 0xff, 0x80, 0x60, 0x18, 0x6, 0x1, 0x80,
    0x60, 0x17, 0xf8,

    /* U+0039 "9" */
    0x7f, 0xa0, 0x18, 0x6, 0x1, 0x80, 0x60, 0x18,
    0x7, 0xff, 0x0, 0x40, 0x10, 0x4, 0x1, 0x0,
    0x40, 0x17, 0xf8,

    /* U+003A ":" */
    0xa0, 0xa
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 51, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 0, .box_w = 2, .box_h = 2, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 209, .box_w = 1, .box_h = 14, .ofs_x = 10, .ofs_y = 1},
    {.bitmap_index = 23, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 209, .box_w = 9, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 209, .box_w = 10, .box_h = 13, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 76, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 95, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 209, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 132, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 209, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 51, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xe
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 15, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 48, .range_length = 11, .glyph_id_start = 3,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t dseg7_16 = {
#else
lv_font_t dseg7_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if DSEG7_16*/
