/*
 * sd_mng.h
 *
 *  Created on: Jun 17, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_SD_MNG_H_
#define MAIN_SD_MNG_H_

#include <unistd.h>
#include "esp_system.h"

// macros
#define SD_MAX_CHAR_SIZE            64
#define MOUNT_POINT                 "/sdcard"

// Public Function Definition
esp_err_t sd_mng_init( void );
esp_err_t sd_mng_mount_card( void );
esp_err_t sd_mng_unmount_card( void );
esp_err_t sd_mng_format_card( void );
esp_err_t sd_mng_write_file( const char *path, char *data );
esp_err_t sd_mng_read_file( const char *path, char *data, uint8_t max_length );

#endif /* MAIN_SD_MNG_H_ */
