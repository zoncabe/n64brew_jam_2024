#ifndef H_MICROUI_N64
#define H_MICROUI_N64

#include "microui.h"
#include "../kodachrome/kodachrome.h"
#include <stdbool.h>
#include <stdint.h>
#include <libdragon.h>

/*~~s4ys~~*/
mu_Color pack_color_to_mu(int colIdx);
void mu64_set_font_idx(int idx);
typedef struct {
  mu_Color color;
  int style_id;
} ColorStyleMap;
/*~~s4ys~~*/

void mu64_init(joypad_port_t joypad_idx);

void mu64_start_frame();
void mu64_end_frame();
void mu64_draw();

bool mu64_is_active();
void mu64_set_mouse_speed(float speed);

extern mu_Context mu_ctx;

#endif