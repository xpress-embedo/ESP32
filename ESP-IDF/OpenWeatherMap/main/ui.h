// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: OpenWeatherMap

#ifndef _OPENWEATHERMAP_UI_H
#define _OPENWEATHERMAP_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_Delhi
void ui_Delhi_screen_init(void);
extern lv_obj_t * ui_Delhi;
extern lv_obj_t * ui_delhiImage;
extern lv_obj_t * ui_Temperature;
extern lv_obj_t * ui_Pressure;
extern lv_obj_t * ui_tempValue;
extern lv_obj_t * ui_pressureValue;
// SCREEN: ui_Shimla
void ui_Shimla_screen_init(void);
extern lv_obj_t * ui_Shimla;
extern lv_obj_t * ui_shimlaImage;
// SCREEN: ui_Jaipur
void ui_Jaipur_screen_init(void);
extern lv_obj_t * ui_Jaipur;
extern lv_obj_t * ui_jaipurImage;
// SCREEN: ui_Leh
void ui_Leh_screen_init(void);
extern lv_obj_t * ui_Leh;
extern lv_obj_t * ui_lehImage;
extern lv_obj_t * ui____initial_actions0;

LV_IMG_DECLARE(ui_img_delhi_png);    // assets\Delhi.png
LV_IMG_DECLARE(ui_img_shimla_png);    // assets\Shimla.png
LV_IMG_DECLARE(ui_img_jaipur_png);    // assets\Jaipur.png
LV_IMG_DECLARE(ui_img_leh_png);    // assets\Leh.png

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
