#include <stdint.h>
#include <libdragon.h>

#include "kodachrome.h"

const uint8_t COLORS[COLOR_COUNT][4] = 
{
    {209, 0, 0, 255},    // RED
    {255, 102, 34, 255}, // ORANGE
    {255, 218, 33, 255}, // YELLOW
    {51, 221, 0, 255},   // GREEN
    {17, 51, 204, 255},  // BLUE
    {34, 0, 102, 255},   // INDIGO
    {51, 0, 68, 255},    // VIOLET
    {0, 0, 0, 255},      // BLACK
    {255, 255, 255, 255}, // WHITE
    {192, 192, 192, 255}, // LIGHT_GREY  
    {128, 128, 128, 255}, // GREY        
    {64, 64, 64, 255}, // DARK_GREY   
    {0, 0, 0, 127}, // TRANSPARENT 
    {209, 0, 0, 200}, // T_RED       
    {255, 102, 34, 200}, // T_ORANGE    
    {255, 218, 33, 200}, // T_YELLOW    
    {51, 221, 0, 200}, // T_GREEN     
    {17, 51, 204, 200}, // T_BLUE      
    {34, 0, 102, 200}, // T_INDIGO    
    {51, 0, 68, 200}, // T_VIOLET    
    {31,31,31,200}, // T_BLACK     
    {255, 255, 255, 200}, // T_WHITE     
    {192, 192, 192, 200}, // T_GREY      
    {130, 0, 0, 255}, // DARK_RED    
    {0, 100, 0, 255}, // DARK_GREEN  
    {225, 9, 22, 255}, // N_RED       
    {245, 178, 1, 255}, // N_YELLOW    
    {50, 153, 0, 255}, // N_GREEN     
    {1, 0, 154, 255}, // N_BLUE      
};

const color_t pack_color_32(int color_label)
{
    return RGBA32(COLORS[color_label][0], COLORS[color_label][1], COLORS[color_label][2], COLORS[color_label][3]);
}

const color_t pack_color_16(int color_label)
{
    return RGBA16(COLORS[color_label][0], COLORS[color_label][1], COLORS[color_label][2], COLORS[color_label][3]);
}