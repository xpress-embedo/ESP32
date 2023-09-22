/*
 * sntp_time_sync.c
 *
 *  Created on: 22-Sep-2023
 *      Author: xpress_embedo
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

#include "tasks_common.h"
#include "http_server.h"
#include "wifi_app.h"
#include "sntp_time_sync.h"

// Private Variables
static const char TAG[] = "SNTP Time Sync";
// SNTP Operating Mode Set Status
static bool sntp_op_mode_set = false;

// Private Function Prototypes
static void sntp_time_sync_task( void *pvParam );
static void sntp_time_sync_obtain_time( void );
static void sntp_time_synch_time_init_sntp( void );

// Public Function Definitions
/*
 * Starts the NTP Server Synchronization Task
 */
void sntp_time_sync_task_start( void )
{
  // Create the SNTP Sync Task
  xTaskCreate(&sntp_time_sync_task, "SNTP Time Sync", SNTP_TIME_SYNC_TASK_STACK_SIZE, NULL, SNTP_TIME_SYNC_TASK_PRIORIY, NULL);
}

/*
 * Returns local time if set.
 * @return local time buffer
 */
char *sntp_time_sync_get_time( void )
{
  static char time_buffer[100] = { 0 };
  time_t now = 0;
  struct tm time_info = { 0 };

  time(&now);
  localtime_r(&now, &time_info);
  // Check the time, in case we need to initialize/re-initialize
  if( time_info.tm_year < (2016-1900) )
  {
    ESP_LOGI(TAG, "Time is not set yet");
    // TODO: Problem for the time being putting some dummy data
    // strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
    sprintf(time_buffer, "22.09.2023 10:10:00");
  }
  else
  {
    strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &time_info);
    ESP_LOGI(TAG, "Current Time Info: %s", time_buffer);
  }
  return time_buffer;
}

// Private Function Definition
/*
 * The SNTP time synchronization task.
 * @param pvParam parameter which can be passed to the task
 */
static void sntp_time_sync_task( void *pvParam )
{
  for(;;)
  {
    sntp_time_sync_obtain_time();
    vTaskDelay(10000/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/*
 * Gets the current time and if the current time is not up to date, the
 * sntp_time_synch_time_init_sntp function is called.
 */
static void sntp_time_sync_obtain_time( void )
{
  time_t now = 0;
  struct tm time_info = { 0 };

  time(&now);

  localtime_r(&now, &time_info);
  // Check the time, in case we need to initialize/re-initialize
  if( (sntp_op_mode_set == false) || (time_info.tm_year < (2016-1900)) )
  {
    sntp_time_synch_time_init_sntp();
    // Set the local time zone
    setenv("TZ", "UTC-05:30", 1);
    // setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
  }
  else
  {
    ESP_LOGI(TAG, "sntp_time_sync_obtain_time:" );
  }
}

/*
 * Initialize SNTP Service using the SNTP_OPMODE_POLL_MODE
 */
static void sntp_time_synch_time_init_sntp( void )
{
  ESP_LOGI(TAG, "Initializing the SNTP Service");
  if( !sntp_op_mode_set )
  {
    // Set the Operating Mode
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_op_mode_set = true;
  }

  sntp_setservername(0, "pool.ntp.org");

  // initialize the servers
  sntp_init();

  // inform the http server that the service is initialized
  http_server_monitor_send_msg(HTTP_MSG_TIME_SERVICE_INITIALIZED);
}
