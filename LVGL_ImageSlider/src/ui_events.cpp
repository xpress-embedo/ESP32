// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.2.0
// PROJECT: LVGL_ImageSlider

#include "ui.h"

void ChangeImageAngle(lv_event_t * e)
{
  lv_img_set_angle(ui_MainImage, lv_slider_get_value(ui_SliderRotate)*10);
}

void ChangeImageScale(lv_event_t * e)
{
  uint16_t zoom_value = 256*lv_slider_get_value(ui_SliderScale)/100;
  lv_img_set_zoom(ui_MainImage, zoom_value);
}
