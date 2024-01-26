#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "tft.h"

int16_t x = 0;
int16_t y = 0;

void app_main(void)
{
  tft_init();

  while (true)
  {
    if( xpt2046_read(&x, &y) )
    {
      printf("X = %d, Y = %d\n\n", x, y);
    }
    tft_delay_ms(10);
    // printf("Hello from app_main!\n");
    // lv_timer_handler();
  }
}
