RGB LED
====================

## WiFi Application (High-Level Perspective)
* The ESP32 should start its Access Point so that other devices can connect to it.
  * This enables users to access information e.g. sensor data, device info, connection status/information, user options to connect and disconnect from an Access Point, Display Local Time etc.
* The WiFi application will start an HTTP server, which will support a web page.
* The application will contain a FreeRTOS task that accepts FreeRTOS Queue message `xQueueCreate`, `xQueueSend` and `xQueueReceive` for event coordination.
