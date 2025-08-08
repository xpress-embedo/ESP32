/*
 * http_server.c
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "wifi_app.h"
#include "http_server.h"

// Private Macros
#define HTTP_SERVER_TASK_SIZE                       (8*1024u)
#define HTTP_SERVER_TASK_PRIORITY                   (4u)
#define HTTP_SERVER_MAX_URI_HANDLERS                (20u)
#define HTTP_SERVER_RECEIVE_WAIT_TIMEOUT            (10u)   // in seconds
#define HTTP_SERVER_SEND_WAIT_TIMEOUT               (10u)   // in seconds

#define HTTP_SERVER_MONITOR_TASK_SIZE               (4*1024u)
#define HTTP_SERVER_MONITOR_TASK_PRIORITY           (3u)
#define HTTP_SERVER_MONITOR_QUEUE_SIZE              (4u)


// Private Variables
static const char TAG[] = "http_server";

// HTTP Server Task Handle
static httpd_handle_t http_server_handle = NULL;
// HTTP Server Monitor Task Handler
static TaskHandle_t task_http_server_monitor = NULL;

// Queue Handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_q_handle;

// WiFi Connect Status
static http_server_wifi_connect_status_e g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECT_NONE;

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
static esp_err_t http_server_wifi_connect_handler(httpd_req_t *req);
static esp_err_t http_server_wifi_connect_status_handler(httpd_req_t *req);
static esp_err_t http_server_get_wifi_connect_info_handler(httpd_req_t *req);
static esp_err_t http_server_wifi_disconnect_json_handler(httpd_req_t *req);

// Public Function Definitions
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
static void http_server_monitor(void *pvParameter)
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
        g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECTING;
        break;
      case HTTP_MSG_WIFI_CONNECT_SUCCESS:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");
        g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECT_SUCCESS;
        break;
      case HTTP_MSG_WIFI_CONNECT_FAIL:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");
        g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECT_FAILED;
        break;
      case HTTP_MSG_WIFI_USER_DISCONNECT:
        ESP_LOGI( TAG, "HTTP_MSG_WIFI_USER_DISCONNECT");
        g_wifi_connect_status = HTTP_WIFI_STATUS_DISCONNECTED;
        break;
      case HTTP_MSG_WIFI_OTA_UPDATE_SUCCESSFUL:
        ESP_LOGI( TAG, "HTTP_MSG_OTA_UPDATE_SUCCESSFUL");
        // fw_update_status = OTA_UPDATE_SUCCESSFUL;
        // http_server_fw_update_reset_timer();
        break;
      case HTTP_MSG_WIFI_OTA_UPDATE_FAILED:
        ESP_LOGI( TAG, "HTTP_MSG_OTA_UPDATE_FAILED");
        // fw_update_status = OTA_UPDATE_FAILED;
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
              HTTP_SERVER_MONITOR_TASK_SIZE, NULL, \
              HTTP_SERVER_MONITOR_TASK_PRIORITY, &task_http_server_monitor);

  // create a message queue
  http_server_monitor_q_handle = xQueueCreate(HTTP_SERVER_MONITOR_QUEUE_SIZE,\
                                              sizeof(http_server_q_msg_t));

  // want to specify core id where the task should run, then use config.code_id
  // note ESP32-S2 has single core only, so this will not work on there

  // Adjust the default priority to 1 less than the wifi application task
  config.task_priority = HTTP_SERVER_TASK_PRIORITY;

  // Specify the Stack Size (default is 4096)
  config.stack_size = HTTP_SERVER_TASK_SIZE;

  // Increase our URI Handlers
  config.max_uri_handlers = HTTP_SERVER_MAX_URI_HANDLERS;

  // Increase the timeout limits
  config.recv_wait_timeout = HTTP_SERVER_RECEIVE_WAIT_TIMEOUT;
  config.send_wait_timeout = HTTP_SERVER_SEND_WAIT_TIMEOUT;

  ESP_LOGI(TAG,
           "http_server_configure: Starting Server on port: '%d' with task priority: '%d'",
           config.server_port, config.task_priority);

  // Start the httpd server port
  if( httpd_start( &http_server_handle, &config) == ESP_OK )
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

    // Register wifiConnect (.json) handler
    httpd_uri_t wifi_connect_json =
    {
      .uri = "/wifiConnect",
      .method    = HTTP_POST,
      .handler   = http_server_wifi_connect_handler,
      .user_ctx  = NULL
    };

    // Register wifiConnectStatus (.json) handler
    httpd_uri_t wifi_connect_status_json =
    {
      .uri = "/wifiConnectStatus",
      .method    = HTTP_POST,
      .handler   = http_server_wifi_connect_status_handler,
      .user_ctx  = NULL
    };

    // Register wifiConnectInfo (.json) handler
    httpd_uri_t wifi_connect_info_json =
    {
      .uri = "/wifiConnectInfo",
      .method    = HTTP_GET,
      .handler   = http_server_get_wifi_connect_info_handler,
      .user_ctx  = NULL
    };

    // Register wifiDisconnect (.json) handler
    httpd_uri_t wifi_disconnect_json =
    {
      .uri = "/wifiDisconnect",
      .method    = HTTP_DELETE,
      .handler   = http_server_wifi_disconnect_json_handler,
      .user_ctx  = NULL
    };

    // Register Query Handler
    httpd_register_uri_handler(http_server_handle, &jquery_js);
    httpd_register_uri_handler(http_server_handle, &index_html);
    httpd_register_uri_handler(http_server_handle, &app_css);
    httpd_register_uri_handler(http_server_handle, &app_js);
    httpd_register_uri_handler(http_server_handle, &favicon_ico);
    httpd_register_uri_handler(http_server_handle, &wifi_connect_json);
    httpd_register_uri_handler(http_server_handle, &wifi_connect_status_json);
    httpd_register_uri_handler(http_server_handle, &wifi_connect_info_json);
    httpd_register_uri_handler(http_server_handle, &wifi_disconnect_json);
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

/*
 * wifiConnect.json handler is invoked after the connect button is pressed and
 * it handles the receiving the SSID and Password entered by the user
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_connect_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "/wifiConnect requested");

  size_t len_ssid = 0, len_password = 0;
  char *ssid_str = NULL, *password_str = NULL;

  // Get the SSID Header
  len_ssid = httpd_req_get_hdr_value_len(req, "my-connect-ssid") + 1;
  if( len_ssid )
  {
    // allocate memory, but make to free it when it is no longer needed
    ssid_str = malloc(len_ssid);
    if( httpd_req_get_hdr_value_str(req, "my-connect-ssid", ssid_str, len_ssid) == ESP_OK )
    {
      ESP_LOGI(TAG, "wifi_connect_handler: Found Header => my-connect-ssid: %s", ssid_str );
    }
  }

  // Get the Password Header
  len_password = httpd_req_get_hdr_value_len(req, "my-connect-pswd") + 1;
  if( len_password )
  {
    // allocate memory, but make to free it when it is no longer needed
    password_str = malloc(len_password);
    if( httpd_req_get_hdr_value_str(req, "my-connect-pswd", password_str, len_password) == ESP_OK )
    {
      ESP_LOGI(TAG, "wifi_connect_handler: Found Header => my-connect-pswd: %s", password_str );
    }
  }

  // Update the WiFi Network Configuration and let the WiFi Application Know
  wifi_config_t * wifi_config = wifi_app_get_wifi_config();
  memset( wifi_config, 0x00, sizeof(wifi_config_t));
  memcpy( wifi_config->sta.ssid, ssid_str, len_ssid );
  memcpy( wifi_config->sta.password, password_str, len_password );

  wifi_app_send_msg( WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER );

  // free the allocated memory
  free(ssid_str);
  free(password_str);

  return ESP_OK;
}

/*
 * wifiConnectStatus handler updates the connection status for the web page
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_connect_status_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "/wifiConnectStatus requested");
  char status_JSON[100];

  sprintf( status_JSON, "{\"wifi_connect_status\":%d}", g_wifi_connect_status );

  httpd_resp_set_type(req, "application/json" );
  httpd_resp_send(req, status_JSON, strlen(status_JSON) );

  return ESP_OK;
}

/*
 * wifiConnectInfo handler updates the web page with the connection information
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_get_wifi_connect_info_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "/wifiConnectInfo.json Requested");

  char ip_info_JSON[200] = { 0 };
  // to be on safer size let's clear the local array, to make sure it doesn't
  // contain any garbage data
  memset(ip_info_JSON, 0x00, sizeof(ip_info_JSON));

  char ip[IP4ADDR_STRLEN_MAX];
  char netmask[IP4ADDR_STRLEN_MAX];
  char gateway[IP4ADDR_STRLEN_MAX];

  if( g_wifi_connect_status == HTTP_WIFI_STATUS_CONNECT_SUCCESS )
  {
    wifi_ap_record_t wifi_data;
    ESP_ERROR_CHECK( esp_wifi_sta_get_ap_info(&wifi_data) );
    char *ssid = (char*)wifi_data.ssid;

    esp_netif_ip_info_t ip_info;
    // get interface's IP address information
    ESP_ERROR_CHECK( esp_netif_get_ip_info(esp_netif_sta, &ip_info) );
    // convert this to human readable form
    esp_ip4addr_ntoa( &ip_info.ip, ip, IP4ADDR_STRLEN_MAX );
    esp_ip4addr_ntoa( &ip_info.netmask, netmask, IP4ADDR_STRLEN_MAX );
    esp_ip4addr_ntoa( &ip_info.gw, gateway, IP4ADDR_STRLEN_MAX );

    sprintf(ip_info_JSON, "{\"ip\":\"%s\",\"netmask\":\"%s\",\"gw\":\"%s\",\"ap\":\"%s\"}", \
            ip, netmask, gateway, ssid );
  }
  else
  {
    ESP_LOGI(TAG, "WiFi Connection is not Successful");
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, ip_info_JSON, strlen(ip_info_JSON));

  return ESP_OK;
}

/*
 * wifiDisconnect handler responds by sending a message to the WiFi Application
 * to disconnect
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_disconnect_json_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "wifiDisconnect.json requested");

  wifi_app_send_msg(WIFI_APP_MSG_USR_REQUESTED_STA_DISCONNECT);
  return ESP_OK;
}

