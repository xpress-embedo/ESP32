/*
 * http_server.c
 *
 *  Created on: 17-Jul-2023
 *      Author: xpress_embedo
 */
#include "esp_http_server.h"
#include "esp_log.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

// Macros
#define HTTP_SERVER_MAX_URI_HANDLERS                    (20u)
#define HTTP_SERVER_RECEIVE_WAIT_TIMEOUT                (10u)   // in seconds
#define HTTP_SERVER_SEND_WAIT_TIMEOUT                   (10u)   // in seconds
#define HTTP_SERVER_MONITOR_QUEUE_LEN                   (3u)

// Private Variables
static const char TAG[] = "http_server";
// HTTP Server Task Handle
static httpd_handle_t http_server_handle = NULL;
// HTTP Server Monitor Task Handler
static TaskHandle_t task_http_server_monitor = NULL;
// Queue Handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_q_handle;

// Embedded Files: JQuery, index.html, app.css, app.js, and favicon.ico files
extern const uint8_t jquery_3_3_1_min_js_start[]      asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[]        asm("_binary_jquery_3_3_1_min_js_end");
extern const uint8_t index_html_start[]               asm("_binary_index_html_start");
extern const uint8_t index_html_end[]                 asm("_binary_index_html_end");
extern const uint8_t app_css_start[]                  asm("_binary_app_css_start");
extern const uint8_t app_css_end[]                    asm("_binary_app_css_end");
extern const uint8_t app_js_start[]                   asm("_binary_app_js_start");
extern const uint8_t app_js_end[]                     asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[]              asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[]                asm("_binary_favicon_ico_end");

// Private Function Prototypes
static void http_server_monitor(void *pvParameter);
static httpd_handle_t http_server_configure( void );
static esp_err_t http_server_j_query_handler(httpd_req_t *req);
static esp_err_t http_server_index_html_handler(httpd_req_t *req);
static esp_err_t http_server_app_css_handler(httpd_req_t *req);
static esp_err_t http_server_app_js_handler(httpd_req_t *req);
static esp_err_t http_server_favicon_handler(httpd_req_t *req);
//static esp_err_t http_server_ota_update_handler(httpd_req_t *req);
//static esp_err_t http_server_ota_status_handler(httpd_req_t *req);

// Public Function Definition
/*
 * Starts the HTTP Server
 */
void http_server_start(void)
{
  if( http_server_handle == NULL )
  {
    http_server_handle = http_server_configure();
  }
}

/*
 * Stops the HTTP Server
 */
void http_server_stop(void)
{
  if( http_server_handle )
  {
    httpd_stop(http_server_handle);
    ESP_LOGI(TAG, "http_server_stop: stopping HTTP Server");
    http_server_handle = NULL;
  }

  if( task_http_server_monitor )
  {
    vTaskDelete(task_http_server_monitor);
    ESP_LOGI(TAG,"http_server_stop: stopping HTTP server monitor");
    task_http_server_monitor = NULL;
  }
}

/*
 * Sends a message to the Queue
 * @param msg_id Message ID from the http_server_msg_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t http_server_monitor_send_msg(http_server_msg_e msg_id)
{
  http_server_q_msg_t msg;
  msg.msg_id = msg_id;
  return xQueueSend(http_server_monitor_q_handle, &msg, portMAX_DELAY );
}


// Private Function Definitions
/*
 * HTTP Server Monitor Task used to track events of the HTTP Server.
 * @param pvParameter parameters which can be passed to the task
 * @return http server instance handle if successful, NULL otherwise
 */
static void http_server_monitor( void *pvParameter )
{
  http_server_q_msg_t msg;
  for( ;; )
  {
    if( xQueueReceive(http_server_monitor_q_handle, &msg, portMAX_DELAY) )
    {
      switch (msg.msg_id)
      {
      case HTTP_MSG_WIFI_CONNECT_INIT:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_CONNECT_INIT");
        break;
      case HTTP_MSG_WIFI_CONNECT_SUCCESS:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");
        break;
      case HTTP_MSG_WIFI_CONNECT_FAIL:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");
        break;
      case HTTP_MSG_WIFI_OTA_UPDATE_SUCCESSFUL:
        ESP_LOGI( TAG, "HTTP_MSG_OTA_UPDATE_SUCCESSFUL");
//        g_fw_update_status = OTA_UPDATE_SUCCESSFUL;
        break;
      case HTTP_MSG_WIFI_OTA_UPDATE_FAILED:
        ESP_LOGI( TAG, "HTTP_MSG_OTA_UPDATE_FAILED");
//        g_fw_update_status = OTA_UPDATE_FAILED;
        break;
      case HTTP_MSG_WIFI_OTA_UPDATE_INITIALIZED:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_OTA_UPDATE_INITIALIZED");
        break;
      default:
        break;
      }
    }
  }
}

/*
 * Sets up the default httpd server configuration
 * @return http server instance handle if successful, NULL otherwise
 */
