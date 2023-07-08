/*
 * display_mng.c
 *
 *  Created on: 08-Jul-2023
 *      Author: xpress_embedo
 */

#include "display_mng.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"

// Public Function Definitions
void display_init(void)
{
  // Start LVGL and LCD Driver
  bsp_display_start();

  bsp_display_lock(0);
  /*Create a window*/
  lv_obj_t *win = lv_win_create(lv_scr_act(), 40);
  assert(win);
  lv_win_add_title(win, "ESP32-S2-Kaluga-Kit:\nBoard Support Package example");
  bsp_display_unlock();
}
