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
extern lv_obj_t * ui_DopDownSSID;
extern lv_obj_t * ui_WiFiSSIDLabel;
extern lv_obj_t * ui_WiFiPasswordLabel;

void ShowKeyBoard(lv_event_t * e);





void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
