#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "gui_mng.h"
#include "lvgl.h"

// Private Variables
static uint8_t button_counter = 0;
static lv_obj_t * count_label;

// Private Function Declarations
void lv_button_demo(void);
static void event_handler(lv_event_t * e);

void app_main(void)
{
  gui_start();

  lv_button_demo();

  while (true)
  {
    printf("Hello World from Main\n");
    sleep(10);
  }
}

// Private Function Definitions
/**
 * @brief Simple Button Screen containing two labels and two buttons
 * @param  None
 */
void lv_button_demo(void)
{
  lv_obj_t * label;

  lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_text_font(btn1, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

  label = lv_label_create(btn1);
  lv_label_set_text(label, "Button");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(label);

  count_label = lv_label_create(lv_scr_act());
  lv_obj_align(count_label, LV_ALIGN_CENTER, 0, -100);
  lv_label_set_text(count_label, "Counts: 0");
  lv_obj_set_style_text_font(count_label, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_height(btn2, LV_SIZE_CONTENT);

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Toggle");
  lv_obj_set_style_text_font(btn2, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(label);
}

/**
 * @brief Event Handler Function for Button Clicked Events
 * @param e lvgl event
 */
static void event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if( (code == LV_EVENT_CLICKED) || (code ==  LV_EVENT_LONG_PRESSED_REPEAT) )
  {
    if ( code == LV_EVENT_CLICKED)
      LV_LOG_USER("Click Event");
    else if( code == LV_EVENT_LONG_PRESSED_REPEAT )
      LV_LOG_USER("Press and Hold Event");
    button_counter++;
    lv_label_set_text_fmt(count_label, "Count: %d", button_counter);
  }
  else if(code == LV_EVENT_VALUE_CHANGED)
  {
    LV_LOG_USER("Toggle Event");
  }
}
