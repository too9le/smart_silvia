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

hw_timer_t *stopWatch = NULL;

void app_main(void)
{	
	int state = 0;
	int stateChange = 0;
	initArduino();
	configure_oled();
	float timer = 0.0;
	float tmpWeight    = 0.0;
	float targetWeight = 0.0;

	stopWatch = timerBegin(0, 80, true);

	rotery_init();
	setTargetDisplay();
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	int rotButtonPressed = 0; 
	while(1){
		if(state == 0){
			if (stateChange == 1){
				setTargetDisplay();
				stateChange = 0;
			}
			tmpWeight = getRoteryPossition();
			rotButtonPressed = getButtonPress();
			if(rotButtonPressed == 1)
			{
				state = 1;
				stateChange = 1;
			}
			else if(tmpWeight != targetWeight)
			{
				targetWeight = tmpWeight;
				updateTargetWeight(targetWeight);
			}
		}
		else if(state == 1){
			rotButtonPressed = getButtonPress();
			if (stateChange == 1){
				setBrewDisplay(targetWeight);
				stateChange = 0;
				timerRestart(stopWatch);
			}
			if(rotButtonPressed == 1 || timer > 60.0)
			{
				state = 0;
				stateChange = 1;
			}
			else{
				timer = timerReadSeconds(stopWatch);
				updateBrewTimer(timer);
			}

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
