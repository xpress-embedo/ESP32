/**
 * @file main_screen_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "main_screen_gen.h"
#include "../ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * main_screen_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "main_screen_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x121824), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, 255, 0);
        lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_main_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(lv_obj_0, 45, 0);

        lv_obj_t * head_label = lv_label_create(lv_obj_0);
        lv_obj_set_name(head_label, "head_label");
        lv_label_set_text(head_label, "Red Green Blue Mixer");
        lv_obj_set_style_text_font(head_label, title_large, 0);
        lv_obj_set_style_text_align(head_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(head_label, lv_color_hex(0xFAFAFA), 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_1, lv_pct(90));
        lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_1, 0, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_top(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_1, 14, 0);
        lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(lv_obj_1, 10, 0);
        lv_obj_t * red_slider = lv_slider_create(lv_obj_1);
        lv_obj_set_name(red_slider, "red_slider");
        lv_obj_set_flex_grow(red_slider, 1);
        lv_slider_set_min_value(red_slider, 0);
        lv_slider_set_max_value(red_slider, 255);
        lv_slider_bind_value(red_slider, &red_val);
        lv_obj_set_style_bg_color(red_slider, lv_color_hex(0x330000), 0);
        lv_obj_set_style_bg_opa(red_slider, 255, 0);
        lv_obj_set_style_bg_color(red_slider, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(red_slider, 255, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(red_slider, lv_color_hex(0xFF0000), LV_PART_KNOB);
        lv_obj_set_style_bg_opa(red_slider, 255, LV_PART_KNOB);

        lv_obj_t * red_label = lv_label_create(lv_obj_1);
        lv_obj_set_name(red_label, "red_label");
        lv_obj_set_width(red_label, 60);
        lv_label_bind_text(red_label, &red_val, "%3d");
        lv_obj_set_style_text_font(red_label, body_medium, 0);
        lv_obj_set_style_text_align(red_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(red_label, lv_color_hex(0xFAFAFA), 0);

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_2, lv_pct(90));
        lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_2, 0, 0);
        lv_obj_set_style_border_width(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_top(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 14, 0);
        lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_cross_place(lv_obj_2, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(lv_obj_2, 10, 0);
        lv_obj_t * green_slider = lv_slider_create(lv_obj_2);
        lv_obj_set_name(green_slider, "green_slider");
        lv_obj_set_flex_grow(green_slider, 1);
        lv_slider_set_min_value(green_slider, 0);
        lv_slider_set_max_value(green_slider, 255);
        lv_slider_bind_value(green_slider, &green_val);
        lv_obj_set_style_bg_color(green_slider, lv_color_hex(0x003300), 0);
        lv_obj_set_style_bg_opa(green_slider, 255, 0);
        lv_obj_set_style_bg_color(green_slider, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(green_slider, 255, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(green_slider, lv_color_hex(0x00FF00), LV_PART_KNOB);
        lv_obj_set_style_bg_opa(green_slider, 255, LV_PART_KNOB);

        lv_obj_t * green_label = lv_label_create(lv_obj_2);
        lv_obj_set_name(green_label, "green_label");
        lv_obj_set_width(green_label, 60);
        lv_label_bind_text(green_label, &green_val, "%3d");
        lv_obj_set_style_text_font(green_label, body_medium, 0);
        lv_obj_set_style_text_align(green_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(green_label, lv_color_hex(0xFAFAFA), 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_3, lv_pct(90));
        lv_obj_set_height(lv_obj_3, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_3, 0, 0);
        lv_obj_set_style_border_width(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_top(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_3, 14, 0);
        lv_obj_set_flex_flow(lv_obj_3, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_cross_place(lv_obj_3, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(lv_obj_3, 10, 0);
        lv_obj_t * blue_slider = lv_slider_create(lv_obj_3);
        lv_obj_set_name(blue_slider, "blue_slider");
        lv_obj_set_flex_grow(blue_slider, 1);
        lv_slider_set_min_value(blue_slider, 0);
        lv_slider_set_max_value(blue_slider, 255);
        lv_slider_bind_value(blue_slider, &blue_val);
        lv_obj_set_style_bg_color(blue_slider, lv_color_hex(0x000033), 0);
        lv_obj_set_style_bg_opa(blue_slider, 255, 0);
        lv_obj_set_style_bg_color(blue_slider, lv_color_hex(0x0000FF), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(blue_slider, 255, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(blue_slider, lv_color_hex(0x0000FF), LV_PART_KNOB);
        lv_obj_set_style_bg_opa(blue_slider, 255, LV_PART_KNOB);

        lv_obj_t * blue_label = lv_label_create(lv_obj_3);
        lv_obj_set_name(blue_label, "blue_label");
        lv_obj_set_width(blue_label, 60);
        lv_label_bind_text(blue_label, &blue_val, "%3d");
        lv_obj_set_style_text_font(blue_label, body_medium, 0);
        lv_obj_set_style_text_align(blue_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(blue_label, lv_color_hex(0xFAFAFA), 0);

        lv_obj_t * rgb_mix_rectangle = lv_obj_create(lv_obj_0);
        lv_obj_set_name(rgb_mix_rectangle, "rgb_mix_rectangle");
        lv_obj_set_width(rgb_mix_rectangle, lv_pct(90));
        lv_obj_set_height(rgb_mix_rectangle, lv_pct(30));
        lv_obj_set_style_bg_color(rgb_mix_rectangle, lv_color_hex(0x000000), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

