Weather Station Server Application
====================

### Non Volatile Storage
* After the ESP32 is connected successfully to an Access Point via the web page, the SSID and the Password used to connect will be save in the flash, that's why in `wifi_app_task` function when we received the message `WIFI_APP_MSG_STA_CONNECTED_GOT_IP` we saved the credentials. Here important point is we shouldn't save credentials again and again, it should be save only when the connection is triggered from the HTTP Server, which can be determined using the Event Bit `WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT`.
* Upon startup, the ESP32 will check the flash for any saved credentials, if there are any, they will be used to attempt a connection immediately. This is also done in the function `wifi_app_task` here at start-up the message `WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS` is posted into the Queue.
* Upon startup, if after the `WIFI_MAX_CONN_RETRIES` is reached and a connection cannot be established, we will clear the flash.
* If the "Disconnect Button" on the web page is pressed, the credentials will be cleared.

### Button with Interrupt and Semaphore
* Will use the BOOT button to disconnect the ESP32 and clear the credentials.
* The BOOT button will be configured to generate an interrupt on IO0.
* When the interrupt occurs, a message will be sent to the WiFi Application about the user request to disconnect/clear credentials.
* Upon receiving the message, the WiFi Application will check if there is really and active connection, prior to disconnecting and clearing the credentials.

### Displaying Local Time on the Web Page (SNTP Time Synchronization)
* SNTP Stands for Simple Network Time Protocol is a protocol designed to synchronize the clock of devices connected to the internet. The basic operation is as follows:
  * The client device connects to the server using the UDP protocol on port 123.
  * The client transmits a request packet to the server.
  * The server responds with a time stamp packet.
  * The client can then parse out the current date and time values.
  * If the ESP32 is connected to the internet, it can get the date and time using the SNTP.
* Once the ESP32 has an internet connection (connected to an AP/Router), the SNTP task start function will be called.
* The task start function will set off the FreeRTOS time synchronization task, which will call a function to obtain the updated time. In this implementation, the task will keep synchronizing/checking the time is up-to date.
* The obtain time function will initialize the SNTP service to query an SNTP server for the universal time, the obtain time function will reinitialize in the case the time is not up-to date.
* The local time zone will be set after the SNTP service is initialized.
* The web server will respond with the updated time once time service is initialized.


