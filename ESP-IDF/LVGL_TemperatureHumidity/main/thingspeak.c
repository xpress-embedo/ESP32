/*
 * thingspeak.c
 *
 *  Created on: Feb 5, 2024
 *      Author: xpress_embedo
 */
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"

#include "main.h"
#include "thingspeak.h"

// Private Macros
#define THINGSPEAK_EVENT_QUEUE_LEN          (5)

// Private Variables
static const char *TAG = "ThingSpeak";
static const char *THINGSPEAK_KEY = "ThingSpeak Key";
static const char *CLIENT_KEY = "Content-Type";
static const char *CLIENT_VALUE = "application/x-www-form-urlencoded";
static QueueHandle_t thingspeak_event = NULL;

// Private Function Declaration
static void thingspeak_task( void *pvParameters );
static void thingspeak_send_temp_humidity(void);


// Public Function Prototypes

/**
 * @brief Start the connection with ThingSpeak
 * @param  none
 */
void thingspeak_start( void )
{
  // create message queue with the length THINGSPEAK_EVENT_QUEUE_LEN
  thingspeak_event = xQueueCreate( THINGSPEAK_EVENT_QUEUE_LEN, sizeof(thingspeak_q_msg_t) );
  if( thingspeak_event == NULL )
  {
    ESP_LOGE(TAG, "Unable to Create Queue");
  }
  xTaskCreate(&thingspeak_task, "ThingSpeak Task", 4096*2, NULL, 6, NULL);
}

/**
 * @brief Send an event to thingspeak queue
 * @param event event code
 * @param pData pointer to data if any (not used for future use)
 * @return pdTRUE if successful else pdFALSE
 */
BaseType_t thingspeak_send_event( thingspeak_event_t event, uint8_t *pData )
{
  BaseType_t status = pdFALSE;
  thingspeak_q_msg_t msg;

  if( event < THING_SPEAK_EV_MAX )
  {
    msg.event_id  = event;
    msg.data      = pData;
    status = xQueueSend( thingspeak_event, &msg, portMAX_DELAY );
  }
  return status;
}


/**
 * @brief ThingSpeak Task
 * @param pvParameters 
 */
static void thingspeak_task( void *pvParameters )
{
  thingspeak_q_msg_t msg;

  while( 1 )
  {
    // Wait for events posted in Queue
    if( xQueueReceive(thingspeak_event, &msg, portMAX_DELAY) )
    {
      // the below is the code to handle the state machine
      if( THING_SPEAK_EV_NONE != msg.event_id )
      {
        switch( msg.event_id )
        {
          case THING_SPEAK_EV_TEMP_HUMID:
            thingspeak_send_temp_humidity();
            break;
          default:
            break;
        } // switch case end
      }   // if event received in limit end
    }     // xQueueReceive end
  }
}

/**
 * @brief Send Temperature and Humidity data to ThingSpeak cloud
 * @param  none
 */
static void thingspeak_send_temp_humidity(void)
{
  esp_err_t err;
  uint8_t temperature = 0;
  uint8_t humidity = 0;

  sensor_data_t *sensor_data = get_temperature_humidity();
  size_t idx = sensor_data->sensor_idx;
  if( (idx > 0) && (idx < SENSOR_BUFF_SIZE) )
  {
    // before posting the event we have incremented the index and hence to get
    // the last sensor data we have to use - 1
    idx = (idx - 1);
    temperature = sensor_data->temperature[idx];
    humidity = sensor_data->humidity[idx];
  }
  else
  {
    temperature = 0;
    humidity = 0;
  }

  char thingspeak_url[200];
  snprintf( thingspeak_url, sizeof(thingspeak_url), "https://api.thingspeak.com/update?api_key=%s&field1=%u&field2=%u", THINGSPEAK_KEY, temperature, humidity);
  esp_http_client_config_t config =
  {
    .url = thingspeak_url,
    .method = HTTP_METHOD_GET,
    // todo: maybe for future
    // .transport_type = HTTP_TRANSPORT_OVER_SSL, // Specify transport type
    // .crt_bundle_attach = esp_crt_bundle_attach, // Attach the certificate bundle
  };

  esp_http_client_handle_t client = esp_http_client_init( &config );
  // set header
  esp_http_client_set_header( client, CLIENT_KEY, CLIENT_VALUE );
  ESP_LOGI(TAG, "ThingSpeak URL = %s", thingspeak_url);
  // note: we can also use these two functions, but I never tried
  // esp_http_client_set_post_field(client, post_data, strlen(post_data));
  // esp_http_client_set_url(client, post_data);
  
  // perform the request
  err = esp_http_client_perform(client);
  if (err == ESP_OK)
  {
    int status_code = esp_http_client_get_status_code(client);
    if( status_code == 200 )
    {
      ESP_LOGI(TAG, "Message Sent Successfully.");
    }
    else
    {
      ESP_LOGE(TAG, "Message Sending Failed.");
    }
  }
  esp_http_client_cleanup(client);
}
