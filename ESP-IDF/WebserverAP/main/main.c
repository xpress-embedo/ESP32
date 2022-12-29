
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "driver/gpio.h"

/* This Example use WiFi config. that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

#define RED_LED                    (GPIO_NUM_21)

// Private Function Declaration
static void led_configure( void );
static void led_set_status( bool status );
static void wifi_init_softap(void);

static esp_err_t led_off_handler(httpd_req_t *req);
static esp_err_t led_on_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

static httpd_handle_t start_webserver(void);
static esp_err_t stop_webserver(httpd_handle_t server);

// Handler Function
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);

// Private Constant Variables
static const char *TAG = "WebServer";

char led_on_html_code[] =
  "<!DOCTYPE html>\
  <html>\
  <head>\
  <style>\
  .button {\
    border: none;\
    color: white;\
    padding: 15px 32px;\
    text-align: center;\
    text-decoration: none;\
    display: inline-block;\
    font-size: 16px;\
    margin: 4px 2px;\
    cursor: pointer;\
  }\
  .led_button {background-color: #4CAF50;} /* Green */\
  </style>\
  </head>\
  <body>\
  <h1>ESP32 WebServer Application</h1>\
  <p>On Board LED is OFF</p>\
  <button class=\"button led_button\" onclick=\"window.location.href='/ledon'\">LED ON</button>\
  </body>\
  </html>";

char led_off_html_code[] =
  "<!DOCTYPE html>\
  <html>\
  <head>\
  <style>\
  .button {\
    border: none;\
    color: white;\
    padding: 15px 32px;\
    text-align: center;\
    text-decoration: none;\
    display: inline-block;\
    font-size: 16px;\
    margin: 4px 2px;\
    cursor: pointer;\
  }\
  .led_button {background-color: #000000;} /* Black */\
  </style>\
  </head>\
  <body>\
  <h1>ESP32 WebServer Application</h1>\
  <p>On Board LED is ON</p>\
  <button class=\"button led_button\" onclick=\"window.location.href='/ledoff'\">LED OFF</button>\
  </body>\
  </html>";

static const httpd_uri_t ledoff =
{
  .uri       = "/ledoff",
  .method    = HTTP_GET,
  .handler   = led_off_handler,
  .user_ctx  = led_on_html_code
};

static const httpd_uri_t ledon =
{
  .uri       = "/ledon",
  .method    = HTTP_GET,
  .handler   = led_on_handler,
  .user_ctx  = led_off_html_code
};

static const httpd_uri_t root =
{
  .uri       = "/",
  .method    = HTTP_GET,
  .handler   = led_off_handler,
  .user_ctx  = led_off_html_code
};

static httpd_handle_t server = NULL;


void app_main(void)
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if( (ret==ESP_ERR_NVS_NO_FREE_PAGES) || (ret==ESP_ERR_NVS_NEW_VERSION_FOUND) )
  {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Configure the LED
  led_configure();

  ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
  wifi_init_softap();
}

// Private Function Definitions
static void led_configure( void )
{
  gpio_reset_pin( RED_LED );
  gpio_set_direction( RED_LED, GPIO_MODE_OUTPUT );
}

static void led_set_status( bool status )
{
  if( status )
  {
    gpio_set_level(RED_LED, 1);
  }
  else
  {
    gpio_set_level(RED_LED, 0);
  }
}

