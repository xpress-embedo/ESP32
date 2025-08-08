/*
 * app_nvs.h
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#ifndef MAIN_APP_NVS_H_
#define MAIN_APP_NVS_H_

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// Public Function Prototypes
esp_err_t app_nvs_save_sta_creds( void );
bool app_nvs_load_sta_creds( void );
esp_err_t app_nvs_clear_sta_creds( void );

#endif /* MAIN_APP_NVS_H_ */
