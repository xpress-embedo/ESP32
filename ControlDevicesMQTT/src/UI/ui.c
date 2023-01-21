// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.2.0
// PROJECT: ControlDevicesMQTT

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_MainScreen;
lv_obj_t * ui_Keyboard;
void ui_event_TextAreaPassword(lv_event_t * e);
lv_obj_t * ui_TextAreaPassword;
lv_obj_t * ui_DropDownSSID;
lv_obj_t * ui_WiFiSSIDLabel;
lv_obj_t * ui_WiFiPasswordLabel;
void ui_event_ConnectButton(lv_event_t * e);
lv_obj_t * ui_ConnectButton;
lv_obj_t * ui_ConnectButtonLabel;
void ui_event_RescanButton(lv_event_t * e);
lv_obj_t * ui_RescanButton;
lv_obj_t * ui_RescanButtonLabel;
lv_obj_t * ui_ConnectingLabel;
lv_obj_t * ui_ConnectedScreen;
lv_obj_t * ui_ConnectedLabel;
lv_obj_t * ui_IPAddressLabel;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_TextAreaPassword(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_FOCUSED) {
        ShowKeyBoard(e);
    }
    if(event_code == LV_EVENT_DEFOCUSED) {
        HideKeyBoard(e);
    }
    if(event_code == LV_EVENT_READY) {
        HideKeyBoard(e);
    }
    if(event_code == LV_EVENT_CLICKED) {
        ShowKeyBoard(e);
    }
}
void ui_event_ConnectButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        ConnectToRouter(e);
    }
}
void ui_event_RescanButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        ReScanWiFiSSID(e);
    }
}

