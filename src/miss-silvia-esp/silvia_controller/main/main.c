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

#include "driver/gpio.h"

#include "HX711.h"

hw_timer_t *stopWatch = NULL;

void app_main(void)
{	
	// Define all our constats these could likely be pound defines
	// 1. HX711 circuit wiring
	const int LOADCELL_SCK_PIN  = 16;
	const int LOADCELL_DOUT_PIN = 17;

	// 2. Adjustment settings
	const long LOADCELL_OFFSET = 0;
	const long LOADCELL_DIVIDER = 1;

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

	//initilize water sesnor
	gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);


	// 3. Initialize library
	HX711_init(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, eGAIN_128 );
	HX711_power_up();
	HX711_set_scale(LOADCELL_DIVIDER);
	HX711_set_offset(LOADCELL_OFFSET);
	HX711_tare( );
	// 4. Acquire reading
	printf("Weight %fg \n", HX711_get_units(10));
	// scale done

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
				timer = 0;
			}
			if(rotButtonPressed == 1 || timer > 60.0)
			{
				state = 0;
				stateChange = 1;
			}
			else{
				tmpWeight = HX711_get_units(1);

				timer = timerReadSeconds(stopWatch);
				updateBrewTimer(timer);
				updateBrewWeight(tmpWeight);
				ESP_LOGI(TAG, "Weight %f g", tmpWeight);

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
