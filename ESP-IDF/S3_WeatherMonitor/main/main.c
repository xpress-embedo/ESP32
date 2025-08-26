#include "main.h"
#include "gui_mng.h"

// Private Macros
#define MAIN_TASK_PERIOD                (5000)

// Private Variables
static const char *TAG = "MAIN";

void app_main(void)
{
	ESP_LOGI( TAG, "Starting Program");
	
	gui_start();
	
	while( 1 )
	{
		ESP_LOGI( TAG, "Working");
		vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS );
	}
}