///////////////////// SCREENS ////////////////////
void ui_MainScreen_screen_init(void)
{
    ui_MainScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MainScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Keyboard = lv_keyboard_create(ui_MainScreen);
    lv_obj_set_width(ui_Keyboard, lv_pct(100));
    lv_obj_set_height(ui_Keyboard, lv_pct(45));
    lv_obj_set_align(ui_Keyboard, LV_ALIGN_BOTTOM_MID);

    ui_TextAreaPassword = lv_textarea_create(ui_MainScreen);
    lv_obj_set_width(ui_TextAreaPassword, lv_pct(60));
    lv_obj_set_height(ui_TextAreaPassword, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TextAreaPassword, lv_pct(18));
    lv_obj_set_y(ui_TextAreaPassword, lv_pct(-27));
    lv_obj_set_align(ui_TextAreaPassword, LV_ALIGN_CENTER);
    lv_textarea_set_max_length(ui_TextAreaPassword, 128);
    lv_textarea_set_placeholder_text(ui_TextAreaPassword, "Enter Password");
    lv_textarea_set_password_mode(ui_TextAreaPassword, true);

    ui_DropDownSSID = lv_dropdown_create(ui_MainScreen);
    lv_dropdown_set_options(ui_DropDownSSID, "");
    lv_obj_set_width(ui_DropDownSSID, lv_pct(60));
    lv_obj_set_height(ui_DropDownSSID, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_DropDownSSID, lv_pct(18));
    lv_obj_set_y(ui_DropDownSSID, lv_pct(-40));
    lv_obj_set_align(ui_DropDownSSID, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_DropDownSSID, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_WiFiSSIDLabel = lv_label_create(ui_MainScreen);
    lv_obj_set_width(ui_WiFiSSIDLabel, lv_pct(34));
    lv_obj_set_height(ui_WiFiSSIDLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_WiFiSSIDLabel, lv_pct(-30));
    lv_obj_set_y(ui_WiFiSSIDLabel, lv_pct(-40));
    lv_obj_set_align(ui_WiFiSSIDLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_WiFiSSIDLabel, "WiFi SSID");

    ui_WiFiPasswordLabel = lv_label_create(ui_MainScreen);
    lv_obj_set_width(ui_WiFiPasswordLabel, lv_pct(34));
    lv_obj_set_height(ui_WiFiPasswordLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_WiFiPasswordLabel, lv_pct(-30));
    lv_obj_set_y(ui_WiFiPasswordLabel, lv_pct(-27));
    lv_obj_set_align(ui_WiFiPasswordLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_WiFiPasswordLabel, "WiFi Password");

    ui_ConnectButton = lv_btn_create(ui_MainScreen);
    lv_obj_set_width(ui_ConnectButton, lv_pct(30));
    lv_obj_set_height(ui_ConnectButton, lv_pct(15));
    lv_obj_set_x(ui_ConnectButton, lv_pct(-25));
    lv_obj_set_y(ui_ConnectButton, lv_pct(-10));
    lv_obj_set_align(ui_ConnectButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ConnectButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ConnectButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_ConnectButtonLabel = lv_label_create(ui_ConnectButton);
    lv_obj_set_width(ui_ConnectButtonLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ConnectButtonLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_ConnectButtonLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ConnectButtonLabel, "Connect");
    lv_obj_set_style_text_color(ui_ConnectButtonLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ConnectButtonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_ConnectButtonLabel, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_RescanButton = lv_btn_create(ui_MainScreen);
    lv_obj_set_width(ui_RescanButton, lv_pct(30));
    lv_obj_set_height(ui_RescanButton, lv_pct(15));
    lv_obj_set_x(ui_RescanButton, lv_pct(25));
    lv_obj_set_y(ui_RescanButton, lv_pct(-10));
    lv_obj_set_align(ui_RescanButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_RescanButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RescanButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_RescanButtonLabel = lv_label_create(ui_RescanButton);
    lv_obj_set_width(ui_RescanButtonLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_RescanButtonLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_RescanButtonLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_RescanButtonLabel, "Re-Scan");
    lv_obj_set_style_text_color(ui_RescanButtonLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RescanButtonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_RescanButtonLabel, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ConnectingLabel = lv_label_create(ui_MainScreen);
    lv_obj_set_width(ui_ConnectingLabel, lv_pct(40));
    lv_obj_set_height(ui_ConnectingLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_ConnectingLabel, lv_pct(0));
    lv_obj_set_y(ui_ConnectingLabel, lv_pct(5));
    lv_obj_set_align(ui_ConnectingLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ConnectingLabel, "Connecting.............");
    lv_obj_add_flag(ui_ConnectingLabel, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_set_style_text_color(ui_ConnectingLabel, lv_color_hex(0xE10E0E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ConnectingLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_ConnectingLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_ConnectingLabel, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_keyboard_set_textarea(ui_Keyboard, ui_TextAreaPassword);
    lv_obj_add_event_cb(ui_TextAreaPassword, ui_event_TextAreaPassword, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ConnectButton, ui_event_ConnectButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_RescanButton, ui_event_RescanButton, LV_EVENT_ALL, NULL);

}
void ui_ConnectedScreen_screen_init(void)
{
    ui_ConnectedScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_ConnectedScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_ConnectedLabel = lv_label_create(ui_ConnectedScreen);
    lv_obj_set_width(ui_ConnectedLabel, lv_pct(30));
    lv_obj_set_height(ui_ConnectedLabel, LV_SIZE_CONTENT);    /// 8
    lv_obj_set_x(ui_ConnectedLabel, lv_pct(0));
    lv_obj_set_y(ui_ConnectedLabel, lv_pct(-10));
    lv_obj_set_align(ui_ConnectedLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ConnectedLabel, "Connected");
    lv_obj_set_style_text_align(ui_ConnectedLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_ConnectedLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_IPAddressLabel = lv_label_create(ui_ConnectedScreen);
    lv_obj_set_width(ui_IPAddressLabel, lv_pct(50));
    lv_obj_set_height(ui_IPAddressLabel, LV_SIZE_CONTENT);    /// 8
    lv_obj_set_align(ui_IPAddressLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_IPAddressLabel, "xxx.xxx.xxx.xxx");
    lv_obj_set_style_text_align(ui_IPAddressLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_IPAddressLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_basic_init(dispp);
    lv_disp_set_theme(dispp, theme);
    ui_MainScreen_screen_init();
    ui_ConnectedScreen_screen_init();
    lv_disp_load_scr(ui_MainScreen);
}
