// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: ConnectToWiFi

#include "ui.h"
#include "main.h"
#include "WiFi.h"
#include "display_mng.h"

#define WIFI_SSID_BUFFER_SIZE                     (20u)

static char wifi_ssid[WIFI_SSID_BUFFER_SIZE] = { 0 };

void ShowKeyBoard(lv_event_t * e)
{
  // Show the keyboard by clearing the hidden flag
  lv_obj_clear_flag( ui_Keyboard, LV_OBJ_FLAG_HIDDEN );
}

void HideKeyBoard(lv_event_t * e)
{
  // Hide the keyboard by clearing the hidden flag
  lv_obj_add_flag( ui_Keyboard, LV_OBJ_FLAG_HIDDEN );
}

void ConnectToRouter(lv_event_t * e)
{
  uint8_t failed = 0;
  LV_LOG_USER("Ready to be Connected to the Router");
  // first step is to get the ssid name and password
  lv_dropdown_get_selected_str(ui_DropDownSSID, wifi_ssid, WIFI_SSID_BUFFER_SIZE);
  LV_LOG_USER("WIFI SSID: %s", wifi_ssid );
  LV_LOG_USER("WIFI PSWD: %s", lv_textarea_get_text(ui_TextAreaPassword) );

  // Try to Connect with the Router
  WiFi.begin( wifi_ssid, lv_textarea_get_text(ui_TextAreaPassword) );
  // enabling the connecting label, we can display a sort of animation also here
  lv_obj_clear_flag( ui_ConnectingLabel, LV_OBJ_FLAG_HIDDEN );
  // Change the state
  Display_ChangeState(DISP_STATE_CONNECTING_MENU_WAIT);
}

void ReScanWiFiSSID(lv_event_t * e)
{
  Display_ChangeState(DISP_STATE_INIT);
}

