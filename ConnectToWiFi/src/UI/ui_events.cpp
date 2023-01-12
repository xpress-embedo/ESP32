// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: ConnectToWiFi

#include "ui.h"
#include "main.h"

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
  LV_LOG_USER("Ready to be Connected to the Router");
}

void ReScanWiFiSSID(lv_event_t * e)
{
  WiFi_Init();
  WiFi_ScanSSID();
}
