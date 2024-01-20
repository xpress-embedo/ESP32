#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "tft.h"

void app_main(void)
{
  tft_init();

  while (true)
  {
    xpt2046_read();
    ili9341_fill( ILI9341_DARKGREEN );
    tft_delay_ms(250);
    ili9341_fill( ILI9341_DARKCYAN );
    tft_delay_ms(250);
    // printf("Hello from app_main!\n");
    // lv_timer_handler();
  }
}
