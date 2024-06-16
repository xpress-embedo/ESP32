#include <stdbool.h>
#include "driver/ledc.h"
#include "rgb_led.h"

static Led_Info_s led_ctrl_channel[RGB_LED_CHANNEL_NUM];

// Private function declaration
static void RGB_SetColor( uint8_t red, uint8_t green, uint8_t blue );

// Public function Definition
/*
 * Initialize the RGB LED settings per channel, including the GPIO for each
 * color, mode and timer configuration
 */
void RGB_PWM_Init( void )
{
  uint8_t rgb_ch;

  // RED
  led_ctrl_channel[0].channel = LEDC_CHANNEL_0;
  led_ctrl_channel[0].gpio = RGB_LED_RED_GPIO;
#if SOC_LEDC_SUPPORT_HS_MODE
  led_ctrl_channel[0].mode = LEDC_HIGH_SPEED_MODE;
#endif
  led_ctrl_channel[0].timer_idx = LEDC_TIMER_0;

  // Green
  led_ctrl_channel[1].channel = LEDC_CHANNEL_1;
  led_ctrl_channel[1].gpio = RGB_LED_GREEN_GPIO;
#if SOC_LEDC_SUPPORT_HS_MODE
  led_ctrl_channel[1].mode = LEDC_HIGH_SPEED_MODE;
#endif
  led_ctrl_channel[1].timer_idx = LEDC_TIMER_0;

  // Blue
  led_ctrl_channel[2].channel = LEDC_CHANNEL_2;
  led_ctrl_channel[2].gpio = RGB_LED_BLUE_GPIO;
#if SOC_LEDC_SUPPORT_HS_MODE
  led_ctrl_channel[2].mode = LEDC_HIGH_SPEED_MODE;
#endif
  led_ctrl_channel[2].timer_idx = LEDC_TIMER_0;

  /* Documentation available on the below mentioned page
  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
  Timer Configuration: by specifying the PWM signal's frequency and duty cycle
                       resolution.
  Channel Configuration: by associating it with the timer and GPIO to output the
                         PWM signal.
  Change PWM Signal: that drives the output in order to change LED's intensity.
                     This can be done under the full control of software or with
                     the hardware fading functions.
  */

  // Configure timer zero
  ledc_timer_config_t ledc_timer =
  {
#if SOC_LEDC_SUPPORT_HS_MODE
    .speed_mode = LEDC_HIGH_SPEED_MODE,
#endif
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 100,     // 100 Hz
  };
  ledc_timer_config( &ledc_timer );

  // Configure the Channels
  for( rgb_ch=0; rgb_ch<RGB_LED_CHANNEL_NUM; rgb_ch++ )
  {
    ledc_channel_config_t ledc_channel =
    {
      .gpio_num = led_ctrl_channel[rgb_ch].gpio,
      .speed_mode = led_ctrl_channel[rgb_ch].mode,
      .channel = led_ctrl_channel[rgb_ch].channel,
      .timer_sel = led_ctrl_channel[rgb_ch].timer_idx,
      .duty = 0,
      .hpoint = 0,
      .intr_type = LEDC_INTR_DISABLE,
    };
    ledc_channel_config( &ledc_channel );
  }
}
/*
 * Color to Indicate WiFi application has started
 */
void RGB_WiFi_Started( void )
{
  RGB_SetColor(255, 102, 255);
  // RGB_SetColor(255, 0, 0);
}

/*
 * Color to Indicate that the HTTP Server has started
 */
void RGB_HTTP_ServerStarted( void )
{
  RGB_SetColor(204, 255, 51);
  // RGB_SetColor(0, 255, 0);
}

/*
 * Color to Indicate that the ESP32 is connected to an access point.
 */
void RGB_WiFi_Connected( void )
{
  RGB_SetColor(0, 255, 153);
  // RGB_SetColor(0, 0, 255);
}

// Private function definition


/*
 * Sets the RGB Color
 */
static void RGB_SetColor( uint8_t red, uint8_t green, uint8_t blue )
{
  // value should be between 0 to 255, and that's the purpose of using uint8_t
  // type variable, also the reason is because we have specified the duty
  // resolution as 8-bit
  ledc_set_duty(led_ctrl_channel[0].mode, led_ctrl_channel[0].channel, red);
  ledc_update_duty(led_ctrl_channel[0].mode, led_ctrl_channel[0].channel);

  ledc_set_duty(led_ctrl_channel[1].mode, led_ctrl_channel[1].channel, green);
  ledc_update_duty(led_ctrl_channel[1].mode, led_ctrl_channel[1].channel);

  ledc_set_duty(led_ctrl_channel[2].mode, led_ctrl_channel[2].channel, blue);
  ledc_update_duty(led_ctrl_channel[2].mode, led_ctrl_channel[2].channel);
}
