#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "display_mng.h"

void app_main(void)
{
  display_init();
  while (true)
  {
    printf("Hello from app_main!\n");
    sleep(1);
  }
}
