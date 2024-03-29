/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: --size 8 --bpp 1 --format lvgl --font /home/steven/.local/share/fonts/MinecraftMono.otf --range 32-127 -o src/minecraft_font.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef MINECRAFT_FONT
#define MINECRAFT_FONT 1
#endif

#if MINECRAFT_FONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfa,

    /* U+0022 "\"" */
    0xb4,

    /* U+0023 "#" */
    0x55, 0xf5, 0xf5, 0x50,

    /* U+0024 "$" */
    0x23, 0xa0, 0xe0, 0xf8, 0x80,

    /* U+0025 "%" */
    0x8c, 0x84, 0x44, 0x26, 0x20,

    /* U+0026 "&" */
    0x22, 0x88, 0xdb, 0x49, 0xa0,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0x2a, 0x48, 0x88,

    /* U+0029 ")" */
    0x88, 0x92, 0xa0,

    /* U+002A "*" */
    0xaa, 0x80,

    /* U+002B "+" */
    0x22, 0xf2, 0x20,

    /* U+002C "," */
    0x60,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x8, 0x84, 0x44, 0x22, 0x0,

    /* U+0030 "0" */
    0x74, 0x67, 0x5c, 0xc5, 0xc0,

    /* U+0031 "1" */
    0x23, 0x08, 0x42, 0x13, 0xe0,

    /* U+0032 "2" */
    0x74, 0x42, 0x64, 0x47, 0xe0,

    /* U+0033 "3" */
    0x74, 0x42, 0x60, 0xc5, 0xc0,

    /* U+0034 "4" */
    0x19, 0x53, 0x1f, 0x84, 0x20,

    /* U+0035 "5" */
    0xfc, 0x3c, 0x10, 0xc5, 0xc0,

    /* U+0036 "6" */
    0x32, 0x21, 0xe8, 0xc5, 0xc0,

    /* U+0037 "7" */
    0xfc, 0x42, 0x22, 0x10, 0x80,

    /* U+0038 "8" */
    0x74, 0x62, 0xe8, 0xc5, 0xc0,

    /* U+0039 "9" */
    0x74, 0x62, 0xf0, 0x89, 0x80,

    /* U+003A ":" */
    0x88,

    /* U+003B ";" */
    0x40, 0x60,

    /* U+003C "<" */
    0x12, 0x48, 0x42, 0x10,

    /* U+003D "=" */
    0xf0, 0xf,

    /* U+003E ">" */
    0x84, 0x21, 0x24, 0x80,

    /* U+003F "?" */
    0x74, 0x42, 0x22, 0x0, 0x80,

    /* U+0040 "@" */
    0x74, 0x6b, 0x5b, 0xc1, 0xc0,

    /* U+0041 "A" */
    0x74, 0x7f, 0x18, 0xc6, 0x20,

    /* U+0042 "B" */
    0xf4, 0x7d, 0x18, 0xc7, 0xc0,

    /* U+0043 "C" */
    0x74, 0x61, 0x8, 0x45, 0xc0,

    /* U+0044 "D" */
    0xf4, 0x63, 0x18, 0xc7, 0xc0,

    /* U+0045 "E" */
    0xfc, 0x39, 0x8, 0x43, 0xe0,

    /* U+0046 "F" */
    0xfc, 0x39, 0x8, 0x42, 0x0,

    /* U+0047 "G" */
    0x74, 0x27, 0x18, 0xc5, 0xc0,

    /* U+0048 "H" */
    0x8c, 0x7f, 0x18, 0xc6, 0x20,

    /* U+0049 "I" */
    0xe9, 0x24, 0xb8,

    /* U+004A "J" */
    0x8, 0x42, 0x10, 0xc5, 0xc0,

    /* U+004B "K" */
    0x8c, 0xb9, 0x28, 0xc6, 0x20,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x43, 0xe0,

    /* U+004D "M" */
    0x8e, 0xeb, 0x18, 0xc6, 0x20,

    /* U+004E "N" */
    0x8e, 0x6b, 0x38, 0xc6, 0x20,

    /* U+004F "O" */
    0x74, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0050 "P" */
    0xf4, 0x7d, 0x8, 0x42, 0x0,

    /* U+0051 "Q" */
    0x74, 0x63, 0x18, 0xc9, 0xa0,

    /* U+0052 "R" */
    0xf4, 0x7d, 0x18, 0xc6, 0x20,

    /* U+0053 "S" */
    0x74, 0x1c, 0x10, 0xc5, 0xc0,

    /* U+0054 "T" */
    0xf2, 0x22, 0x22, 0x20,

    /* U+0055 "U" */
    0x8c, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0056 "V" */
    0x8c, 0x63, 0x15, 0x28, 0x80,

    /* U+0057 "W" */
    0x8c, 0x63, 0x1a, 0xee, 0x20,

    /* U+0058 "X" */
    0x8a, 0x88, 0xa8, 0xc6, 0x20,

    /* U+0059 "Y" */
    0x8a, 0x88, 0x42, 0x10, 0x80,

    /* U+005A "Z" */
    0xf8, 0x44, 0x44, 0x43, 0xe0,

    /* U+005B "[" */
    0xf2, 0x49, 0x38,

    /* U+005C "\\" */
    0x82, 0x10, 0x41, 0x8, 0x20,

    /* U+005D "]" */
    0xe4, 0x92, 0x78,

    /* U+005E "^" */
    0x22, 0xa2,

    /* U+005F "_" */
    0xf0,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x70, 0x5f, 0x17, 0x80,

    /* U+0062 "b" */
    0x84, 0x2d, 0x98, 0xc7, 0xc0,

    /* U+0063 "c" */
    0x74, 0x61, 0x17, 0x0,

    /* U+0064 "d" */
    0x8, 0x5b, 0x38, 0xc5, 0xe0,

    /* U+0065 "e" */
    0x74, 0x7f, 0x7, 0x0,

    /* U+0066 "f" */
    0x34, 0xf4, 0x44, 0x40,

    /* U+0067 "g" */
    0x7c, 0x62, 0xf1, 0xf0,

    /* U+0068 "h" */
    0x84, 0x2d, 0x98, 0xc6, 0x20,

    /* U+0069 "i" */
    0x40, 0xc4, 0x44, 0x30,

    /* U+006A "j" */
    0x8, 0x2, 0x10, 0xc6, 0x2e,

    /* U+006B "k" */
    0x88, 0x9a, 0xca, 0x90,

    /* U+006C "l" */
    0xc4, 0x44, 0x44, 0x30,

    /* U+006D "m" */
    0xd5, 0x6b, 0x18, 0x80,

    /* U+006E "n" */
    0xf4, 0x63, 0x18, 0x80,

    /* U+006F "o" */
    0x74, 0x63, 0x17, 0x0,

    /* U+0070 "p" */
    0xb6, 0x63, 0xe8, 0x40,

    /* U+0071 "q" */
    0x6c, 0xe2, 0xf0, 0x84,

    /* U+0072 "r" */
    0xb6, 0x61, 0x8, 0x0,

    /* U+0073 "s" */
    0x74, 0x1c, 0x1f, 0x0,

    /* U+0074 "t" */
    0x44, 0xe4, 0x44, 0x30,

    /* U+0075 "u" */
    0x8c, 0x63, 0x17, 0x80,

    /* U+0076 "v" */
    0x8c, 0x62, 0xa2, 0x0,

    /* U+0077 "w" */
    0x8c, 0x6b, 0x57, 0x80,

    /* U+0078 "x" */
    0x8a, 0x88, 0xa8, 0x80,

    /* U+0079 "y" */
    0x8c, 0x62, 0xf1, 0xf0,

    /* U+007A "z" */
    0xf1, 0x24, 0xf0,

    /* U+007B "{" */
    0x29, 0x44, 0x88,

    /* U+007C "|" */
    0xfe,

    /* U+007D "}" */
    0x89, 0x14, 0xa0,

    /* U+007E "~" */
    0x4d, 0x64
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 85, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 85, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 85, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 3, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 12, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 22, .adv_w = 85, .box_w = 1, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 23, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 26, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 85, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 31, .adv_w = 85, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 34, .adv_w = 85, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 35, .adv_w = 85, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 36, .adv_w = 85, .box_w = 1, .box_h = 1, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 52, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 62, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 67, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 77, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 85, .box_w = 1, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 93, .adv_w = 85, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 95, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 85, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 101, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 110, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 115, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 178, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 212, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 217, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 85, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 255, .adv_w = 85, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 256, .adv_w = 85, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 257, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 266, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 279, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 85, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 287, .adv_w = 85, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 85, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 301, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 305, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 321, .adv_w = 85, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 325, .adv_w = 85, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 329, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 337, .adv_w = 85, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 85, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 361, .adv_w = 85, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 364, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 367, .adv_w = 85, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 85, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 85, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
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
    .cmap_num = 1,
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
const lv_font_t minecraft_font = {
#else
lv_font_t minecraft_font = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 8,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if MINECRAFT_FONT*/

