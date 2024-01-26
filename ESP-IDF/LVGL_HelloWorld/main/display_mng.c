/*
 * display_mng.c
 *
 *  Created on: Jan 26, 2024
 *      Author: xpress_embedo
 */

#include "display_mng.h"

// Public Function Definitions
void display_init( void )
{

  tft_init();
  xpt2046_init();
}
