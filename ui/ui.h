#include <libdragon.h>
#include "../lib/micro-ui/microuiN64.h"


// Step 1/5: Make sure you have a small font loaded
uint8_t ui_register_font(void)
{
    rdpq_font_t *font = rdpq_font_load("rom:/TitanOne-Regular.font64");
    static uint8_t font_id = 2;
    rdpq_text_register_font(font_id, font);

    mu64_init(JOYPAD_PORT_1, font_id);

    return font_id;
}

// Step 2/5: init UI libary, pass in the controller (joystick or N64-mouse) and font-id
// (Note: take a look inside this function for styling and controls.)
void ui_init(void)
{
    mu64_init(JOYPAD_PORT_1, ui_register_font());
}

// Step 3/5: call this BEFORE your game logic starts each frame
void ui_poll (void)
{
    mu64_start_frame();
}

// Step 4/5: call this AFTER your game logic ends each frame
void ui_update(void)
{

    mu64_set_mouse_speed(0.04f * display_get_delta_time()); // keep cursor speed constant

    // You can create windows at any point in your game-logic.
    // This does not render the window directly, which is handled later in a single batch.

    // Basic window, you can add inputs to modify variables
    if (mu_begin_window_ex(&mu_ctx, "       N64BREW GAME JAM 2024", mu_rect(32, 64, 160, 80), (MU_OPT_NOINTERACT | MU_OPT_NOCLOSE) ))
    {

        if (mu_header_ex(&mu_ctx, "INFO", MU_OPT_CLOSED | MU_OPT_AUTOSIZE)) {
            mu_label(&mu_ctx, "Yo, what up?");
        }

        if (mu_header_ex(&mu_ctx, "STATS", MU_OPT_CLOSED | MU_OPT_AUTOSIZE)) {
            float fps = display_get_fps();

            char fps_buffer[16] = {0};
            sprintf(fps_buffer, "FPS: %.2f", fps);
            mu_label(&mu_ctx, fps_buffer);

            sprintf(fps_buffer, "ms: %.4f", display_get_delta_time());
            mu_label(&mu_ctx, fps_buffer);
        }

        mu_end_window(&mu_ctx);
    }

    mu64_end_frame();
}

// Step 5/5: render out the UI at the very end
void ui_draw(void)
{
    ui_update();
    mu64_draw();
}