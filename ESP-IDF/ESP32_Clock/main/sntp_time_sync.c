/*
 * sntp_time_sync.c
 *
 *  Created on: Feb 11, 2024
 *      Author: xpress_embedo
 */
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/apps/sntp.h"

#include "sntp_time_sync.h"

// Private Variables
static const char * TAG = "SNTP";
static bool sntp_op_mode_set = false;
static char time_buffer[100] = { 0 };

// Private Function Declaration
static void sntp_time_sync_task( void *pvParameter );
static void sntp_time_sync_obtain_time( void );
static void sntp_time_sync_init( void );

// Public Function Definition
/**
 * @brief Starts the NTP Server Synchronization task
 * @param  None
 */
void sntp_time_sync_start( void )
{
  xTaskCreate(&sntp_time_sync_task, "SNTP Task", 4096, NULL, 6, NULL );
}

/**
 * @brief Returns the Local Time if Set
 * @param  None
 * @return pointer to local time buffer
 */
char * sntp_time_sync_get_time( void )
{
  time_t now = 0;
  struct tm time_info = { 0 };
  // get the current time
  time(&now);
  localtime_r(&now, &time_info);
  // check if the fetched time is valid or not
  if( time_info.tm_year < (2016-1900) )
  {
    ESP_LOGI(TAG, "Time is not set yet.");
  }
  else
  {
    // time is valid, we can reset the buffer
    memset(time_buffer, 0x00, sizeof(time_buffer) );
    // the function is similar to snprintf, but is used to format the time
    strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
    ESP_LOGI(TAG, "Current Time: %s", time_buffer);
  }
  return (time_buffer);
}

// Private Function Definition

/**
 * @brief SNTP Time Sync Task
 * @param pvParameter 
 */
static void sntp_time_sync_task( void *pvParameter )
{
  while(1)
  {
    sntp_time_sync_obtain_time();
    vTaskDelay(10000/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/**
 * @brief Get the Current Time and if the current time is not upto date, the
 *        sntp_time_sync_init_sntp function is called
 * @param  None
 */
static void sntp_time_sync_obtain_time( void )
{
  time_t now = 0;
  struct tm time_info = { 0 };

  // get the current time
  time(&now);
  // The localtime_r() function converts the calendar time pointed to by clock
  // into a broken-down time stored in the structure to which result points.
  // The localtime_r() function also returns a pointer to that same structure.
  localtime_r(&now, &time_info);

  // check if the fetched time is valid or not
  if( time_info.tm_year < (2016-1900) )
  {
    // this means time is not valid
    sntp_time_sync_init();
    // set the local time zone (this is Indian Time Zone)
    setenv("TZ", "IST-5:30", 1);
    tzset();
  }
}

/**
 * @brief Initialize the SNTP Service
 * @param  None
 */
static void sntp_time_sync_init( void )
{
  ESP_LOGI(TAG, "Initializing SNTP Service");

  if( sntp_op_mode_set == false )
  {
    // operating mode not set, set the operating mode
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_op_mode_set = true;
  }

  sntp_setservername(0, "pool.ntp.org");

  // initialize the servers
  sntp_init();
}