static httpd_handle_t http_server_configure(void)
{
  // Generate the default configuration
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // create HTTP Server Monitor Task
  xTaskCreate(&http_server_monitor, "http_server_monitor", \
              HTTP_SERVER_MONITOR_STACK_SIZE, NULL, \
              HTTP_SERVER_MONITOR_PRIORITY, &task_http_server_monitor);

  // create a message queue
  http_server_monitor_q_handle = xQueueCreate(HTTP_SERVER_MONITOR_QUEUE_LEN,\
                                              sizeof(http_server_q_msg_t));

  // No need to specify the core id as this is esp32s2 with single core

  // Adjust the default priority to 1 less than the wifi application task
  config.task_priority = HTTP_SERVER_TASK_PRIORITY;

  // Specify the Stack Size (default is 4096)
  config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

  // Increase our URI Handlers
  config.max_uri_handlers = HTTP_SERVER_MAX_URI_HANDLERS;

  // Increase the timeout limits
  config.recv_wait_timeout = HTTP_SERVER_RECEIVE_WAIT_TIMEOUT;
  config.send_wait_timeout = HTTP_SERVER_SEND_WAIT_TIMEOUT;

  ESP_LOGI(TAG,
           "http_server_configure: Starting Server on port: '%d' with task priority: '%d'",
           config.server_port, config.task_priority);

  // Start the httpd server port
  if( httpd_start(&http_server_handle, &config) == ESP_OK )
  {
    ESP_LOGI(TAG, "http_server_configure: Registering URI Handlers");
    // Register jQuery handler
    httpd_uri_t jquery_js =
    {
      .uri       = "/jquery-3.3.1.min.js",
      .method    = HTTP_GET,
      .handler   = http_server_j_query_handler,
      .user_ctx  = NULL
    };
    // Register index.html handler
    httpd_uri_t index_html =
    {
      .uri       = "/",
      .method    = HTTP_GET,
      .handler   = http_server_index_html_handler,
      .user_ctx  = NULL
    };
    // Register app.css handler
    httpd_uri_t app_css =
    {
      .uri       = "/app.css",
      .method    = HTTP_GET,
      .handler   = http_server_app_css_handler,
      .user_ctx  = NULL
    };
    // Register app.js handler
    httpd_uri_t app_js =
    {
      .uri       = "/app.js",
      .method    = HTTP_GET,
      .handler   = http_server_app_js_handler,
      .user_ctx  = NULL
    };
    // Register favicon.ico handler
    httpd_uri_t favicon_ico =
    {
      .uri       = "/favicon.ico",
      .method    = HTTP_GET,
      .handler   = http_server_favicon_handler,
      .user_ctx  = NULL
    };
    //// Register OTA Update Handler
    //static const httpd_uri_t ota_update =
    //{
    //  .uri       = "/OTAupdate",
    //  .method    = HTTP_POST,
    //  .handler   = http_server_ota_update_handler,
    //  .user_ctx  = NULL
    //};
    //
    //// Register OTA Status Handler
    //static const httpd_uri_t ota_status =
    //{
    //  .uri       = "/OTAstatus",
    //  .method    = HTTP_POST,
    //  .handler   = http_server_ota_status_handler,
    //  .user_ctx  = NULL
    //};
    // Register Query Handler
    httpd_register_uri_handler(http_server_handle, &jquery_js);
    httpd_register_uri_handler(http_server_handle, &index_html);
    httpd_register_uri_handler(http_server_handle, &app_css);
    httpd_register_uri_handler(http_server_handle, &app_js);
    httpd_register_uri_handler(http_server_handle, &favicon_ico);
//    httpd_register_uri_handler(http_server_handle, &ota_update);
//    httpd_register_uri_handler(http_server_handle, &ota_status);
    return http_server_handle;
  }

  ESP_LOGI(TAG, "http_server_configure: Error starting server!");
  return NULL;
}

/*
 * jQuery get handler requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_j_query_handler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "JQuery Requested");
  httpd_resp_set_type( req, "application/javascript");
  error = httpd_resp_send(req, (const char*)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end-jquery_3_3_1_min_js_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "http_server_j_query_handler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "http_server_j_query_handler: Response Sent Successfully" );
  }
  return error;
}

/*
 * Send the index HTML page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "Index HTML Requested");
  httpd_resp_set_type( req, "text/html");
  error = httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "http_server_index_html_handler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "http_server_index_html_handler: Response Sent Successfully" );
  }
  return error;
}

/*
 * app.css get handler is requested when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "APP CSS Requested");
  httpd_resp_set_type( req, "text/css");
  error = httpd_resp_send(req, (const char*)app_css_start, app_css_end-app_css_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "http_server_app_css_handler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "http_server_app_css_handler: Response Sent Successfully" );
  }
  return error;
}

/*
 * app.js get handler requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "APP JS Requested");
  httpd_resp_set_type( req, "application/javascript");
  error = httpd_resp_send(req, (const char*)app_js_start, app_js_end-app_js_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "http_server_app_js_handler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "http_server_app_js_handler: Response Sent Successfully" );
  }
  return error;
}

/*
 * Sends the .ico file when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_favicon_handler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "Favicon.ico Requested");
  httpd_resp_set_type( req, "image/x-icon");
  error = httpd_resp_send(req, (const char*)favicon_ico_start, favicon_ico_end-favicon_ico_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "http_server_favicon_handler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "http_server_favicon_handler: Response Sent Successfully" );
  }
  return error;
}

/**
 * @brief Receives the *.bin file via the web page and handles the firmware update
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK, other ESP_FAIL if timeout occurs and the update canot be started
 */
//static esp_err_t http_server_ota_update_handler(httpd_req_t *req)
//{
//  esp_err_t error;
//  esp_ota_handle_t ota_handle;
//  char ota_buffer[1024];
//  int content_len = req->content_len;
//  int content_received = 0;
//  int recv_len = 0;
//  bool is_req_body_started = false;
//  bool flash_successful = false;
//
//  const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
//
//  do
//  {
//    /* code */
//  } while ( (recv_len>0) && (content_received < content_len) );
//  return ESP_OK;
//}
//
//static esp_err_t http_server_ota_status_handler(httpd_req_t *req)
//{
//  return ESP_OK;
//}
