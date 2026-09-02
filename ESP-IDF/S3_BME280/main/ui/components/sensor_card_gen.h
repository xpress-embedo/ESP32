/**
 * @file sensor_card_gen.h
 */

#ifndef SENSOR_CARD_H
#define SENSOR_CARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#ifdef LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * sensor_card_create(lv_obj_t * parent, lv_subject_t * value_text, const void * img_src, lv_subject_t * status_label_text, lv_color_t status_text_color, const char * card_title_text, lv_color_t theme_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SENSOR_CARD_H*/