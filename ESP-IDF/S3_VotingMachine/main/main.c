#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "main.h"
#include "gui_mng.h"
#include "sd_mng.h"

// macros
#define MAIN_TASK_PERIOD                (1000)

// Private Variables
static const char *TAG = "MAIN";
static const char *filename = MOUNT_POINT"/parties.txt";
static FILE *file;
static uint8_t num_of_parties = 0;
static char party_list[MAX_NUM_OF_PARTY][MAX_PARTY_NAME_LEN] = { 0 };

// Private Function Prototype

void app_main(void)
{
  esp_err_t err;

  // initialize SD Card and FAT File System
  err = sd_mng_init();

  vTaskDelay(10);

  if( err == ESP_OK )
  {
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
  }
  else
  {
    ESP_LOGE( TAG, "Problem with SD Card");
    num_of_parties = 0;
  }

  vTaskDelay(10);

  // Check if data fetched is correct or not
  if( num_of_parties && (num_of_parties <= MAX_NUM_OF_PARTY) )
  {
    ESP_LOGI( TAG, "Number of Parties: %d", num_of_parties );
    // optional: logging/printing parties on terminal
    for( uint8_t idx = 0; idx < num_of_parties; idx++ )
    {
      ESP_LOGI( TAG, "%.2d => %s", (idx+1), party_list[idx] );
    }
  }
  else
  {
    ESP_LOGE( TAG, "Number of Parties: %d, which is invalid, and maximum number of parties are %d", num_of_parties, MAX_NUM_OF_PARTY );
  }

  // start the gui task, this will handle all the display related stuff
  gui_start();

  while (true)
  {
    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

// Public Function Definitions

/**
 * @brief Number of Political Parties
 * @param  None
 * @return num of political parties
 */
uint8_t get_number_of_parties( void )
{
  return num_of_parties;
}

/**
 * @brief Returns the name of the Party
 * @param party_idx 
 * @return Pointer to the name of the party
 */
char * get_name_of_party( uint8_t party_idx )
{
  if( party_idx < MAX_NUM_OF_PARTY )
  {
    return party_list[party_idx];
  }
  else
  {
    return NULL;
  }
}

// Private Function Definition
