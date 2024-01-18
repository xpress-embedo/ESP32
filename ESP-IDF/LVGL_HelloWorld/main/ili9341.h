/*
 * ili9341.h
 *
 *  Created on: Dec 16, 2023
 *      Author: xpress_embedo
 */
#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <unistd.h>

//#include "lvgl.h"

// Defines
#define ILI9341_DC                    (DISP_PIN_DC)
#define ILI9341_USE_RST               (false)
#define ILI9341_RST                   (DISP_PIN_RESET)
#define ILI9341_INVERT_COLORS         (false)
// LCD Height and Width
#define ILI9341_LCD_WIDTH             (DISP_HOR_RES_MAX)
#define ILI9341_LCD_HEIGHT            (DISP_VER_RES_MAX)

// Colors in RGB565 Format
#define ILI9341_BLACK               0x0000      ///<   0,   0,   0
#define ILI9341_NAVY                0x000F      ///<   0,   0, 123
#define ILI9341_DARKGREEN           0x03E0      ///<   0, 125,   0
#define ILI9341_DARKCYAN            0x03EF      ///<   0, 125, 123
#define ILI9341_MAROON              0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE              0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE               0x7BE0      ///< 123, 125,   0
#define ILI9341_LIGHTGREY           0xC618      ///< 198, 195, 198
#define ILI9341_DARKGREY            0x7BEF      ///< 123, 125, 123
#define ILI9341_BLUE                0x001F      ///<   0,   0, 255
#define ILI9341_GREEN               0x07E0      ///<   0, 255,   0
#define ILI9341_CYAN                0x07FF      ///<   0, 255, 255
#define ILI9341_RED                 0xF800      ///< 255,   0,   0
#define ILI9341_MAGENTA             0xF81F      ///< 255,   0, 255
#define ILI9341_YELLOW              0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE               0xFFFF      ///< 255, 255, 255
#define ILI9341_ORANGE              0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW         0xAFE5      ///< 173, 255,  41
#define ILI9341_PINK                0xFC18      ///< 255, 130, 198

// ILI9341 Commands
/* Level 1 Commands */
#define ILI9341_SWRESET             0x01U   /* Software Reset */
#define ILI9341_READ_DISPLAY_ID     0x04U   /* Read display identification information */
#define ILI9341_RDDST               0x09U   /* Read Display Status */
#define ILI9341_RDDPM               0x0AU   /* Read Display Power Mode */
#define ILI9341_RDDMADCTL           0x0BU   /* Read Display MADCTL */
#define ILI9341_RDDCOLMOD           0x0CU   /* Read Display Pixel Format */
#define ILI9341_RDDIM               0x0DU   /* Read Display Image Format */
#define ILI9341_RDDSM               0x0EU   /* Read Display Signal Mode */
#define ILI9341_RDDSDR              0x0FU   /* Read Display Self-Diagnostic Result */
#define ILI9341_SPLIN               0x10U   /* Enter Sleep Mode */
#define ILI9341_SLEEP_OUT           0x11U   /* Sleep out register */
#define ILI9341_PTLON               0x12U   /* Partial Mode ON */
#define ILI9341_NORMAL_MODE_ON      0x13U   /* Normal Display Mode ON */
#define ILI9341_DINVOFF             0x20U   /* Display Inversion OFF */
#define ILI9341_DINVON              0x21U   /* Display Inversion ON */
#define ILI9341_GAMMA               0x26U   /* Gamma register */
#define ILI9341_DISPLAY_OFF         0x28U   /* Display off register */
#define ILI9341_DISPLAY_ON          0x29U   /* Display on register */
#define ILI9341_CASET               0x2AU   /* Colomn address register */
#define ILI9341_RASET               0x2BU   /* Page address register */
#define ILI9341_GRAM                0x2CU   /* GRAM register */
#define ILI9341_RGBSET              0x2DU   /* Color SET */
#define ILI9341_RAMRD               0x2EU   /* Memory Read */
#define ILI9341_PLTAR               0x30U   /* Partial Area */
#define ILI9341_VSCRDEF             0x33U   /* Vertical Scrolling Definition */
#define ILI9341_TEOFF               0x34U   /* Tearing Effect Line OFF */
#define ILI9341_TEON                0x35U   /* Tearing Effect Line ON */
#define ILI9341_MAC                 0x36U   /* Memory Access Control register*/
#define ILI9341_VSCRSADD            0x37U   /* Vertical Scrolling Start Address */
#define ILI9341_IDMOFF              0x38U   /* Idle Mode OFF */
#define ILI9341_IDMON               0x39U   /* Idle Mode ON */
#define ILI9341_PIXEL_FORMAT        0x3AU   /* Pixel Format register */
#define ILI9341_WRITE_MEM_CONTINUE  0x3CU   /* Write Memory Continue */
#define ILI9341_READ_MEM_CONTINUE   0x3EU   /* Read Memory Continue */
#define ILI9341_SET_TEAR_SCANLINE   0x44U   /* Set Tear Scanline */
#define ILI9341_GET_SCANLINE        0x45U   /* Get Scanline */
#define ILI9341_WDB                 0x51U   /* Write Brightness Display register */
#define ILI9341_RDDISBV             0x52U   /* Read Display Brightness */
#define ILI9341_WCD                 0x53U   /* Write Control Display register*/
#define ILI9341_RDCTRLD             0x54U   /* Read CTRL Display */
#define ILI9341_WRCABC              0x55U   /* Write Content Adaptive Brightness Control */
#define ILI9341_RDCABC              0x56U   /* Read Content Adaptive Brightness Control */
#define ILI9341_WRITE_CABC          0x5EU   /* Write CABC Minimum Brightness */
#define ILI9341_READ_CABC           0x5FU   /* Read CABC Minimum Brightness */
#define ILI9341_READ_ID1            0xDAU   /* Read ID1 */
#define ILI9341_READ_ID2            0xDBU   /* Read ID2 */
#define ILI9341_READ_ID3            0xDCU   /* Read ID3 */

