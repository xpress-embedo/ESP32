#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "sd_mng.h"

// macros
#define MAIN_TASK_PERIOD                (1000)
#define MAX_PARTY_NAME_LEN              (10)       // Assuming Congress party has longest name
#define MAX_NUM_OF_PARTY                (10)      // Let's assume 10 number of parties are maximum

// Private Variables
static const char *TAG = "MAIN";
static const char *filename = MOUNT_POINT"/parties.txt";
static FILE *file;
static uint8_t num_of_parties = 0;
static char party_list[MAX_NUM_OF_PARTY][MAX_PARTY_NAME_LEN] = { 0 };

// Private Function Prototype

void app_main(void)
{
  // initialize SD Card and FAT File System
  sd_mng_init();

  vTaskDelay(10);

  file = fopen( filename, "r" );
  if( NULL == file )
  {
    ESP_LOGE( TAG, "Unable to Open the File");
  }
  else
  {
    char data[MAX_PARTY_NAME_LEN] = { 0 };
    while( fgets( data, sizeof(data), file) )
    {
      // find '\n' and replace with NULL character
      char *pos = strchr( data, '\n' );
      if( pos )
      {
        *pos = '\0';
      }
      ESP_LOGI( TAG, "Party Name: %s", data );
      memcpy( party_list[num_of_parties], data, sizeof(data) );
      memset( data, 0x00, sizeof(data) );
      num_of_parties++;
    }
    fclose(file);
  }

  // unmount
  sd_mng_unmount_card();

  vTaskDelay(10);

  // Check if data fetched is correct or not
  if( num_of_parties < MAX_NUM_OF_PARTY )
  {
    ESP_LOGI( TAG, "Number of Parties: %d", num_of_parties );
  }
  else
  {
    ESP_LOGE( TAG, "Number of Parties: %d, is greater or equal %d", num_of_parties, MAX_NUM_OF_PARTY );
  }
  for( uint8_t idx = 0; idx < num_of_parties; idx++ )
  {
    ESP_LOGI( TAG, "%.2d => %s", (idx+1), party_list[idx] );
  }

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }

}

// Private Function Definition
