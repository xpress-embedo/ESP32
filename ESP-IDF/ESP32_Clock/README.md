ESP32 Clock Using SNTP Time Synchronization
====================

### Configuration Steps
* After the FreeRTOS task kicks off and obtain time function is called, the initialize SNTP funcion is called, where the first SNTP function used, configures the client in poll mode to query the server every `n` seconds.
  * `sntp_setoperatingmode(SNTP_OPMODE_POLL)`
* Also, within the SNTP initialize function, we will tell the client which server to use. A common choice is a cluster of servers from `pool.ntp.org`, `sntp_setservername(0, "pool.ntp.org")`.
* Then we will initialize the service, i.e. by using `sntp_init()`
* Set the Time Zone variable and initialize the time zone conversion.
  * Set the Time Zone, for example `setenv("TZ,"CET-1",1")` and `tzset()` to initialize the timezone conversion routine.
* After SNTP service is initialized, we need to check if the system clock has the updated time.
  * To get the actual time from the system clock, we use the `time()` function, which updates the `time_t` variable.
  * To split the variable into different time values (year, month, day...), the `localtime_r` function is used which updates the `tm` structure.
    ```C
    time_t now = 0;
    struct tm time_info = { 0 };
    time(&now);
    localtime_r(&now, &time_info);
    ```
  * We will then check the information from the `tm` structure to see if the time was set yet.
    ```C
    if( time_info.tm_year < (2016 - 1990) )
    {
      sntp_time_sync_init_sntp();
      // set the local time zone
      setenv("TZ,"CET-1",1");
      tzset();
    }
    ```
