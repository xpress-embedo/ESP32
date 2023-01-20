// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.0
// LVGL VERSION: 8.2.0
// PROJECT: LVGL_ImageSlider

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_MainScreen;
lv_obj_t * ui_MainImage;
void ui_event_SliderRotate(lv_event_t * e);
lv_obj_t * ui_SliderRotate;
void ui_event_SliderScale(lv_event_t * e);
lv_obj_t * ui_SliderScale;
lv_obj_t * ui_DegreeLabel;
lv_obj_t * ui_ScaleLabel;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_SliderRotate(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_slider_set_text_value(ui_DegreeLabel, target, "", "");
        ChangeImageAngle(e);
    }
}
void ui_event_SliderScale(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_slider_set_text_value(ui_ScaleLabel, target, "", "");
        ChangeImageScale(e);
    }
}

///////////////////// SCREENS ////////////////////
void ui_MainScreen_screen_init(void)
{
    ui_MainScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MainScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_MainScreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MainScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MainImage = lv_img_create(ui_MainScreen);
    lv_img_set_src(ui_MainImage, &ui_img_multicolor_png);
    lv_obj_set_width(ui_MainImage, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MainImage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MainImage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_MainImage, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_MainImage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_SliderRotate = lv_slider_create(ui_MainScreen);
    lv_slider_set_range(ui_SliderRotate, 0, 360);
    lv_obj_set_width(ui_SliderRotate, 10);
    lv_obj_set_height(ui_SliderRotate, 200);
    lv_obj_set_x(ui_SliderRotate, -140);
    lv_obj_set_y(ui_SliderRotate, 0);
    lv_obj_set_align(ui_SliderRotate, LV_ALIGN_CENTER);

    ui_SliderScale = lv_slider_create(ui_MainScreen);
    lv_slider_set_value(ui_SliderScale, 100, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_SliderScale) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_SliderScale, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_SliderScale, 10);
    lv_obj_set_height(ui_SliderScale, 200);
    lv_obj_set_x(ui_SliderScale, 140);
    lv_obj_set_y(ui_SliderScale, 0);
    lv_obj_set_align(ui_SliderScale, LV_ALIGN_CENTER);

    ui_DegreeLabel = lv_label_create(ui_MainScreen);
    lv_obj_set_width(ui_DegreeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DegreeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DegreeLabel, -140);
    lv_obj_set_y(ui_DegreeLabel, -110);
    lv_obj_set_align(ui_DegreeLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_DegreeLabel, "0");

    ui_ScaleLabel = lv_label_create(ui_MainScreen);
    lv_obj_set_width(ui_ScaleLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ScaleLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_ScaleLabel, 140);
    lv_obj_set_y(ui_ScaleLabel, -110);
    lv_obj_set_align(ui_ScaleLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ScaleLabel, "100");

    lv_obj_add_event_cb(ui_SliderRotate, ui_event_SliderRotate, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_SliderScale, ui_event_SliderScale, LV_EVENT_ALL, NULL);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_basic_init(dispp);
    lv_disp_set_theme(dispp, theme);
    ui_MainScreen_screen_init();
    lv_disp_load_scr(ui_MainScreen);
}