static void wifi_init_softap( void )
{
  /* Initialize the Network Interface or we can say initialize underlying TCP/IP
  stack, this function should be called exactly once, when the application
  starts */
  ESP_ERROR_CHECK( esp_netif_init() );

  // Create Default Event Loop
  ESP_ERROR_CHECK( esp_event_loop_create_default() );

  /* This creates default WIFI AP. In case of any init error this API aborts.
  The API creates esp_netif object with default WiFi access point config,
  attaches the netif to wifi and registers default wifi handlers.*/
  esp_netif_create_default_wifi_ap();

  /* Initialize WiFi Allocate resource for WiFi driver, such as WiFi control
  structure, RX/TX buffer, WiFi NVS structure etc. This WiFi also starts WiFi
  task.
  Note: This API must be called before all other WiFi API can be called.
  Note: Always use WIFI_INIT_CONFIG_DEFAULT macro to initialize the
  configuration to default values, this can guarantee all the fields get correct
  value when more fields are added into wifi_init_config_t in future release.
  If you want to set your own initial values, overwrite the default values which
  are set by WIFI_INIT_CONFIG_DEFAULT.
  Please be notified that the field 'magic' of wifi_init_config_t should always
  be WIFI_INIT_CONFIG_MAGIC! */
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  // Register an instance of event handler to the default loop.
  ESP_ERROR_CHECK(esp_event_handler_instance_register( WIFI_EVENT,\
                                                       ESP_EVENT_ANY_ID,\
                                                       &wifi_event_handler,\
                                                       NULL,
                                                       NULL) );
  ESP_ERROR_CHECK(esp_event_handler_register( IP_EVENT, \
                                              IP_EVENT_AP_STAIPASSIGNED, \
                                              &connect_handler, \
                                              &server) );
  ESP_ERROR_CHECK(esp_event_handler_register( WIFI_EVENT, \
                                              WIFI_EVENT_STA_DISCONNECTED, \
                                              &disconnect_handler, \
                                              &server) );

  wifi_config_t wifi_config =
  {
    .ap =
    {
      // this data will come from SDK Config, Example Configuration section
      .ssid = EXAMPLE_ESP_WIFI_SSID,
      .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
      .channel = EXAMPLE_ESP_WIFI_CHANNEL,
      .password = EXAMPLE_ESP_WIFI_PASS,
      .max_connection = EXAMPLE_MAX_STA_CONN,
      .authmode = WIFI_AUTH_WPA_WPA2_PSK,
      .pmf_cfg =
      {
        .required = false,
      },
    },
  };

  // if no password is specified change the authenticate mode to open, this is
  // used for open networks
  if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
  {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  // Set the operating mode, here we are setting the Access Point Mode
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

  // Set the configuration of the ESP32 STA or AP
  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &wifi_config) );
  /* Start WiFi according to current configuration If mode is WIFI_MODE_STA, it
  create station control block and start station If mode is WIFI_MODE_AP, it
  create soft-AP control block and start soft-AP If mode is WIFI_MODE_APSTA,
  it create soft-AP and station control block and start soft-AP and station */
  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_LOGI( TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", \
            EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, \
            EXAMPLE_ESP_WIFI_CHANNEL);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
  }
}

static httpd_handle_t start_webserver(void)
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK)
  {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &ledoff);
    httpd_register_uri_handler(server, &ledon);
    httpd_register_uri_handler(server, &root);
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
  // Stop the httpd server
  return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
  httpd_handle_t* server = (httpd_handle_t*) arg;
  if (*server)
  {
    ESP_LOGI(TAG, "Stopping webserver");
    if (stop_webserver(*server) == ESP_OK)
    {
      *server = NULL;
    }
    else
    {
      ESP_LOGE(TAG, "Failed to stop http server");
    }
  }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
  httpd_handle_t* server = (httpd_handle_t*) arg;
  if (*server == NULL)
  {
    ESP_LOGI(TAG, "Starting webserver");
    *server = start_webserver();
  }
}

/* An HTTP GET handler */
static esp_err_t led_off_handler( httpd_req_t *req )
{
  esp_err_t error;
  ESP_LOGI( TAG, "LED Turned OFF" );
  led_set_status(0);
  const char *response = (const char *)req->user_ctx;
  error = httpd_resp_send(req, response, strlen(response) );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "Response Sent Successfully" );
  }
  return error;
}

static esp_err_t led_on_handler( httpd_req_t *req )
{
  esp_err_t error;
  ESP_LOGI( TAG, "LED Turned ON" );
  led_set_status(1);
  const char *response = (const char *)req->user_ctx;
  error = httpd_resp_send(req, response, strlen(response) );
  if( error != ESP_OK )
  {
    ESP_LOGI( TAG, "Error %d while sending Response", error );
  }
  else
  {
    ESP_LOGI( TAG, "Response Sent Successfully" );
  }
  return error;
}

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
  /* For any other URI send 404 and close socket */
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}


