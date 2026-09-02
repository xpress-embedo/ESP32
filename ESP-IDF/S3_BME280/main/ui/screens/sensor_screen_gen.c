/**
 * @file sensor_screen_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sensor_screen_gen.h"
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

lv_obj_t * sensor_screen_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "sensor_screen_#");
        lv_obj_set_width(lv_obj_0, lv_pct(100));
        lv_obj_set_height(lv_obj_0, lv_pct(100));
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x0B0F19), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, 255, 0);
        lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_main_place(lv_obj_0, LV_FLEX_ALIGN_START, 0);
        lv_obj_set_style_flex_cross_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_top(lv_obj_0, 18, 0);
        lv_obj_set_style_pad_bottom(lv_obj_0, 18, 0);
        lv_obj_set_style_pad_left(lv_obj_0, 24, 0);
        lv_obj_set_style_pad_right(lv_obj_0, 24, 0);
        lv_obj_set_style_pad_row(lv_obj_0, 20, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_1, lv_pct(100));
        lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_1, 0, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, 0, 0);
        lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_main_place(lv_obj_1, LV_FLEX_ALIGN_START, 0);
        lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
        lv_obj_set_width(lv_obj_2, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x0E2A1E), 0);
        lv_obj_set_style_bg_opa(lv_obj_2, 220, 0);
        lv_obj_set_style_border_width(lv_obj_2, 1, 0);
        lv_obj_set_style_border_color(lv_obj_2, lv_color_hex(0x2ECC71), 0);
        lv_obj_set_style_radius(lv_obj_2, 20, 0);
        lv_obj_set_style_pad_top(lv_obj_2, 6, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 6, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 14, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 14, 0);
        lv_obj_set_style_pad_column(lv_obj_2, 8, 0);
        lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_cross_place(lv_obj_2, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_2);
        lv_obj_set_width(lv_obj_3, 10);
        lv_obj_set_height(lv_obj_3, 10);
        lv_obj_set_style_radius(lv_obj_3, 10, 0);
        lv_obj_set_style_bg_color(lv_obj_3, lv_color_hex(0x2ECC71), 0);
        lv_obj_set_style_bg_opa(lv_obj_3, 255, 0);
        lv_obj_set_style_border_width(lv_obj_3, 0, 0);

        lv_obj_t * status_label = lv_label_create(lv_obj_2);
        lv_obj_set_name(status_label, "status_label");
        lv_label_set_text(status_label, "BME280: Connected");
        lv_obj_set_style_text_font(status_label, body_medium, 0);
        lv_obj_set_style_text_color(status_label, lv_color_hex(0x2ECC71), 0);

        lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
        lv_obj_set_width(lv_obj_4, lv_pct(100));
        lv_obj_set_height(lv_obj_4, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_4, 0, 0);
        lv_obj_set_style_border_width(lv_obj_4, 0, 0);
        lv_obj_set_flex_flow(lv_obj_4, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_flex_main_place(lv_obj_4, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
        lv_obj_set_style_flex_cross_place(lv_obj_4, LV_FLEX_ALIGN_START, 0);
        lv_obj_set_style_pad_column(lv_obj_4, 20, 0);
        sensor_card_create(lv_obj_4, &temp_str, thermometer_img, &temp_status_str, lv_color_hex(0x8FA0B8), "Temperature Card", lv_color_hex(0xF39C12));

        sensor_card_create(lv_obj_4, &hum_str, humidity_img, &hum_status_str, lv_color_hex(0x00E5FF), "Humidity Card", lv_color_hex(0x00E5FF));

        sensor_card_create(lv_obj_4, &press_str, pressure_img, &press_status_str, lv_color_hex(0xBD10E0), "Barometric Pressure Card", lv_color_hex(0xBD10E0));

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

