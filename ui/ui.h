#include <libdragon.h>
#include "../lib/micro-ui/microuiN64.h"

#define FONT_ID_GAME 2

enum FONT_STYLES 
{
    STYLE_DEFAULT,
    STYLE_COUNT
};

// Step 1/5: Make sure you have a small font loaded
uint8_t ui_register_font(void)
{
    // Load font64 and register to font index 2, 0 reserved by the SDK, 1 should be reserved debug output
    rdpq_font_t *font = rdpq_font_load("rom:/TitanOne-Regular.font64");
    static uint8_t font_id = FONT_ID_GAME;
    rdpq_text_register_font(font_id, font);

    // Create and register font style
    rdpq_fontstyle_t txt_game_fontStyle;
    txt_game_fontStyle.color = pack_color(BLACK);
    rdpq_font_style(
        font, 
        STYLE_DEFAULT, 
        &txt_game_fontStyle
    );

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

// Basic window, you can add inputs to modify variables
void ui_main_menu(void)
{
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
}

// Step 4/5: call this AFTER your game logic ends each frame
void ui_update(void)
{

    mu64_set_mouse_speed(0.04f * display_get_delta_time()); // keep cursor speed constant

    // You can create windows at any point in your game-logic.
    // This does not render the window directly, which is handled later in a single batch.
    ui_main_menu();

    mu64_end_frame();
}

// Step 5/5: render out the UI at the very end
void ui_draw(void)
{
    ui_update();
    mu64_draw();
}