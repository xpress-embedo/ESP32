# Some Important Commands
On Windows I am using Eclipse based IDE, so it's not a problem, but on Linux environment I didn't installed this IDE, and using the command-line version, maybe I need to install IDE when I wanted to develop and debug my ESP32 Kaluga Development Kit, as this is the only board which supports the debugging.

Use the following command to get the ESP-IDF framework.
```bash
mkdir -p ~/esp
cd ~/esp
git clone -b v5.5 --recursive https://github.com/espressif/esp-idf.git
```
Once the above command clones the repository from internet, we need to install the tools for the devices we want to use, with the following commands.
```bash
# to install tools for esp32 device
./install.sh esp32
# to install tools for multiple devices
./install.sh esp32 esp32s2 esp32s3
# to install tools for all available espressif devices
./install.sh all
```

Then we have to use the following command.
```bash
source ./export.sh
```

The following are some of the important ESP-IDF commands.

```bash
# create project
idf.py create-project <project name>

# set target chip
idf.py set-target <target>
# examples
idf.py set-target esp32
idf.py set-target esp32s3
idf.py set-target esp32s2

# start the graphical configuration tool
idf.py menuconfig

# build the project
idf.py build

# remove build output
idf.py clean

# full clean the project (deletes entire build content)
idf.py fullclean

# Flash the project
idf.py flash
```

# Some Important Links
=======================

[Development Board Overview](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#development-board-overviews)

[ESP-WROVER-KIT JTAG Interface](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/configure-ft2232h-jtag.html)

[ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)

[Eclipse IDE Plugin](https://github.com/espressif/idf-eclipse-plugin)

[ESP-IDF Tools Installer for Windows](https://github.com/espressif/idf-installer#esp-idf-tools-installer-for-windows)

[Get Starter Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

[ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

[API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)

[Examples on GitHub](https://github.com/espressif/esp-idf/tree/master/examples)

## WiFi Related
[List of WLAN Channels](https://en.wikipedia.org/wiki/List_of_WLAN_channels)

[WiFi Channel Selection Guidelines](https://www.espressif.com/sites/default/files/documentation/esp8266_wi-fi_channel_selection_guidelines_en.pdf)

[AP Basic Configuration ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/release-v4.2/esp32/api-guides/wifi.html#ap-basic-configuration)

[WiFi Bandwidth Option](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#wi-fi-ht20-40)

[WiFi Power Option](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-power-saving-mode)