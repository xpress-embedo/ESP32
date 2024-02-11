// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: ESP32_Clock

#ifndef _ESP32_CLOCK_UI_H
#define _ESP32_CLOCK_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_MainScreen
void ui_MainScreen_screen_init(void);
extern lv_obj_t * ui_MainScreen;
extern lv_obj_t * ui_imgBackground;
extern lv_obj_t * ui_imgSecond;
extern lv_obj_t * ui____initial_actions0;

LV_IMG_DECLARE(ui_img_bg_png);    // assets\bg.png
LV_IMG_DECLARE(ui_img_clock_sec_png);    // assets\clock_sec.png



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
