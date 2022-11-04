#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#include "oled_controller.h"

void app_main(void)
{	
	configure_oled();
	float timer = 0.0;
	setTargetDisplay();
	vTaskDelay(10000 / portTICK_PERIOD_MS);

	setBrewDisplay(40.0);
	while(1){
		timer +=1;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		updateBrewTimer(timer);

	}

	// while(1){
	// 	char * timer_val:
	// 	char * weight_val;

	// 	timer_val = "0.0"
	// 	weight_val = "0.0"

	// }
	// Restart module
	esp_restart();
}
