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
#define MOUNT_POINT                     "/sdcard"

// Public Function Definition
// Public Function Definition
esp_err_t sd_mng_init( void );
esp_err_t sd_mng_mount_card( void );
esp_err_t sd_mng_unmount_card( void );
esp_err_t sd_mng_format_card( void );

#endif /* MAIN_SD_MNG_H_ */
