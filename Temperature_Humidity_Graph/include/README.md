# Temperature and Humidity Graph Using ESP32 ILI9341 DHT11 and LVGL
[Blog Post Link](http://embeddedlaboratory.blogspot.com/2023/01/temperature-and-humidity-graph-using.html)

In this post, I will tell you, how to interface ILI9341 with ESP32, and then I will use the DHT11 sensor to get the temperature and humidity data and plot this data on the ILI9341 display, but for plotting I will be using "lv_chart" objects provided by LVGL, LVGL stands for Light and Versatile Embedded Graphics Library, this is the most popular free and open-source embedded graphics library to create beautiful UIs for any MCU, MPU and display type.
![Demo](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEhM1jTBvNdryHRmX2GI6TbTPFymskOdSkBke4zenP0kbeG2BvcPbxpl8Bo1Dyk6s2BtEsnHb4nZCqYwmIRrtOsPi0lp2j8YOG-v9txUj--GYtYNaus62q-4HrEB-6LqbK7kGqnudNw6s_TM-559TEYMS3OeBxi5Hd3TFDqCTcHG1-d4TzaH_OhNOviwLw/s2717/Graph.jpg)

In one of my previous posts, I showed you guys how to use LVGL with STM32F4 and STM32F7 Discovery Boards, so this post is basically an extension of that post.

We will do the same thing which we have done in the post on STM32 and LVGL integration, and if you compare both the code you can see that basically there is very less difference. So, in this project, we will do the following.
* Display VIBGYOR Colors for 4 seconds
* Display Red, Green, and Blue Sliders, and by changing the value of these sliders, the rectangle color is updated
* Once all slider value reaches 255 value the software switches to the next screen.
* In this screen, the temperature and humidity values taken from the DHT11 sensor are plotted on the graph using the "lv_chart" object.
* The difference between the previous post and this post is that here I will be plotting temperature and humidity data, while in the previous post, I was just plotting the temperature value.

## Project Setup
For this project, I will be using Arduino framework for ESP32, but instead of using Arduino IDE, I will be using PlatformIO with Visual Studio Code, as it has more advantages as compared to Arduino IDE, and for me, the following are some of the main advantages.
* PlatformIO with Visual Studio Code Extension is much faster in compiling the code as compared to the Arduino IDE (I don't the exact reason, but I read somewhere that PlatformIO utilizes all cores of our processor, I don't know how true this statement is, but for me, it is absolutely much faster)
* The auto-Completion feature is missing in Arduino IDE
* Libraries are added to individual projects rather than to all projects.
  * In PlatformIO whenever we add a library it is added to only that project, and all the changes done to our project are specific to our project, while for Arduino IDE, if we have to customize something we have to do that in the libraries folder and that usually creates a mess in most of the cases, this project is also an example of this, because here also we have to modify some of the library files.
* We can see the content of the Library just inside the Visual Studio Code, which is not available on Arduino IDE

The schematic diagram or connection diagram for this project is as below.

![Schematic](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEiKF_-fNiEruOkIqsi7RDKBetXslFmjaq-OsMxaN_phbTfW_1EEqQe4BI5YQ87LMC5AZqWLPamLo_u35q1TGmBnxClRn9TRP2v3T4AkA33hUChSke4wf0prJ6nQRzqmUEnqKh-AVWLeao4038bJKCOAy3D6VQ97J-XgawfCJvg-xbcB7ArAh8kIugfAqA/s1626/DHT11_ILI9341_SPI_ESP32_Schematic.png)

From the above picture, the pinout is not visible properly, so you use the below-mentioned table for the connections.

![Connection Details](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgn9db8dOBZDuaauIGv95_H3cDTtJK7tDucgiUPRWNfa6hokAVeY-o7UZZdG06jTStpT7mW0grwET1pu8f9T6bNiDywEwzoJUIppXdvGLQQ7BZdQTR5D-dsjvmS2nCG0bR-RxfZm480Xt-4GyRNWjcAbwzgdASHHowl7DONiV6HMAb44JuLqXUs7YcjwA/s594/ESP32_ILI9341_Touch_SPI_PinOut.png)

After doing this, we have to install three libraries, one is for the DHT11 sensor, the second is for SPI communication and the other one is the LVGL library, this can be done by going into the PlatformIO libraries section.

![Install DHT11 Sensor Library](https://blogger.googleusercontent.com/img/a/AVvXsEibCYOMw5QC6QPe0eKfyAEHyXAa3WOTgapkU6W4fXTJjl1-dzKPtJGbX4lgWveS9TTnFcjlTEww7EJckK_AmpCFcq3T40ZBua9ZUIJmx_80D-R0wUpdiSPWn0lKD26SCCTHgJvIO5PxINYx_VIKn7iKzyC02Kp7loXHCgTrOammdM76qoU1kmhYN-xacQ)

![Install TFT SPI Library](https://blogger.googleusercontent.com/img/a/AVvXsEje94ARq9s-IlXRXkFT4s3M2lBpauqtIUQYDe-aY0zkWuMclw39ePnlU9vii_ySZ8nOqQlrFV46ZA-DoCju7mX9-Tf71FFdJR3m5xVpOQArczNQ2Lbw-ZDXcCBneM4lOKDp8nprsPVtisEHJfo0x8_C9fC5qnqo7z2z-4GUAGqj41TxA7iwmLjQuN2HXg)

![Install LVGL Library](https://blogger.googleusercontent.com/img/a/AVvXsEhb_q4WnPFIKgSVBtJThcb2l-CX1TPcpiGMVAXmHxhvNbUxd4z6nB9D5povsmn089ZiGH4o0eQuD9uB50RuD7MSYYgkg1hUPrkfMOCjUyOPhlyhawsnGItWaxugQo1CAr2R7qCL2hQjNEbIlYu76lCSPPoNYqLImzMoVdL5uafQ60Dqg5XruRT_Fgn43g)

Once all three libraries are installed the "platformio.ini" file will look something like this.
```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
lib_deps = 
  lvgl/lvgl@^8.3.4
  bodmer/TFT_eSPI@^2.4.79
  adafruit/Adafruit Unified Sensor@^1.1.7
  adafruit/DHT sensor library@^1.4.4
monitor_speed = 115200
```

### Configuring SPI
The "TFT_eSPI" library is a generic library, and we have to configure this library for our project as per the connections.

If PlatformIO is used, then this library is present inside the "*.pio" folder as shown in the below image.

![Configuring SPI PlatformIO](https://blogger.googleusercontent.com/img/a/AVvXsEiCJMJv19IitGryjdzQpPywFSS42BeBjTtBigvCkquxc1YKmfsU7x2Eglr5Suusg7ztygHZgQFKFx-huJxLaCgvsEDx9ofAz39LFSaS4wepR_dL2gtLI3s4sJdxv6E2eylUtyg5mejP5CW2npDpunEd-j3XxR0kigrWvNHg-2o3AnpqTMCqWjACV6l62w)

And if Arduino IDE is used then this library is usually installed under the "Arduino\libraries\TFT_eSPI" path, as shown in the below image.

[Configuring SPI Arduino](https://blogger.googleusercontent.com/img/a/AVvXsEjPHhYVFyWjcxUMh-eB_l5GfJ3tI2ksfsVP_voS8gAENEMpsn3gnCTwJ4CZqn_c2Lqr4Rg7Y4W4a8a1o1O-SjLct-ixyBT0hPm38g_cK5Rrdf1B6xvm2hubksxWq1pfEOc5yjKColP-7r7R3RCEnAC3o58EbwtNsDDVELWE_9iyhebf2SoB91G2cXQpwA)

In both cases, we have to modify the `User_Setup.h` file, as shown below.
First select the correct driver, in my case it is `ILI9341_DRIVER`.
```C
// Only define one driver, the other ones must be commented out
#define ILI9341_DRIVER       // Generic driver for common displays
//#define ILI9341_2_DRIVER     // Alternative ILI9341 driver, see https://github.com/Bodmer/TFT_eSPI/issues/1172
//#define ST7735_DRIVER      // Define additional parameters below for this display
//#define ILI9163_DRIVER     // Define additional parameters below for this display
//#define S6D02A1_DRIVER
```

The second step is to configure the SPI pins, as shown below.

```C
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5  // Chip select control pin
#define TFT_DC   0  // Data Command control pin
//#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
```

If on the hardware, if you have connected the `TFT_RST` with some `ESP32` pin, then specify that pin here, else specify `-1` as I have done in my case.

The next step is to specify the Touch Chip Select pin, which in my case is `2` pin.
```C
#define TOUCH_CS 2     // Chip select pin (T_CS) of touch screen
```

Note: You can change `TFT_CS` which is the chip select pin, `TFT_DC` pin which is TFT Data or Command Mode control signal pin, `TFT_RST` pin which is the reset pin, and `TOUCH_CS` which is Touch Chip Select Pin, but for SPI pins always select the pins specified here.

Make sure this piece of code is also like this.
```C
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT
```

And then configure the SPI Frequency to be used, for TFT I am using 40MHz, for Reading 20MHz and for Touch 25MHz, as shown below.
```C
// Define the SPI clock frequency, this affects the graphics rendering speed. Too
// fast and the TFT driver will not keep up and display corruption appears.
// With an ILI9341 display 40MHz works OK, 80MHz sometimes fails
// With a ST7735 display more than 27MHz may not work (spurious pixels and lines)
// With an ILI9163 display 27 MHz works OK.

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  55000000 // STM32 SPI1 only (SPI2 maximum is 27MHz)
// #define SPI_FREQUENCY  80000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000
```

### Calibrating Touch
Once the above step is done, the next step is to get the calibration data for the Touch Screen, which can be get easily by using the Example Sketch. The path of this example sketch is as follows.
```
Examples\TFT_eSPI\Generic\Touch_calibrate
```
And the touch calibration project is as below.
```C
/*
  Sketch to generate the setup() calibration values, these are reported
  to the Serial Monitor.
  The sketch has been tested on the ESP8266 and screen with XPT2046 driver.
*/
#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

//------------------------------------------------------------------------------------------

void setup() {
  // Use serial port
  Serial.begin(115200);

  // Initialise the TFT screen
  tft.init();

  // Set the rotation to the orientation you wish to use in your project before calibration
  // (the touch coordinates returned then correspond to that rotation only)
  tft.setRotation(1);

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();

/*
  // Replace above line with the code sent to Serial Monitor
  // once calibration is complete, e.g.:
  uint16_t calData[5] = { 286, 3534, 283, 3600, 6 };
  tft.setTouch(calData);
*/

  // Clear the screen
  tft.fillScreen(TFT_BLACK);
  tft.drawCentreString("Touch screen to test!",tft.width()/2, tft.height()/2, 2);
}

//------------------------------------------------------------------------------------------

void loop(void) {
  uint16_t x = 0, y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) {
    tft.fillCircle(x, y, 2, TFT_WHITE);
    //Serial.print("x,y = ");
    //Serial.print(x);
    //Serial.print(",");
    //Serial.println(y);
  }
}

//------------------------------------------------------------------------------------------

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();

  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}
```

Once you run this project, after following all the instructions you will get something like this on Serial Terminal.
```C
  // Replace above line with the code sent to Serial Monitor
  // once calibration is complete, e.g.:
  uint16_t calData[5] = { 286, 3534, 283, 3600, 6 };
  tft.setTouch(calData);
```
These are the calibration values and we have to use these calibration values in our project, or else touch might not work properly.

### Configuring LVGL
The LVGL is a generic library that can be used with any display, with just some basic configuration.

If PlatformIO is used the library is present inside the `.pio` folder with the name `lvgl` as shown below.  
![Configuring LVGL](https://blogger.googleusercontent.com/img/a/AVvXsEirWIzxZowb3wptT1H5V-c0IQEJ1as0n-UP_0voqlBQYAQ6RhX4saftz5dRLR1Z2HBj4anm4PklZifYmEf7QuylJGNgb2IuSioJiZNC8M7JDc8FCjjCfMXnWng_6CNuPjG3R2EpgUx909dWJnPieTFkZRnYPazddyDoF8Q7mQ_fRCx49696pIe8Q4mFvg)

Here there is a file `lv_conf_template.h`, we have to copy this file outside the `lvgl` folder and rename this file to `lv_conf.h`.
And we have to enable the code/configuration present inside this file to 1, from 0, as shown below.
```C
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
```

The next, step is to make sure that the color dept is 16, for RGB565 color format, and SWAP is set "0", as shown below.

```C
/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
#define LV_COLOR_16_SWAP 0

/*Enable features to draw on transparent background.
 *It's required if opa, and transform_* style properties are used.
 *Can be also used if the UI is above another layer, e.g. an OSD menu or video player.*/
#define LV_COLOR_SCREEN_TRANSP 0

/* Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 * 0: round down, 64: round up from x.75, 128: round up from half, 192: round up from x.25, 254: round up */
#define LV_COLOR_MIX_ROUND_OFS 0

/*Images pixels with this color will not be drawn if they are chroma keyed)*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*pure green*/
```

Then we can configure the memory allocated to lvgl, which should be greater than 2 kilobytes as shown below it is set to 48 kilobytes.

```C
/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    /*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
    #define LV_MEM_SIZE (48U * 1024U)          /*[bytes]*/

    /*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
    #define LV_MEM_ADR 0     /*0: unused*/
    /*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc*/
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC
    #endif

#else       /*LV_MEM_CUSTOM*/
```

Then the next step is to configure the tick source, I will set it to 1 for Arduino-based platforms as shown below. This removes the need to manually update the tick with the function call `lv_tick_inc`.

```C
/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/
```

This step is optional, by setting `LV_USE_LOG`, we can log different types of the log messages, at the moment I have set it to `LV_LOG_LEVEL_WARN`.

```C
#define LV_USE_LOG 1
#if LV_USE_LOG

    /*How important log should be added:
    *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
    *LV_LOG_LEVEL_INFO        Log important events
    *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
    *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
    *LV_LOG_LEVEL_USER        Only logs added by the user
    *LV_LOG_LEVEL_NONE        Do not log anything*/
    #define LV_LOG_LEVEL      LV_LOG_LEVEL_WARN

    /*1: Print the log with 'printf';
    *0: User need to register a callback with `lv_log_register_print_cb()`*/
    #define LV_LOG_PRINTF 0

    /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1

#endif  /*LV_USE_LOG*/
```

And finally, we can enable and disable the fonts as shown below, I have enabled `MONTSERRAT_12, 14, and 16` fonts. Again this is an optional step.

```C
/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
#define LV_FONT_MONTSERRAT_8  1
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0
```

### Final Step
The source code is present in the `src` folder, and after running the code the following images can be seen on the `ILI9341` display.  
![VIBGYOR](https://blogger.googleusercontent.com/img/a/AVvXsEgWvyWm0uQdk-65p3bhioLzuzt5-l8Sd1AVwrvjzxXmw2wUocUustd6TZAd0JlYiYIJKIwGe5b8CiX_M8-brURwR5N1Zu5lPdOAorS4XPc2S5IteBN7BsVXdJC-7BhDrRJEYkKgy4MWxjtCuYVzd6W9NJKbtFb_DgdEKWGEDGaUhqzpH9q2kOMlg5e-TQ)

![RGB Mixer](https://blogger.googleusercontent.com/img/a/AVvXsEgQzLtfuov8ZzDGNhREsb36OxpDIHAvbl2xCDMlDulggywiwpHoST2NVNvoBIHcSxhpYJ6r-0y9tj3SntNhbVHq17Xk61JEb6O2BepMU7unpH3S5xNFDHrCsmwUyJgPpuRIfgAK9Or8HR4HwMMAzWhoFli_3ZCr2rFFFmODfTkxet8wYNkXvVlbqeKZkg)

![RGB Mixer](https://blogger.googleusercontent.com/img/a/AVvXsEjAlAeQHMwrL9Ld9lDAQ6uCO06ntbeID-9P8gkbpgX_70aKzKbmmmjskbS5XmNbdUiPbeZU23RvZgdqVNfeXxlEMLOcNno2xWJKkgGy3FjUZiYDzLQqnCRjyEcqt0C9QM7VcMDwY2R2lKg4sF4HC_5hhXMPZyeidNYGI4PWTDLT-Ow1bv3_EgXi3slhiQ)

![Temperature and Humidity Graph](https://blogger.googleusercontent.com/img/a/AVvXsEh-nbQX9w-faX1FPmnrZ7DUo-WJerDJQBYzKC5zDQWhk_Qx2YfQhPIT7x1luDkjxxM-pONLLqKYZqxmoMb_QgFYtxRnMDM-P3eHwr70c69ThBXYb7IVchcyW_5czEJLX6aVRxpZkkpeyL5T3A1WejZq_wUaf4ABqrKDKV9YVM7gVFKw2Cz18cvPBf_T1w)
