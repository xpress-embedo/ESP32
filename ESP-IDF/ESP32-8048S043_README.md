# ESP32-8048S043
The full board name is `ESP32-8048S043C` this board is from Sunton company, and has the following important points.  
* ESP32-S3 is the on-board MCU
* The Flash size connected is 16MB, this is needed while updating the SDKConfig.
* The PSRAM size is 8MB, this shall be updated in the SDKConfig, to use external SPI PSRAM, and mode should be octal. This is more suitable for loading loading fonts and graphics.
  ```
  CONFIG_SPIRAM=y
  CONFIG_SPIRAM_MODE_OCT=y
  CONFIG_SPIRAM_FETCH_INSTRUCTIONS=y
  CONFIG_SPIRAM_RODATA=y
  CONFIG_SPIRAM_SPEED_80M=y
  ```
* Display Panel is 4.3 inch IPS
* Display Resolution is 800x480
* Touch Screen is Capacitive (GTA911)
* SD Card supported
* Dimming is supported

This is how the board looks.  
<img src="esp32-8048s043-docs/board information/board_frontside.png" width = 400>
<img src="esp32-8048s043-docs/board information/board_backside.png" width = 400>
<!-- ![Front Size](<esp32-8048s043-docs/board information/board_frontside.png>)   -->
<!-- ![Back Side](<esp32-8048s043-docs/board information/board_backside.png>)   -->

### Notes
The display driver chip is ILI9485 or ST7262.
