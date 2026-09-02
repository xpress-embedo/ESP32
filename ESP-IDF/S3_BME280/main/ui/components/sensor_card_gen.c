/**
 * @file sensor_card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sensor_card_gen.h"
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

lv_obj_t * sensor_card_create(lv_obj_t * parent, lv_subject_t * value_text, const void * img_src, lv_subject_t * status_label_text, lv_color_t status_text_color, const char * card_title_text, lv_color_t theme_color)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "sensor_card_#");
        lv_obj_set_width(lv_obj_0, lv_pct(31));
        lv_obj_set_height(lv_obj_0, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_cross_place(lv_obj_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(lv_obj_0, 10, 0);

        lv_obj_t * card_box = lv_obj_create(lv_obj_0);
        lv_obj_set_name(card_box, "card_box");
        lv_obj_set_width(card_box, lv_pct(100));
        lv_obj_set_height(card_box, 310);
        lv_obj_set_flag(card_box, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_bg_color(card_box, lv_color_hex(0x1D2939), 0);
        lv_obj_set_style_bg_opa(card_box, 255, 0);
        lv_obj_set_style_radius(card_box, 20, 0);
        lv_obj_set_style_border_width(card_box, 2, 0);
        lv_obj_set_style_border_color(card_box, theme_color, 0);
        lv_obj_set_style_shadow_width(card_box, 20, 0);
        lv_obj_set_style_shadow_color(card_box, theme_color, 0);
        lv_obj_set_style_shadow_opa(card_box, 30, 0);
        lv_obj_set_style_pad_all(card_box, 18, 0);
        lv_obj_set_flex_flow(card_box, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_main_place(card_box, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
        lv_obj_set_style_flex_cross_place(card_box, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_t * value_label = lv_label_create(card_box);
        lv_obj_set_name(value_label, "value_label");
        lv_obj_set_width(value_label, lv_pct(100));
        lv_label_bind_text(value_label, value_text, NULL);
        lv_obj_set_style_text_font(value_label, title_large, 0);
        lv_obj_set_style_text_color(value_label, lv_color_hex(0xFAFAFA), 0);
        lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * center_img = lv_image_create(card_box);
        lv_obj_set_name(center_img, "center_img");
        lv_image_set_src(center_img, img_src);

        lv_obj_t * status_label = lv_label_create(card_box);
        lv_obj_set_name(status_label, "status_label");
        lv_obj_set_width(status_label, lv_pct(100));
        lv_label_bind_text(status_label, status_label_text, NULL);
        lv_obj_set_style_text_font(status_label, body_normal, 0);
        lv_obj_set_style_text_color(status_label, status_text_color, 0);
        lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * title_label = lv_label_create(lv_obj_0);
        lv_obj_set_name(title_label, "title_label");
        lv_obj_set_width(title_label, lv_pct(100));
        lv_label_set_text(title_label, card_title_text);
        lv_obj_set_style_text_color(title_label, lv_color_hex(0xCAD5E2), 0);
        lv_obj_set_style_text_font(title_label, body_normal, 0);
        lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

