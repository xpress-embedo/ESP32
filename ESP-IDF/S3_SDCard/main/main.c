/* This is the SD Card project, all the experiments related to SD card will be
 * done here, inside this repository
 */
#include <sys/unistd.h>
#include <string.h>

#include "esp_log.h"
#include "sd_mng.h"

// macros

// Private Variables
static const char *TAG = "MAIN";

// Private Function Definitions

void app_main( void )
{
  char file_data[SD_MAX_CHAR_SIZE] = { 0 };

  sd_mng_init();    // initialize the sd card and mount fat file system
  const char *filename = MOUNT_POINT"/parties.txt";
  FILE *write_file = fopen( filename, "w" );
  if( write_file == NULL )
  {
    ESP_LOGE(TAG, "Failed to Open file for Writing" );
  }
  else
  {
    char data[] = "BJP\nCongress\nBSP\nSP\nTMC\nJDU\n";
    fprintf(write_file, data);
    fclose(write_file);
  }

  FILE *read_file = fopen( filename, "r" );
  if( write_file == NULL )
  {
    ESP_LOGE(TAG, "Failed to Open file for Reading" );
  }
  else
  {
    static uint8_t idx = 0;
    while( fgets( file_data, sizeof(file_data), read_file) )
    {
      char *pos = strchr( file_data, '\n' );
      if( pos )
      {
        // replace \n with NULL character
        *pos = '\0';
      }
      idx++;
      ESP_LOGI( TAG, "%d -> %s", idx, file_data );
    }
  }
  sd_mng_unmount_card();
}

// Commented for now
// void app_main(void)
// {
//   esp_err_t ret;

//   ret = sd_mng_init();
  
//   // Use POSIX and C standard library functions to work with files.
//   // First create a file.
//   const char *file_hello = MOUNT_POINT"/parties.txt";
//   /*
//   char data[SD_MAX_CHAR_SIZE];
//   snprintf(data, SD_MAX_CHAR_SIZE, "%s\n", "Hello World");
//   ret = sd_mng_write_file(file_hello, data);
//   if (ret != ESP_OK)
//   {
//     return;
//   }
//   */

//   // const char *file_foo = MOUNT_POINT"/foo.txt";

//   // // Check if destination file exists before renaming
//   // struct stat st;
//   // if (stat(file_foo, &st) == 0)
//   // {
//   //   // Delete it if it exists
//   //   unlink(file_foo);
//   // }

//   // // Rename original file
//   // ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
//   // if (rename(file_hello, file_foo) != 0) 
//   // {
//   //   ESP_LOGE(TAG, "Rename failed");
//   //   return;
//   // }

//   ret = sd_mng_read_file(file_hello);
//   if (ret != ESP_OK)
//   {
//     return;
//   }

//   // const char *file_nihao = MOUNT_POINT"/nihao.txt";
//   // memset(data, 0, SD_MAX_CHAR_SIZE);
//   // snprintf(data, SD_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
//   // ret = write_file(file_nihao, data);
//   // if (ret != ESP_OK) 
//   // {
//   //   return;
//   // }

//   // //Open file for reading
//   // ret = read_file(file_nihao);
//   // if (ret != ESP_OK) 
//   // {
//   //   return;
//   // }

//   sd_mng_unmount_card();
// }

// Private Function Definitions
// todo
