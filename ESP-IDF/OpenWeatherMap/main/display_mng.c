/*
 * display_mng.c
 *
 *  Created on: 08-Jul-2023
 *      Author: xpress_embedo
 */

#include "display_mng.h"
#include "bsp/esp-bsp.h"
#include "ui.h"

// Public Function Definitions
void display_init(void)
{
  // Start LVGL and LCD Driver
  bsp_display_start();
  bsp_display_lock(0);
  ui_init();
  bsp_display_unlock();
}

void display_mng(void)
{

}
