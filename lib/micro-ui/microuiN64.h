#ifndef H_MICROUI_N64
#define H_MICROUI_N64

#include "microui.h"
#include <stdbool.h>
#include <stdint.h>
#include <libdragon.h>

// COLORS
extern color_t RED;
extern color_t ORANGE;
extern color_t YELLOW;
extern color_t GREEN;
extern color_t BLUE;
extern color_t INDIGO;
extern color_t VIOLET;
extern color_t BLACK;
extern color_t WHITE;
extern color_t GREY;

extern color_t DARK_RED;
extern color_t DARK_GREEN;

mu_Color convert_color_to_mu(color_t src);
color_t convert_mu_to_color(mu_Color src);

void mu64_init(joypad_port_t joypad_idx, uint8_t font_idx);

void mu64_start_frame();
void mu64_end_frame();
void mu64_draw();

bool mu64_is_active();
void mu64_set_mouse_speed(float speed);

extern mu_Context mu_ctx;

#endif