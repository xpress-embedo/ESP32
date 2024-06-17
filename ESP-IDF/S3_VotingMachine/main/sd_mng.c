/*
 * sd_mng.c
 *
 *  Created on: Jun 17, 2024
 *      Author: xpress_embedo
 */

#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_mng.h"

// macros
// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO            CONFIG_SD_PIN_MISO
#define PIN_NUM_MOSI            CONFIG_SD_PIN_MOSI
#define PIN_NUM_CLK             CONFIG_SD_PIN_CLK
#define PIN_NUM_CS              CONFIG_SD_PIN_CS

// Private Variables
static const char *TAG = "SD_MNG";
const char mount_point[] = MOUNT_POINT;
static sdmmc_card_t *card;
static esp_vfs_fat_sdmmc_mount_config_t mount_config;
// By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
// For setting a specific frequency, use host.max_freq_khz (range 400kHz - 20MHz for SDSPI)
// Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
static sdmmc_host_t host = SDSPI_HOST_DEFAULT();

// Private Function Prototypes
// todo

// Public Function Definition

/**
 * @brief Initialize SD Card and File System
 * @param  none
 * @return esp_err_t
 */
esp_err_t sd_mng_init( void )
{
  esp_err_t ret;

  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
#ifdef CONFIG_SD_FORMAT_IF_MOUNT_FAILED
  mount_config.format_if_mount_failed = true;
#else
  mount_config.format_if_mount_failed = false;
#endif
  mount_config.max_files = 5;
  mount_config.allocation_unit_size = 16 * 1024;
  
  ESP_LOGI(TAG, "Initializing SD card");

  // Use settings defined above to initialize SD card and mount FAT filesystem.
  // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
  // Please check its source code and implement error recovery when developing
  // production applications.
  ESP_LOGI(TAG, "Using SPI peripheral");

  spi_bus_config_t bus_cfg = 
  {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4000,
  };

  ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
  if (ret != ESP_OK) 
  {
    ESP_LOGE(TAG, "Failed to initialize bus.");
    return ret;
  }

  // mount the sd card
  ret = sd_mng_mount_card();

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  return ret;
}

/**
 * @brief Mount the SD Card
 * @param  None
 * @return ESP_OK if successful else the error code
 */
esp_err_t sd_mng_mount_card( void )
{
  esp_err_t ret;

  // This initializes the slot without card detect (CD) and write protect (WP) signals.
  // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = PIN_NUM_CS;
  slot_config.host_id = host.slot;

  ESP_LOGI(TAG, "Mounting filesystem");
  ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

  if (ret != ESP_OK) 
  {
    if (ret == ESP_FAIL) 
    {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_SD_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    }
    else 
    {
      ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
    }
    return ret;
  }
  
  ESP_LOGI(TAG, "Filesystem mounted");

  return ret;
}

/**
 * @brief Unmount the SD Card and de-initialize the SPI bus
 * @param  None
 * @return ESP_OK always (this is added for future use)
 */
esp_err_t sd_mng_unmount_card( void )
{
  // All done, unmount partition and disable SPI peripheral
  esp_vfs_fat_sdcard_unmount(mount_point, card);
  ESP_LOGI(TAG, "Card unmounted");

  //deinitialize the bus after all devices are removed
  spi_bus_free(host.slot);

  return ESP_OK;
}

/**
 * @brief Format SD Card
 * @param  None
 * @return ESP_OK if formatting is successful else error code
 */
esp_err_t sd_mng_format_card( void )
{
  esp_err_t ret;
  // Format FATFS
  ret = esp_vfs_fat_sdcard_format(mount_point, card);
  if (ret != ESP_OK) 
  {
    ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
  }
  return ret;
}


