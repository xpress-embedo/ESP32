/*
 * ili9341.c
 *
 *  Created on: Dec 16, 2023
 *      Author: xpress_embedo
 */
#include "ili9341.h"
#include "display_mng.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Defines
 #define TAG "ILI9341"

// structures
// The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct
typedef struct
{
  uint8_t cmd;
  uint8_t data[16];
  uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

// Private Function Prototypes
static void ili9341_set_orientation(uint8_t orientation);
static void ili9341_send_cmd(uint8_t cmd);
static void ili9341_send_data(void * data, uint16_t length);
static void ili9341_send_color(void * data, uint16_t length);

// Private Variables

// Public Function Definition
void ili9341_init( void )
{
	lcd_init_cmd_t ili_init_cmds[]=
	{
    /* This is an un-documented command
    https://forums.adafruit.com/viewtopic.php?f=47&t=63229&p=320378&hilit=0xef+ili9341#p320378
    */
    {0xEF, {0x03, 0x80, 0x02}, 3},
	  /* Power contorl B, power control = 0, DC_ENA = 1 */
		{0xCF, {0x00, 0x83, 0X30}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
		{0xED, {0x64, 0x03, 0X12, 0X81}, 4},
		/* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
		{0xE8, {0x85, 0x01, 0x79}, 3},
		/* Power control A, Vcore=1.6V, DDVDH=5.6V */
		{0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
		/* Pump ratio control, DDVDH=2xVCl */
		{0xF7, {0x20}, 1},
		/* Driver timing control, all=0 unit */
		{0xEA, {0x00, 0x00}, 2},
		/* Power control 1, GVDD=4.75V */
		{0xC0, {0x26}, 1},
		/* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
		{0xC1, {0x11}, 1},
		/* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
		{0xC5, {0x35, 0x3E}, 2},
		/* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
		{0xC7, {0xBE}, 1},
		/* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
		{0x36, {0x28}, 1},
		/* Pixel format, 16bits/pixel for RGB/MCU interface */
		{0x3A, {0x55}, 1},
		/* Frame rate control, f=fosc, 70Hz fps */
		{0xB1, {0x00, 0x1B}, 2},
		/* Enable 3G, disabled */
		{0xF2, {0x08}, 1},
		/* Gamma set, curve 1 */
		{0x26, {0x01}, 1},
		/* Positive gamma correction */
		{0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
		/* Negative gamma correction */
		{0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
		/* Column address set, SC=0, EC=0xEF */
		{0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
		/* Page address set, SP=0, EP=0x013F */
		{0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
		/* Memory write */
		{0x2C, {0}, 0},
		/* Entry mode set, Low vol detect disabled, normal display */
		{0xB7, {0x07}, 1},
		/* Display function control */
		{0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
		/* test command, need analysis */
		{0x36, {0x48}, 1},
		/* Sleep out */
		{0x11, {0}, 0x80},
		/* Display on */
		{0x29, {0}, 0x80},
		{0, {0}, 0xff},
	};

	//Initialize non-SPI GPIOs
	gpio_config_t io_conf = {};
#if (ILI9341_USE_RST == true )
	io_conf.pin_bit_mask = ((1u<<ILI9341_DC) | (1u<<ILI9341_RST) );
#else
	io_conf.pin_bit_mask = (1u<<ILI9341_DC);
#endif
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pull_up_en = true;
	gpio_config(&io_conf);
	// TODO: XE to be removed, alternative solution is written above
	// gpio_pad_select_gpio(ILI9341_DC);
	// gpio_set_direction(ILI9341_DC, GPIO_MODE_OUTPUT);

#if ILI9341_USE_RST
	// gpio_pad_select_gpio(ILI9341_RST);
	// gpio_set_direction(ILI9341_RST, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(ILI9341_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(ILI9341_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
#endif

	ESP_LOGI(TAG, "LCD ILI9341 Initialization.");

	//Send all the commands
	uint16_t cmd = 0;
	while (ili_init_cmds[cmd].databytes!=0xff)
	{
		ili9341_send_cmd( ili_init_cmds[cmd].cmd );
		ili9341_send_data( ili_init_cmds[cmd].data, (ili_init_cmds[cmd].databytes & 0x1F) );
		if (ili_init_cmds[cmd].databytes & 0x80)
		{
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		cmd++;
	}

	// TODO: XE
	// ili9341_set_orientation( 2 );

//#if ILI9341_INVERT_COLORS == 1
//	ili9341_send_cmd(0x21);
//#else
//	ili9341_send_cmd(0x20);
//#endif
}


//void ili9341_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
//{
//	uint8_t data[4];
//
//	// Column addresses
//	ili9341_send_cmd(0x2A);
//	data[0] = (area->x1 >> 8) & 0xFF;
//	data[1] = area->x1 & 0xFF;
//	data[2] = (area->x2 >> 8) & 0xFF;
//	data[3] = area->x2 & 0xFF;
//	ili9341_send_data(data, 4);
//
//	// Page addresses
//	ili9341_send_cmd(0x2B);
//	data[0] = (area->y1 >> 8) & 0xFF;
//	data[1] = area->y1 & 0xFF;
//	data[2] = (area->y2 >> 8) & 0xFF;
//	data[3] = area->y2 & 0xFF;
//	ili9341_send_data(data, 4);
//
//	// Memory write
//	ili9341_send_cmd(0x2C);
//	uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);
//	ili9341_send_color((void*)color_map, size * 2);
//}

void ili9341_sleep_in( void )
{
	uint8_t data[] = {0x08};
	ili9341_send_cmd(0x10);
	ili9341_send_data(&data, 1);
}

void ili9341_sleep_out( void )
{
	uint8_t data[] = {0x08};
	ili9341_send_cmd(0x11);
	ili9341_send_data(&data, 1);
}

// Private Function Definitions
static void ili9341_send_cmd(uint8_t cmd)
{
  display_send_cmd( cmd );
  // TODO: XE
  // disp_wait_for_pending_transactions();
  // gpio_set_level(ILI9341_DC, 0);	  // command mode
  // disp_spi_send_data(&cmd, 1);
}

static void ili9341_send_data(void * data, uint16_t length)
{
  display_send_data(data, length);
  // TODO: XE
  // disp_wait_for_pending_transactions();
  // gpio_set_level(ILI9341_DC, 1);	  // data mode
  // disp_spi_send_data(data, length);
}

static void ili9341_send_color(void * data, uint16_t length)
{
  // TODO: XE
  // disp_wait_for_pending_transactions();
  // gpio_set_level(ILI9341_DC, 1);    // Data Mode
  // disp_spi_send_colors(data, length);
}

static void ili9341_set_orientation(uint8_t orientation)
{
  // ESP_ASSERT(orientation < 4);
  const char *orientation_str[] =
  {
    "PORTRAIT", "PORTRAIT_INVERTED", "LANDSCAPE", "LANDSCAPE_INVERTED"
  };

  ESP_LOGI(TAG, "Display orientation: %s", orientation_str[orientation]);

  uint8_t data[] = {0x48, 0x68, 0x08, 0x08};
#if defined CONFIG_LV_PREDEFINED_DISPLAY_M5STACK
  uint8_t data[] = {0x68, 0x68, 0x08, 0x08};
#elif defined (CONFIG_LV_PREDEFINED_DISPLAY_M5CORE2)
	uint8_t data[] = {0x08, 0x88, 0x28, 0xE8};
#elif defined (CONFIG_LV_PREDEFINED_DISPLAY_WROVER4)
  uint8_t data[] = {0x6C, 0xEC, 0xCC, 0x4C};
#elif defined (CONFIG_LV_PREDEFINED_DISPLAY_NONE)
  uint8_t data[] = {0x48, 0x88, 0x28, 0xE8};
#endif

  // TODO: XE
  // ESP_LOGI(TAG, "0x36 command value: 0x%02X", data[orientation]);

  ili9341_send_cmd(0x36);
  // TODO: XE
  ili9341_send_data((void *) &data[orientation], 1);
}
