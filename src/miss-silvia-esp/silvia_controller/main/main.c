#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"

#include "Arduino.h"

#include "ssd1306.h"
#include "font8x8_basic.h"
#include "oled_controller.h"

#include "encoder_wrapper.c"

void app_main(void)
{	
	int state = 0;
	initArduino();
	configure_oled();
	float timer = 0.0;
	float tmpWeight    = 0.0;
	float targetWeight = 0.0;
	rotery_init();
	setTargetDisplay();
	vTaskDelay(1000 / portTICK_PERIOD_MS);


	while(1){
		if(state == 0){
			tmpWeight = getRoteryPossition();
			if(tmpWeight != targetWeight)
			{
				targetWeight = tmpWeight;
				updateTargetWeight(targetWeight);
			}
		}
		else if(state == 1){
			setBrewDisplay(40.0);
			timer +=1;
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			updateBrewTimer(timer);
		}
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
