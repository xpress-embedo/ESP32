#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "gui_mng.h"

void app_main(void)
{
  gui_start();

  while (true)
  {
    printf("Hello from app_main!\n");
    sleep(5);
  }
}
