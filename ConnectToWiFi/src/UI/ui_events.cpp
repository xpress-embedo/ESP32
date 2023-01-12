// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: ConnectToWiFi

#include "ui.h"
#include "main.h"
#include "WiFi.h"

#define WIFI_SSID_BUFFER_SIZE                     (20u)
#define WIFI_PSWD_BUFFER_SIZE                     (20u)

char wifi_ssid[WIFI_SSID_BUFFER_SIZE] = { 0 };
char wifi_pswd[WIFI_PSWD_BUFFER_SIZE] = { 0 };

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
  // first step is to get the ssid name and password
  lv_dropdown_get_selected_str(ui_DropDownSSID, wifi_ssid, WIFI_SSID_BUFFER_SIZE);
  LV_LOG_USER("WIFI SSID: %s", wifi_ssid );
  LV_LOG_USER("WIFI PSWD: %s", lv_textarea_get_text(ui_TextAreaPassword) );

  // Try to Connect with the Router
  WiFi.begin( wifi_ssid, lv_textarea_get_text(ui_TextAreaPassword) );

  while( WiFi.status() != WL_CONNECTED )
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void ReScanWiFiSSID(lv_event_t * e)
{
  static lv_obj_t *msg_box;
  // msgbox not working, reason is known, will work on this later
  // msg_box = lv_msgbox_create( NULL, "Re-Scanning", \
  //                             "Please Wait, getting WiFi SSID", \
  //                             NULL, false );
  // lv_obj_center(msg_box);
  WiFi_Init();
  WiFi_ScanSSID();
  // lv_msgbox_close( msg_box );
}

