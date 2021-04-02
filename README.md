[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/embeddedlab)

# ESP32
The ESP32 module I am using is of 38 pin, but it doesn't matter, I purchased it from [this link](https://amzn.to/2PSL5eN).  

### Pin Out
The ESP32 chip comes with 48 pins with multiple functions. Not all pins are exposed in all ESP32 development boards, and there are some pins that cannot be used.  
If you're using an ESP32 38 pin Development board, you can use the following GPIO diagram as a reference. ESP32 38 pin Development board pinout diagram GPIO's pins. _This will be used in all the examples_.  
<img src="Support/ESP32-38 PIN-DEVBOARD.png" width=80% height=80% />

If you're using an ESP32 30 pin Development board, you can use the following GPIO diagram as a reference. ESP32 30 pin Development board pinout diagram GPIO's pins.  
<img src="Support/ESP32-30PIN-DEVBOARD.png" width=80% height=80% />

The following figure illustrates the ESP-WROOM-32 chip pinout. Use this diagram if you're using an ESP32 bare chip in your projects.  
<img src="Support/ESP32-VROOM-32D-PINOUT.png" width=80% height=80% />

### ESP32 Peripherals
The ESP32 Peripherals include:
* 18 Analog to Dogital Converter Channels
* 3 SPI Interfaces
* 3 UART Interfaces
* 2 I2C Interfaces
* 16 PWM Output Channels
* 2 Digital to Analog Converters
* 2 I2S Interfaces
* 10 Capacitive Sensing GPIO's
The Analog to Digital Converter pins and Digital to Analog Converter pins are fixed, but other interfaces like UART, I2C, SPI, PWM etc pins, can be configured using internal multiplexer.  
The absolute maximum current drawn per GPIO is 40mA according to the “Recommended Operating Conditions” section in the ESP32 datasheet.  
The ESP32 also features a built-in hall effect sensor that detects changes in the magnetic field in its surroundings.  

#### Input Only Pins
The GPIO's 34 to 39 are GPI's i.e. these pins are input only pins, and these pins don't have internal pull-up or pull-down resistors. These pins can't be used for outputs.  

#### Capacitive Touch GPIOs
The ESP32 has 10 internal capacitive touch sensors. These can sense variations in anything that holds an electrical charge, like the human skin. So they can detect variations induced when touching the GPIOs with a finger. These pins can be easily integrated into capacitive pads, and replace mechanical buttons. The capacitive touch pins can also be used to wake up the ESP32 from sleep/deep sleep.  
Those internal touch sensors are connected to these GPIO’s.  
* T0 (GPIO 4)
* T1 (GPIO 0)
* T2 (GPIO 2)
* T3 (GPIO 15)
* T4 (GPIO 13)
* T5 (GPIO 12)
* T6 (GPIO 14)
* T7 (GPIO 27)
* T8 (GPIO 33)
* T9 (GPIO 32)

#### Analog to Digital Converter (ADC)
The ESP32 has 18 x 12 bits ADC input channels (while the ESP8266 only has 1x 10 bits ADC). These are the GPIOs that can be used as ADC and respective channels.  

* ADC1_CH0 (GPIO 36)
* ADC1_CH1 (GPIO 37)
* ADC1_CH2 (GPIO 38)
* ADC1_CH3 (GPIO 39)
* ADC1_CH4 (GPIO 32)
* ADC1_CH5 (GPIO 33)
* ADC1_CH6 (GPIO 34)
* ADC1_CH7 (GPIO 35)
* ADC2_CH0 (GPIO 4)
* ADC2_CH1 (GPIO 0)
* ADC2_CH2 (GPIO 2)
* ADC2_CH3 (GPIO 15)
* ADC2_CH4 (GPIO 13)
* ADC2_CH5 (GPIO 12)
* ADC2_CH6 (GPIO 14)
* ADC2_CH7 (GPIO 27)
* ADC2_CH8 (GPIO 25)
* ADC2_CH9 (GPIO 26)

**_how to use the ESP32 ADC pins_**
Note: ADC2 pins cannot be used when Wi-Fi is used. So, if you're using Wi-Fi and you're having trouble getting the value from an ADC2 GPIO, you may consider using an ADC1 GPIO instead, that should solve your problem.  
The ADC input channels have a 12 bit resolution. This means that you can get analog readings ranging from 0 to 4095, in which 0 corresponds to 0V and 4095 to 3.3V. You also have the ability to set the resolution of your channels on the code, as well as the ADC range.  

#### Digital to Analog Converter (DAC)
There are 2 x 8 bits DAC channels on the ESP32 to convert digital signals into analog voltage signal outputs. These are the DAC channels.

* DAC1 (GPIO25)
* DAC2 (GPIO26)

#### PWM
The ESP32 LED PWM controller has 16 independent channels that can be configured to generate PWM signals with different properties. All pins that can act as outputs can be used as PWM pins (GPIOs 34 to 39 can’t generate PWM).  
To set a PWM signal, you need to define these parameters in the code:  
* Signal’s frequency;
* Duty cycle;
* PWM channel;
* GPIO where you want to output the signal.

#### I2C
The ESP32 has two I2C channels and any pin can be set as SDA or SCL. The default I2C pins are.  
* GPIO 21 (SDA)
* GPIO 22 (SCL)

#### SPI
By default, the pin mapping for SPI is:  
**VSPI**
* MOSI - GPIO 23
* MISO - GPIO 19
* CLK  - GPIO 18
* CS   - GPIO 5

**HSPI**
* MOSI - GPIO 13
* MISO - GPIO 12
* CLK  - GPIO 14
* CS   - GPIO 15

#### Interrupts
All GPIOs can be configured as interrupts.  

#### Strapping Pins
The ESP32 chip has the following strapping pins:  
* GPIO 0
* GPIO 2
* GPIO 4
* GPIO 5 (must be HIGH during boot)
* GPIO 12 (must be LOW during boot)
* GPIO 15 (must be HIGH during boot)  
These are used to put the ESP32 into bootloader or flashing mode. On most development boards with built-in USB/Serial, you don’t need to worry about the state of these pins. The board puts the pins in the right state for flashing or boot mode.  
However, if you have peripherals connected to those pins, you may have trouble trying to upload new code, flashing the ESP32 with new firmware or resetting the board. If you have some peripherals connected to the strapping pins and you are getting trouble uploading code or flashing the ESP32, it may be because those peripherals are preventing the ESP32 to enter the right mode. After resetting, flashing, or booting, those pins work as expected.  

#### Pins HIGH at Boot
Some GPIO's change its state to HIGH or output PWM signals at boot or reset. This means that if you have outputs connected to these GPIOs you may get unexpected results when the ESP32 resets or boots.  
* GPIO 1
* GPIO 3
* GPIO 5
* GPIO 6 to GPIO 11 **(connected to the ESP32 integrated SPI flash memory – not recommended to use)**
* GPIO 14
* GPIO 15

#### Enable (EN)
Enable (EN) is the 3.3V regulator's enable pin. It is pulled up, so connect to ground to disable the 3.3V regulator. This means that you can use this pin connected to a pushbutton to restart your ESP32.