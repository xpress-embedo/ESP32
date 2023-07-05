Open Weather Map Example
====================

Took Some Help from the following Links
https://github.com/ESP32Tutorials/ESP32-ESP-IDF-OpenWeatherMap-API/tree/main/main
https://www.youtube.com/watch?v=_dRrarmQiAM
https://esp32tutorials.com/esp32-esp-idf-openweathermap-api-sensorless-weather-station/#more-3008

Faced an issue "esp-tls-mbedtls: No server verification option set in esp_tls_cfg_t structure"
Which is fixed using this link
https://github.com/zonmen/IndoorSolution-esp32/issues/1
CONFIG_ESP_TLS_INSECURE=y
CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY=y

This is a template application to be used with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*
