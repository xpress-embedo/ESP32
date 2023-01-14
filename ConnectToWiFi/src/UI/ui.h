// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: ConnectToWiFi

#ifndef _CONNECTTOWIFI_UI_H
#define _CONNECTTOWIFI_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

extern lv_obj_t * ui_MainScreen;
extern lv_obj_t * ui_Keyboard;
void ui_event_TextAreaPassword(lv_event_t * e);
extern lv_obj_t * ui_TextAreaPassword;
extern lv_obj_t * ui_DropDownSSID;
extern lv_obj_t * ui_WiFiSSIDLabel;
extern lv_obj_t * ui_WiFiPasswordLabel;
void ui_event_ConnectButton(lv_event_t * e);
extern lv_obj_t * ui_ConnectButton;
extern lv_obj_t * ui_ConnectButtonLabel;
void ui_event_RescanButton(lv_event_t * e);
extern lv_obj_t * ui_RescanButton;
extern lv_obj_t * ui_RescanButtonLabel;
extern lv_obj_t * ui_ConnectingLabel;
extern lv_obj_t * ui_ConnectedScreen;
extern lv_obj_t * ui_ConnectedLabel;
extern lv_obj_t * ui_IPAddressLabel;

void ShowKeyBoard(lv_event_t * e);
void HideKeyBoard(lv_event_t * e);
void HideKeyBoard(lv_event_t * e);
void ShowKeyBoard(lv_event_t * e);
void ConnectToRouter(lv_event_t * e);
void ReScanWiFiSSID(lv_event_t * e);





void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
