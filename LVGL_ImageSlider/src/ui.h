// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.2.0
// PROJECT: LVGL_ImageSlider

#ifndef _LVGL_IMAGESLIDER_UI_H
#define _LVGL_IMAGESLIDER_UI_H

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
extern lv_obj_t * ui_MainImage;
void ui_event_SliderRotate(lv_event_t * e);
extern lv_obj_t * ui_SliderRotate;
void ui_event_SliderScale(lv_event_t * e);
extern lv_obj_t * ui_SliderScale;
extern lv_obj_t * ui_DegreeLabel;
extern lv_obj_t * ui_ScaleLabel;

void ChangeImageAngle(lv_event_t * e);
void ChangeImageScale(lv_event_t * e);

LV_IMG_DECLARE(ui_img_multicolor_png);    // assets\MultiColor.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
