#include "main.h"

// Private Macros
#define MAIN_TASK_PERIOD                (1000)

// Private Variables
static const char *TAG = "MAIN";

void app_main(void)
{
	ESP_LOGI( TAG, "Starting Program");
	while( 1 )
	{
		ESP_LOGI( TAG, "Working");
		vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS );
	}
}
