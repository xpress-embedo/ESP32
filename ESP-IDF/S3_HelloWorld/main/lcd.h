/*
 * lcd.h
 *
 *  Created on: Mar 2, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_LCD_H_
#define MAIN_LCD_H_

// LCD Screen Resolution
#define LCD_H_RES                     (800)
#define LCD_V_RES                     (480)

// Touch Minimum Maximum Range
// explanation todo
#define TOUCH_H_RES_MIN               (0)
#define TOUCH_H_RES_MAX               (477)
#define TOUCH_V_RES_MIN               (0)
#define TOUCH_V_RES_MAX               (269)

// 18MHz, it is been reported by several users that above 18MHz distortion is observed
#define LCD_PIXEL_CLOCK_HZ            (18*1000*1000)

// GPIO Pin Assignment based on schematic check docs/ESP32-8048S043-1.png in root readme file
#define LCD_BK_LIGHT_ON_LEVEL         (1)
#define LCD_BK_LIGHT_OFF_LEVEL        (!LCD_BK_LIGHT_ON_LEVEL)
#define LCD_PIN_BK_LIGHT              (GPIO_NUM_2)

#define LCD_PIN_HSYNC                 (GPIO_NUM_39)     // HSYNC
#define LCD_PIN_VSYNC                 (GPIO_NUM_41)     // VSYNC
#define LCD_PIN_DE                    (GPIO_NUM_40)     // DE
#define LCD_PIN_PCLK                  (GPIO_NUM_42)     // DCLK
#define LCD_PIN_DATA0                 (GPIO_NUM_8)      // B3
#define LCD_PIN_DATA1                 (GPIO_NUM_3)      // B4
#define LCD_PIN_DATA2                 (GPIO_NUM_46)     // B5
#define LCD_PIN_DATA3                 (GPIO_NUM_9)      // B6
#define LCD_PIN_DATA4                 (GPIO_NUM_1)      // B7
#define LCD_PIN_DATA5                 (GPIO_NUM_5)      // G2
#define LCD_PIN_DATA6                 (GPIO_NUM_6)      // G3
#define LCD_PIN_DATA7                 (GPIO_NUM_7)      // G4
#define LCD_PIN_DATA8                 (GPIO_NUM_15)     // G5
#define LCD_PIN_DATA9                 (GPIO_NUM_16)     // G6
#define LCD_PIN_DATA10                (GPIO_NUM_4)      // G7
#define LCD_PIN_DATA11                (GPIO_NUM_45)     // R3
#define LCD_PIN_DATA12                (GPIO_NUM_48)     // R4
#define LCD_PIN_DATA13                (GPIO_NUM_47)     // R5
#define LCD_PIN_DATA14                (GPIO_NUM_21)     // R6
#define LCD_PIN_DATA15                (GPIO_NUM_14)     // R7
#define LCD_PIN_DISP_EN               (GPIO_NUM_NC)     // not connected

#define TOUCH_PIN_RESET               (GPIO_NUM_38)     // REST
#define TOUCH_PIN_SCL                 (GPIO_NUM_20)
#define TOUCH_PIN_SDA                 (GPIO_NUM_19)
#define TOUCH_PIN_INT                 (GPIO_NUM_18)
#define TOUCH_FREQ_HZ                 (400000)

// Public Function Declaration
void lcd_init( void );
void lcd_set_backlight( bool state );
uint8_t gui_update_lock( void );
void gui_update_unlock( void );

#endif /* MAIN_LCD_H_ */
