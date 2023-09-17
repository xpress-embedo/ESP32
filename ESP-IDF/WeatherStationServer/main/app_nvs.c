/*
 * app_nvs.c
 *
 *  Created on: 17-Sep-2023
 *      Author: xpress_embedo
 */


// Public Function Definition

#include "app_nvs.h"
#include "wifi_app.h"

// Tag for logging to the monitor
static const char TAG[] = "NVS";

// NVS name space used for station mode credentials
const char app_nvs_sta_creds_namespace[] = "stacreds";

/*
 * Saves Station Mode WiFi Credentials to NVS
 * @return ESP_OK if successful
 */
esp_err_t app_nvs_save_sta_creds( void )
{
  nvs_handle handle;
  esp_err_t esp_err;
  ESP_LOGI(TAG, "app_nvs_save_sta_creds: Saving Station mode credentials to flash");

  wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();
  if( wifi_sta_config )
  {
    esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
    if( esp_err != ESP_OK )
    {
      ESP_LOGI(TAG, "app_nvs_save_sta_creds: Error (%s) opening NVS handle.", esp_err_to_name(esp_err));
      return esp_err;
    }

    // Set SSID
    esp_err = nvs_set_blob(handle, "ssid", wifi_sta_config->sta.ssid, WIFI_MAX_SSID_LEN);
    if( esp_err != ESP_OK )
    {
      ESP_LOGI(TAG, "app_nvs_save_sta_creds: Error (%s) setting SSID to NVS.", esp_err_to_name(esp_err));
      return esp_err;
    }

    // Set Password
    esp_err = nvs_set_blob(handle, "password", wifi_sta_config->sta.password, WIFI_MAX_PASSWORD_LEN);
    if( esp_err != ESP_OK )
    {
      ESP_LOGI(TAG, "app_nvs_save_sta_creds: Error (%s) setting Password to NVS.", esp_err_to_name(esp_err));
      return esp_err;
    }

    // Commit Credentials
    esp_err = nvs_commit(handle);
    if( esp_err != ESP_OK )
    {
      ESP_LOGI(TAG, "app_nvs_save_sta_creds: Error (%s) committing credentials to NVS.", esp_err_to_name(esp_err));
      return esp_err;
    }

    nvs_close(handle);
    // note: safety issue, we shouldn't disclose our password over serial terminal
    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Wrote wifi_sta_config: Station SSID: %s, Password: %s", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
  }
  ESP_LOGI(TAG, "app_nvs_save_sta_creds: returned ESP_OK");
  return ESP_OK;
}

/*
 * Loads Previously Saved Credentials from the NVS
 * @return true if previously saved credentials were found.
 */
bool app_nvs_load_sta_creds( void )
{
  nvs_handle handle;
  esp_err_t esp_err;
  ESP_LOGI(TAG, "app_nvs_load_sta_creds: Loading WiFi credentials from Flash");

  if( nvs_open(app_nvs_sta_creds_namespace, NVS_READONLY, &handle) == ESP_OK )
  {
    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();
    // If NULL then allocate memory
    if( wifi_sta_config == NULL )
    {
      // Allocate memory for the WiFi Configuration
      wifi_sta_config = (wifi_config_t*)malloc(sizeof(wifi_config_t));
    }
    memset(wifi_sta_config, 0x00, sizeof(wifi_config_t) );

    // allocate local buffer, to copy the data from it to the wifi_config
    size_t buffer_size = sizeof(wifi_config_t);
    uint8_t *buffer = (uint8_t*)malloc( sizeof(uint8_t) * buffer_size );
    memset( buffer, 0x00, buffer_size);

    // Load SSID
    buffer_size = sizeof(wifi_sta_config->sta.ssid);
    esp_err = nvs_get_blob(handle, "ssid", buffer, &buffer_size);
    if( esp_err != ESP_OK )
    {
      free(buffer);
      ESP_LOGI(TAG, "app_nvs_load_sta_creds: (%s) no station SSID found in NVS", esp_err_to_name(esp_err) );
      return false;
    }
    // copy the ssid read from nvs to program i.e. wifi_config
    memcpy(wifi_sta_config->sta.ssid, buffer, buffer_size);

    // Load Password
    buffer_size = sizeof(wifi_sta_config->sta.password);
    esp_err = nvs_get_blob(handle, "password", buffer, &buffer_size);
    if( esp_err != ESP_OK )
    {
      free(buffer);
      ESP_LOGI(TAG, "app_nvs_load_sta_creds: (%s) retrieving password", esp_err_to_name(esp_err) );
      return false;
    }
    // copy the password read from nvs to program i.e. wifi_config
    memcpy(wifi_sta_config->sta.password, buffer, buffer_size);

    free(buffer);
    nvs_close(handle);
    ESP_LOGI(TAG, "app_nvs_load_sta_creds: Found SSID: %s, and Password: %s", \
             wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
    if( wifi_sta_config->sta.ssid[0] != '\0' )
    {
      return true;
    }
    else
    {
      return false;
    }
    // the above statement can be shortened like this also
    // return (wifi_sta_config->sta.ssid[0] != '\0')
  }
  else
  {
    return false;
  }

  return true;
}

/*
 * Clears the Station Mode Credentials from NVS
 * @return ESP_OK if successful
 */
esp_err_t app_nvs_clear_sta_creds( void )
{
  nvs_handle handle;
  esp_err_t esp_err;
  ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Clearing WiFi Station mode credentials from the flash");

  esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
  if( esp_err != ESP_OK )
  {
    ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Error (%s) opening NVS handle", esp_err_to_name(esp_err));
    return esp_err;
  }

  // Erase Credentials
  esp_err = nvs_erase_all(handle);
  if( esp_err != ESP_OK )
  {
    ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Error (%s) NVS Commit", esp_err_to_name(esp_err));
    return esp_err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "app_nvs_clear_sta_creds: returned ESP_OK");

  return ESP_OK;
}
