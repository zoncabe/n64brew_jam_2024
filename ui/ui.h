#ifndef UI_H
#define UI_H

#include <libdragon.h>
#include "../lib/micro-ui/microuiN64.h"
#include "../util/JsonUtil.h"

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

struct DialogData{
    int dialogID;
    int dialogLines;
    int textPos[4];
    std::string text;
    std::string windowColor;
    std::string textColor;
};

class DialogSerializer : public JsonSerializer<DialogData>
{
public:
	DialogSerializer() = default;
	virtual ~DialogSerializer() = default;

protected:
	virtual void Serialize(nlohmann::json& parentNode, DialogData& instance, bool isReading) override
	{
        JsonUtil::Serialize(parentNode["dialogLines"], instance.dialogLines, isReading);
        JsonUtil::SerializeString(parentNode["text"], instance.text, isReading);
        for (int i = 0; i < 2; ++i)
        {
            JsonUtil::Serialize(parentNode["textPos"][i], instance.textPos[i], isReading);
        }
        JsonUtil::SerializeString(parentNode["windowColor"], instance.windowColor, isReading);
        JsonUtil::SerializeString(parentNode["textColor"], instance.textColor, isReading);
	}
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

/* Function to parse raw RGBA data, if needed
static int ui_pack_color_from_string(const std::string& hex)
{
    unsigned int color = 0;
    std::stringstream ss;
    ss << std::hex << hex.substr(1); // Skip the leading '#'
    ss >> color;
    return color;
}
*/

int ui_string_to_color(const std::string& colorStr)
{
    static std::map<std::string, ColorNames> colorMap = {
        {"RED", RED}, {"ORANGE", ORANGE}, {"YELLOW", YELLOW}, {"GREEN", GREEN}, {"BLUE", BLUE}, {"INDIGO", INDIGO}, {"VIOLET", VIOLET},
        {"BLACK", BLACK}, {"WHITE", WHITE}, {"LIGHT_GREY", LIGHT_GREY}, {"GREY", GREY}, {"DARK_GREY", DARK_GREY}, {"TRANSPARENT", TRANSPARENT},
        {"T_RED", T_RED}, {"T_ORANGE", T_ORANGE}, {"T_YELLOW", T_YELLOW}, {"T_GREEN", T_GREEN}, {"T_BLUE", T_BLUE}, {"T_INDIGO", T_INDIGO}, {"T_VIOLET", T_VIOLET},
        {"DARK_RED", DARK_RED}, {"DARK_GREEN", DARK_GREEN}, {"N_RED", N_RED}, {"N_YELLOW", N_YELLOW}, {"N_GREEN", GREEN}, {"N_BLUE", BLUE}
    };

    auto it = colorMap.find(colorStr);
    if (it != colorMap.end())
    {
        return it->second;
    }
    else
    {
        return WHITE;
    }
}

void ui_textbox(void)
{
    // Parse JSON.
    DialogData dialogData = {};
	static const char* kDialogFilePath = "rom:/dialog_test.json";
	DialogSerializer serializer;
    static int printOnce = 0;
	if (serializer.Load(kDialogFilePath, dialogData) && printOnce == 0)
	{
		debugf("\nLoaded Dialog JSON: %d\n", dialogData.dialogID);
        printOnce = 1;
	}

    // Set up styles.
    mu_ctx._style.colors[MU_COLOR_BASE] = pack_color_to_mu(ui_string_to_color(dialogData.windowColor));
    mu_ctx._style.colors[MU_COLOR_TEXT] = pack_color_to_mu(ui_string_to_color(dialogData.textColor));

    // Calculate text window and text box positions.
    const int charWidth = 10;
    int textBoxOffset = ((dialogData.dialogLines+1) * charWidth);
    dialogData.textPos[2] = ((dialogData.dialogLines+1) * (charWidth * 2));
    dialogData.textPos[3] = (dialogData.dialogLines * (charWidth + 1));

    mu_Rect textWindow = mu_rect(dialogData.textPos[0], dialogData.textPos[1], dialogData.textPos[2], dialogData.textPos[3]);
    mu_Rect textBox = mu_rect(dialogData.textPos[0]+charWidth, dialogData.textPos[1] - textBoxOffset, dialogData.textPos[2] * 2, dialogData.textPos[3] + textBoxOffset);

    // Draw the text box window.
    if (mu_begin_window_ex(&mu_ctx, "", textWindow, (MU_OPT_NOTITLE | MU_OPT_NOFRAME | MU_OPT_POPUP)))
    {
        mu_Id id = mu_get_id(&mu_ctx, &dialogData.text, sizeof(dialogData.text));
        mu_textbox_raw(&mu_ctx, const_cast<char*>(dialogData.text.c_str()), dialogData.text.length(), id, textBox, MU_OPT_POPUP);
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