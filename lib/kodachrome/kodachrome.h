/**
* @copyright 2024 - s4ys
* @license Unlicense
*/

#ifndef KODACHROME_H
#define KODACHROME_H


#include <stdint.h>
#include <libdragon.h>

enum ColorNames
{
    // Standard ROYGBIV
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    INDIGO,
    VIOLET,
    // RGB 0 (full black) & 1 (full white)
    BLACK,
    WHITE,
    // RGB 1 * (n*.25f)
    LIGHT_GREY, // n = 3
    GREY, // n = 2
    DARK_GREY, // n = 1
    // Transparent Colors
    TRANSPARENT,
    T_RED,
    T_ORANGE,
    T_YELLOW,
    T_GREEN,
    T_BLUE,
    T_INDIGO,
    T_VIOLET,
    T_BLACK,
    T_WHITE,
    T_GREY,
    // Darker Variants
    DARK_RED,
    DARK_GREEN,
    // N64 Logo Colors
    N_RED,
    N_YELLOW,
    N_GREEN,
    N_BLUE,
    COLOR_COUNT
};

extern const uint8_t COLORS[COLOR_COUNT][4];

const color_t pack_color_32(int color_label);
const color_t pack_color_16(int color_label);



#endif // KODACHROME_H