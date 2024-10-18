#ifndef UI_H
#define UI_H

#include <libdragon.h>
#include "../lib/micro-ui/microuiN64.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_ID_GAME 2
enum FONT_IDS 
{
    ID_RESERVED,
    ID_DEBUG,
    ID_DEFAULT,
    ID_TITLE,
    ID_COUNT
};
enum FONT_STYLES 
{
    STYLE_DEFAULT,
    STYLE_TITLE,
    STYLE_BRIGHT,
    STYLE_GREEN,
    STYLE_COUNT
};

// Step 1/5: Make sure you have a small font loaded
void ui_register_fonts(void)
{
    // Load font64 and register to font index 2, 0 reserved by the SDK, 1 should be reserved debug output
    rdpq_font_t *font[ID_COUNT];
    font[ID_DEFAULT] = rdpq_font_load("rom:/TitanOne-Regular.font64");
    font[ID_TITLE] = rdpq_font_load("rom:/chunkysans.font64");

    // Create and register font styles
    rdpq_fontstyle_t txt_game_fontStyle;
    txt_game_fontStyle.color = pack_color_32(WHITE);

    rdpq_fontstyle_t txt_title_fontStyle;
    txt_title_fontStyle.color = pack_color_32(BLACK);

    rdpq_fontstyle_t txt_bright_fontStyle;
    txt_bright_fontStyle.color = pack_color_32(T_YELLOW);

    rdpq_fontstyle_t txt_green_fontStyle;
    txt_green_fontStyle.color = pack_color_32(GREEN);

    for (int i = 2; i < ID_COUNT; i++)
    {
        rdpq_text_register_font(i, font[i]);
        rdpq_font_style(
            font[i], 
            STYLE_DEFAULT, 
            &txt_game_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_TITLE, 
            &txt_title_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_BRIGHT, 
            &txt_bright_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_GREEN, 
            &txt_green_fontStyle
        );
    }

}

// Step 2/5: init UI libary, pass in the controller (joystick or N64-mouse) and font-id
// (Note: take a look inside this function for styling and controls.)
void ui_init(void)
{
    ui_register_fonts();
    mu64_init(JOYPAD_PORT_1);
}

// Step 3/5: call this BEFORE your game logic starts each frame
void ui_poll (void)
{
    mu64_start_frame();
}

// Basic window, you can add inputs to modify variables
void ui_main_menu(void)
{
    mu_ctx._style.colors[MU_COLOR_PANELBG]  = pack_color_to_mu(T_BLUE);
    mu_ctx._style.colors[MU_COLOR_WINDOWBG] = pack_color_to_mu(T_BLUE);

    if (mu_begin_window_ex(&mu_ctx, "N64BREW GAME JAM 2024", mu_rect(32, 32, 194, 90), (MU_OPT_NOINTERACT | MU_OPT_NOCLOSE) ))
    {
        mu_ctx._style.colors[MU_COLOR_TEXT]  = pack_color_to_mu(WHITE);
        if (mu_header_ex(&mu_ctx, "INFO", MU_OPT_EXPANDED | MU_OPT_AUTOSIZE)) {
            mu_ctx._style.colors[MU_COLOR_TEXT]  = pack_color_to_mu(YELLOW);
            mu_label(&mu_ctx, "Yo, what up?");
        }

        mu_ctx._style.colors[MU_COLOR_TEXT]  = pack_color_to_mu(WHITE);
        if (mu_header_ex(&mu_ctx, "STATS", MU_OPT_EXPANDED | MU_OPT_AUTOSIZE)) {
            mu_ctx._style.colors[MU_COLOR_TEXT]  = pack_color_to_mu(GREEN);
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

void ui_textbox(void)
{

    mu_ctx._style.colors[MU_COLOR_BASEHOVER]  = pack_color_to_mu(DARK_GREY);
    mu_ctx._style.colors[MU_COLOR_WINDOWBG] = pack_color_to_mu(BLACK);
    mu_ctx._style.colors[MU_COLOR_TEXT]  = pack_color_to_mu(YELLOW);
    const char *text;
    const int charWidth = 10;
    uint8_t dialogID = 0;
    int dialogLines;
    int textBoxOffset;
    int textPos[4];
    switch(dialogID)
    {
        case 0:
            dialogLines = 5; // parse
            textBoxOffset = ((dialogLines+1) * charWidth);
            textPos[2] = ((dialogLines+1) * (charWidth*2));
            textPos[3] = (dialogLines * (charWidth+1));
            textPos[0] = 32; // parse
            textPos[1] = 128; // parse
            text = "\n\
                    I'm Spaghet, boo!\n\
                    Spaghet! Spooked ya!\n\
                    Spaghet! Spooked ya!\n\
                    Spaghet! Spooked ya!\n\
                    Spaghet! Spooked ya!"; // parse, format with leading line and line breaks
            break;
    }

    mu_Rect textWindow = mu_rect(textPos[0],textPos[1],textPos[2],textPos[3]);
    mu_Rect textBox = mu_rect(textPos[0]-32,textPos[1]-textBoxOffset,textPos[2]*2,textPos[3]+textBoxOffset);

    if (mu_begin_window_ex(&mu_ctx, "", textWindow, (MU_OPT_NOTITLE | MU_OPT_NOFRAME | MU_OPT_POPUP)))
    {
        mu_Id id = mu_get_id(&mu_ctx, &text, sizeof(text));
        mu_textbox_raw(&mu_ctx, const_cast<char*>(text), strlen(text), id, textBox, MU_OPT_POPUP);
        mu_end_window(&mu_ctx);
    }
}

// Step 4/5: call this AFTER your game logic ends each frame
void ui_update(void)
{

    mu64_set_mouse_speed(0.04f * display_get_delta_time()); // keep cursor speed constant

    // You can create windows at any point in your game-logic.
    // This does not render the window directly, which is handled later in a single batch.
    mu64_set_font_idx(ID_DEFAULT);
    ui_textbox();
    mu64_end_frame();
    mu64_draw();

    mu64_start_frame();
    mu64_set_font_idx(ID_TITLE);
    ui_main_menu();
    mu64_end_frame();
}

// Step 5/5: render out the UI at the very end
void ui_draw(void)
{
    ui_update();
    mu64_draw();
}

#ifdef __cplusplus
}
#endif

#endif // UI_H