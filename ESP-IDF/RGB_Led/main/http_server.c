/*
 * http_server.c
 *
 *  Created on: 16-Jan-2023
 *      Author: xpress_embedo
 */
#include "esp_http_server.h"
#include "esp_log.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

// Private Function Prototypes
static httpd_handle_t HTTP_Server_Configure( void );
static esp_err_t HTTP_Server_jQueryHandler(httpd_req_t *req);
static esp_err_t HTTP_Server_IndexHTMLHandler(httpd_req_t *req);
static esp_err_t HTTP_Server_AppCSSHandler(httpd_req_t *req);
static esp_err_t HTTP_Server_AppJSHandler(httpd_req_t *req);
static esp_err_t HTTP_Server_FaviconIcoHandler(httpd_req_t *req);

// TAG used for ESP Serial Console Messages
static const char TAG[] = "http_server";

// HTTP Server Task Handle
static httpd_handle_t http_server_handle = NULL;

// Register jQuery handler
static const httpd_uri_t jquery_js =
{
  .uri       = "/jquery-3.3.1.min.js",
  .method    = HTTP_GET,
  .handler   = HTTP_Server_jQueryHandler,
  .user_ctx  = NULL
};

// Register index.html handler
static const httpd_uri_t index_html =
{
  .uri       = "/",
  .method    = HTTP_GET,
  .handler   = HTTP_Server_IndexHTMLHandler,
  .user_ctx  = NULL
};

// Register app.css handler
static const httpd_uri_t app_css =
{
  .uri       = "/app.css",
  .method    = HTTP_GET,
  .handler   = HTTP_Server_AppCSSHandler,
  .user_ctx  = NULL
};

// Register app.js handler
static const httpd_uri_t app_js =
{
  .uri       = "/app.js",
  .method    = HTTP_GET,
  .handler   = HTTP_Server_AppJSHandler,
  .user_ctx  = NULL
};

// Register favicon.ico handler
static const httpd_uri_t favicon_ico =
{
  .uri       = "/favicon.ico",
  .method    = HTTP_GET,
  .handler   = HTTP_Server_FaviconIcoHandler,
  .user_ctx  = NULL
};

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

/*
 * Send a message to the Queue
 * @param msg_id message ID from the HTTP_Server_Msg_e enum
 * @return pdTRUE if an item was successfully send to the queue, otherwise pdFALSE
 * @note Expand the parameter list based on your requirements
 */
BaseType_t HTTP_Server_Monitor_SendMsg( HTTP_Server_Msg_e msg_id )
{
  return pdFALSE;
}

/*
 * Starts the HTTP Server
 */
void HTTP_Server_Start( void )
{
  if( http_server_handle == NULL )
  {
    http_server_handle = HTTP_Server_Configure();
  }
}

/*
 * Stops the HTTP Server
 */
void HTTP_Server_Stop( void )
{
  if( http_server_handle )
  {
    httpd_stop(http_server_handle);
    ESP_LOGI(TAG, "HTTP_Server_Stop: Stopping the HTTP Server");
    http_server_handle = NULL;
  }
}

// Private Function Definitions
/*
 * Set up the default httpd server configuration.
 * @return http server instance handle if successful, NULL otherwise
 */
static httpd_handle_t HTTP_Server_Configure( void )
{
  // Generate the Default Configuration
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // TODO: create HTTP Monitor Task

  // TODO: Create the message queue

  // the core that the HTTP server will run on
  config.core_id = HTTP_SERVER_TASK_CORE_ID;
  // adjust the default priority to 1 less than the Wi-Fi Application
  config.task_priority = HTTP_SERVER_TASK_PRIORITY;
  // bump up the stack size (default is 4096)
  config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
  // increase uri handlers
  config.max_uri_handlers = 20;
  // increase timeout limits
  config.recv_wait_timeout = 10;
  config.send_wait_timeout = 10;

  ESP_LOGI( TAG,
            "HTTP_Server_Configure: Starting Server on Port: %d with Priority: %d",
            config.server_port, config.task_priority );

  if (httpd_start(&http_server_handle, &config) == ESP_OK)
  {
    // Set URI handlers
    ESP_LOGI(TAG, "HTTP_Server_Configure: Registering URI handlers");
    httpd_register_uri_handler(http_server_handle, &jquery_js);
    httpd_register_uri_handler(http_server_handle, &index_html);
    httpd_register_uri_handler(http_server_handle, &app_css);
    httpd_register_uri_handler(http_server_handle, &app_js);
    httpd_register_uri_handler(http_server_handle, &favicon_ico);
    return http_server_handle;
  }

  ESP_LOGI(TAG, "HTTP_Server_Configure: Error starting server!");
  return NULL;
}

/*
 * jQuery get handler requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t HTTP_Server_jQueryHandler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "JQuery Requested");
  httpd_resp_set_type( req, "application/javascript");
  error = httpd_resp_send(req, (const char*)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end-jquery_3_3_1_min_js_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "HTTP_Server_jQueryHandler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "HTTP_Server_jQueryHandler: Response Sent Successfully" );
  }
  return error;
}

/*
 * Send the index HTML page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t HTTP_Server_IndexHTMLHandler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "Index HTML Requested");
  httpd_resp_set_type( req, "text/html");
  error = httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "HTTP_Server_IndexHTMLHandler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "HTTP_Server_IndexHTMLHandler: Response Sent Successfully" );
  }
  return error;
}

/*
 * app.css get handler is requested when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t HTTP_Server_AppCSSHandler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "APP CSS Requested");
  httpd_resp_set_type( req, "text/css");
  error = httpd_resp_send(req, (const char*)app_css_start, app_css_end-app_css_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "HTTP_Server_AppCSSHandler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "HTTP_Server_AppCSSHandler: Response Sent Successfully" );
  }
  return error;
}

/*
 * app.js get handler requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t HTTP_Server_AppJSHandler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "APP JS Requested");
  httpd_resp_set_type( req, "application/javascript");
  error = httpd_resp_send(req, (const char*)app_js_start, app_js_end-app_js_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "HTTP_Server_AppJSHandler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "HTTP_Server_AppJSHandler: Response Sent Successfully" );
  }
  return error;
}

/*
 * Sends the .ico file when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK
 */
static esp_err_t HTTP_Server_FaviconIcoHandler(httpd_req_t *req)
{
  esp_err_t error;
  ESP_LOGI(TAG, "Favicon.ico Requested");
  httpd_resp_set_type( req, "image/x-icon");
  error = httpd_resp_send(req, (const char*)favicon_ico_start, favicon_ico_end-favicon_ico_start );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "HTTP_Server_FaviconIcoHandler: Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "HTTP_Server_FaviconIcoHandler: Response Sent Successfully" );
  }
  return error;
}
