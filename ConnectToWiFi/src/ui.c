// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: ConnectToWiFi

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_MainScreen;
lv_obj_t * ui_Keyboard;
lv_obj_t * ui_TextAreaPassword;
lv_obj_t * ui_DopDownSSID;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_MainScreen_screen_init(void)
{
    ui_MainScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MainScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Keyboard = lv_keyboard_create(ui_MainScreen);
    lv_obj_set_width(ui_Keyboard, lv_pct(95));
    lv_obj_set_height(ui_Keyboard, lv_pct(45));
    lv_obj_set_align(ui_Keyboard, LV_ALIGN_BOTTOM_MID);

    ui_TextAreaPassword = lv_textarea_create(ui_MainScreen);
    lv_obj_set_width(ui_TextAreaPassword, lv_pct(50));
    lv_obj_set_height(ui_TextAreaPassword, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TextAreaPassword, lv_pct(20));
    lv_obj_set_y(ui_TextAreaPassword, lv_pct(-10));
    lv_obj_set_align(ui_TextAreaPassword, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextAreaPassword, "********");
    lv_textarea_set_one_line(ui_TextAreaPassword, true);
    lv_textarea_set_password_mode(ui_TextAreaPassword, true);

    ui_DopDownSSID = lv_dropdown_create(ui_MainScreen);
    lv_dropdown_set_options(ui_DopDownSSID, "todo-1\ntodo-2\n");
    lv_obj_set_width(ui_DopDownSSID, lv_pct(50));
    lv_obj_set_height(ui_DopDownSSID, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_DopDownSSID, lv_pct(20));
    lv_obj_set_y(ui_DopDownSSID, lv_pct(-25));
    lv_obj_set_align(ui_DopDownSSID, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_DopDownSSID, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    lv_keyboard_set_textarea(ui_Keyboard, ui_TextAreaPassword);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_basic_init(dispp);
    lv_disp_set_theme(dispp, theme);
    ui_MainScreen_screen_init();
    lv_disp_load_scr(ui_MainScreen);
}
