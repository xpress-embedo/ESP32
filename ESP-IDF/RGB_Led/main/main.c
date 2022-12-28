#include "nvs_flash.h"
#include "rgb_led.h"
#include "wifi_app.h"

void app_main(void)
{
  // Initialize Non-Volatile Storage
  esp_err_t ret = nvs_flash_init();
  if( (ret==ESP_ERR_NVS_NO_FREE_PAGES) || (ret==ESP_ERR_NVS_NEW_VERSION_FOUND) )
  {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  RGB_PWM_Init();

  // Start WiFi
  WiFi_App_Start();

  while (true)
  {
  }
}
