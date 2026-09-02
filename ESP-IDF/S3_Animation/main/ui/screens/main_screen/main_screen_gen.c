/**
 * @file main_screen_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "main_screen_gen.h"
#include "../../ui.h"

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
        lv_obj_set_width(lv_obj_0, lv_pct(100));
        lv_obj_set_height(lv_obj_0, lv_pct(100));

        lv_obj_t * img_bg = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_bg, "img_bg");
        lv_image_set_src(img_bg, background);
        lv_obj_set_width(img_bg, lv_pct(100));
        lv_obj_set_height(img_bg, lv_pct(100));
        lv_obj_set_align(img_bg, LV_ALIGN_CENTER);

        lv_obj_t * img_cloud = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_cloud, "img_cloud");
        lv_image_set_src(img_cloud, cloud);
        lv_obj_set_x(img_cloud, 0);
        lv_obj_set_y(img_cloud, 30);

        lv_obj_t * img_balloon = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_balloon, "img_balloon");
        lv_image_set_src(img_balloon, balloon);
        lv_obj_set_x(img_balloon, 736);
        lv_obj_set_y(img_balloon, 150);

        lv_obj_t * img_dog_r1 = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_dog_r1, "img_dog_r1");
        lv_image_set_src(img_dog_r1, dog_walk1);
        lv_obj_set_align(img_dog_r1, LV_ALIGN_BOTTOM_LEFT);
        lv_obj_set_x(img_dog_r1, 0);
        lv_obj_set_y(img_dog_r1, -30);

        lv_obj_t * img_dog_r2 = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_dog_r2, "img_dog_r2");
        lv_image_set_src(img_dog_r2, dog_walk2);
        lv_obj_set_align(img_dog_r2, LV_ALIGN_BOTTOM_LEFT);
        lv_obj_set_x(img_dog_r2, 0);
        lv_obj_set_y(img_dog_r2, -30);
        lv_obj_set_style_opa(img_dog_r2, (255 * 0 / 100), 0);

        lv_obj_t * img_dog_l1 = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_dog_l1, "img_dog_l1");
        lv_image_set_src(img_dog_l1, dog_walk1_r);
        lv_obj_set_align(img_dog_l1, LV_ALIGN_BOTTOM_LEFT);
        lv_obj_set_x(img_dog_l1, 0);
        lv_obj_set_y(img_dog_l1, -30);
        lv_obj_set_style_opa(img_dog_l1, (255 * 0 / 100), 0);

        lv_obj_t * img_dog_l2 = lv_image_create(lv_obj_0);
        lv_obj_set_name(img_dog_l2, "img_dog_l2");
        lv_image_set_src(img_dog_l2, dog_walk2_r);
        lv_obj_set_align(img_dog_l2, LV_ALIGN_BOTTOM_LEFT);
        lv_obj_set_x(img_dog_l2, 0);
        lv_obj_set_y(img_dog_l2, -30);
        lv_obj_set_style_opa(img_dog_l2, (255 * 0 / 100), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

