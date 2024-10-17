#ifndef H_MICROUI_N64
#define H_MICROUI_N64

#include "microui.h"
#include <stdbool.h>
#include <stdint.h>
#include <libdragon.h>

// COLORS
extern const uint8_t RED[4];
extern const uint8_t ORANGE[4];
extern const uint8_t YELLOW[4];
extern const uint8_t GREEN[4];
extern const uint8_t BLUE[4];
extern const uint8_t INDIGO[4];
extern const uint8_t VIOLET[4];
extern const uint8_t BLACK[4];
extern const uint8_t WHITE[4];
extern const uint8_t GREY[4];
extern const uint8_t DARK_RED[4];
extern const uint8_t DARK_GREEN[4];
const color_t pack_color(const uint8_t src[]);
mu_Color pack_color_to_mu(const uint8_t src[]);

void mu64_init(joypad_port_t joypad_idx, uint8_t font_idx);

void mu64_start_frame();
void mu64_end_frame();
void mu64_draw();

bool mu64_is_active();
void mu64_set_mouse_speed(float speed);

extern mu_Context mu_ctx;

#endif