/* Level 2 Commands */
#define ILI9341_RGB_INTERFACE       0xB0U   /* RGB Interface Signal Control */
#define ILI9341_FRMCTR1             0xB1U   /* Frame Rate Control (In Normal Mode) */
#define ILI9341_FRMCTR2             0xB2U   /* Frame Rate Control (In Idle Mode) */
#define ILI9341_FRMCTR3             0xB3U   /* Frame Rate Control (In Partial Mode) */
#define ILI9341_INVTR               0xB4U   /* Display Inversion Control */
#define ILI9341_BPC                 0xB5U   /* Blanking Porch Control register */
#define ILI9341_DFC                 0xB6U   /* Display Function Control register */
#define ILI9341_ETMOD               0xB7U   /* Entry Mode Set */
#define ILI9341_BACKLIGHT1          0xB8U   /* Backlight Control 1 */
#define ILI9341_BACKLIGHT2          0xB9U   /* Backlight Control 2 */
#define ILI9341_BACKLIGHT3          0xBAU   /* Backlight Control 3 */
#define ILI9341_BACKLIGHT4          0xBBU   /* Backlight Control 4 */
#define ILI9341_BACKLIGHT5          0xBCU   /* Backlight Control 5 */
#define ILI9341_BACKLIGHT7          0xBEU   /* Backlight Control 7 */
#define ILI9341_BACKLIGHT8          0xBFU   /* Backlight Control 8 */
#define ILI9341_POWER1              0xC0U   /* Power Control 1 register */
#define ILI9341_POWER2              0xC1U   /* Power Control 2 register */
#define ILI9341_VCOM1               0xC5U   /* VCOM Control 1 register */
#define ILI9341_VCOM2               0xC7U   /* VCOM Control 2 register */
#define ILI9341_NVMWR               0xD0U   /* NV Memory Write */
#define ILI9341_NVMPKEY             0xD1U   /* NV Memory Protection Key */
#define ILI9341_RDNVM               0xD2U   /* NV Memory Status Read */
#define ILI9341_READ_ID4            0xD3U   /* Read ID4 */
#define ILI9341_PGAMMA              0xE0U   /* Positive Gamma Correction register */
#define ILI9341_NGAMMA              0xE1U   /* Negative Gamma Correction register */
#define ILI9341_DGAMCTRL1           0xE2U   /* Digital Gamma Control 1 */
#define ILI9341_DGAMCTRL2           0xE3U   /* Digital Gamma Control 2 */
#define ILI9341_INTERFACE           0xF6U   /* Interface control register */

/* Extend register commands */
#define ILI9341_POWERA               0xCBU   /* Power control A register */
#define ILI9341_POWERB               0xCFU   /* Power control B register */
#define ILI9341_DTCA                 0xE8U   /* Driver timing control A */
#define ILI9341_DTCB                 0xEAU   /* Driver timing control B */
#define ILI9341_POWER_SEQ            0xEDU   /* Power on sequence register */
#define ILI9341_3GAMMA_EN            0xF2U   /* 3 Gamma enable register */
#define ILI9341_PRC                  0xF7U   /* Pump ratio control register */

typedef enum _ili9341_orientation_e
{
  LCD_ORIENTATION_0 = 0,
  LCD_PORTRAIT = LCD_ORIENTATION_0,
  LCD_ORIENTATION_90,
  LCD_LANDSCAPE = LCD_ORIENTATION_90,
  LCD_ORIENTATION_180,
  LCD_ORIENTATION_270,
} ili9341_orientation_e;

// Public Prototypes
void ili9341_init( void );
void ili9341_set_orientation( ili9341_orientation_e orientation );
ili9341_orientation_e ili9341_get_orientation( void );

void ili9341_set_window( uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end );
void ili9341_draw_pixel( uint16_t x, uint16_t y, uint16_t color );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ILI9341_H*/
