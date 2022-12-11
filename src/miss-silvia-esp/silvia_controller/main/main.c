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


#define WATER_LEVEL_PIN 17
#define RELAY_PIN_1 32
#define RELAY_PIN_2 33

hw_timer_t *stopWatch = NULL;



void app_main(void)
{	
	// Define all our constats these could likely be pound defines
	// 1. HX711 circuit wiring
	const int LOADCELL_1_SCK_PIN  = 0;
	const int LOADCELL_1_DOUT_PIN = 4;
	const int LOADCELL_2_SCK_PIN  = 2;
	const int LOADCELL_2_DOUT_PIN = 15;
	// 2. Adjustment settings
	const long LOADCELL_OFFSET = 0;

	// this number was done from measurments using a known kitchen scale:
	// reading / known weight
	// 46186 /97.4g
	const long LOADCELL_DIVIDER = 474.1889;


	int state = 0;
	int stateChange = 0;

	float timer = 0.0;
	float tmpWeight    = 0.0;
	float targetWeight = 0.0;

	float weight1 = 0.0;
	float weight2 = 0.0;

	int brewNumber = 0;

	initArduino();
	configure_oled();

	stopWatch = timerBegin(0, 80, true);

	rotery_init();
	int rotButtonPressed = 0; 

	//initilize water sesnor
	gpio_set_direction(WATER_LEVEL_PIN, GPIO_MODE_INPUT);

	// initilize relay pins
	gpio_set_direction(RELAY_PIN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY_PIN_2, GPIO_MODE_OUTPUT);
	gpio_set_level(RELAY_PIN_1, 0);
	gpio_set_level(RELAY_PIN_2, 0);

	// // 3. Initialize library
	HX711_init(LOADCELL_1_DOUT_PIN, LOADCELL_1_SCK_PIN, eGAIN_128 );
	HX711_power_up();
	HX711_set_offset(LOADCELL_OFFSET);
	HX711_tare( );

	HX711_init_2(LOADCELL_2_DOUT_PIN, LOADCELL_2_SCK_PIN, eGAIN_128 );
	HX711_power_up_2();
	HX711_set_offset_2(LOADCELL_OFFSET);
	HX711_tare_2( );
	//shared function
	HX711_set_scale(LOADCELL_DIVIDER);
	// 4. Acquire reading --debug
	// while(1){
	// 	printf("Weight 1:%fg \n", HX711_get_units(10));
	// 	printf("Weight 2:%fg \n", HX711_get_units_2(10));

	// }
	// scale done

	setTargetDisplay();

	while(1){
		// printf("HEY LISTEN WATER LEVEL: %d\n", gpio_get_level(WATER_LEVEL_PIN));
		if(state == 0){
			if (stateChange == 1){
				setTargetDisplay();
				stateChange = 0;
			}
			rotButtonPressed = getButtonPress();
			tmpWeight = getRoteryPossition();
			if(rotButtonPressed == 1)
			{
				state = 4;
				stateChange = 1;
			}
			else if(tmpWeight != targetWeight)
			{
				targetWeight = tmpWeight;
				updateTargetWeight(targetWeight);
			}
			// scommenting out so I don't have to worry about water level
			if (gpio_get_level(WATER_LEVEL_PIN) == 0) {
				stateChange = 1;
				state = 2;
			}
		}
		else if(state == 1){
			if (stateChange == 1){
				setBrewDisplay(targetWeight);
				
				stateChange = 0;
				timer = 0;
				tmpWeight = 0;
				


				timerRestart(stopWatch);

				// start relays
				gpio_set_level(RELAY_PIN_1, 1);
				gpio_set_level(RELAY_PIN_2, 1);
			}
			rotButtonPressed = getButtonPress();
			if(rotButtonPressed == 1 || timer > 60.0 || targetWeight < tmpWeight)
			{
				state = 3;
				stateChange = 1;
				gpio_set_level(RELAY_PIN_1, 0);
				gpio_set_level(RELAY_PIN_2, 0);
				brewNumber++;
			}
			else{
				weight1 = HX711_get_units(1);
				timer = timerReadSeconds(stopWatch);
				updateBrewTimer(timer);
				weight2 = HX711_get_units_2(1);
				
				timer = timerReadSeconds(stopWatch);
				updateBrewTimer(timer);
				
				tmpWeight = weight1 + weight2;
				
				updateBrewWeight(tmpWeight);
				ESP_LOGI(TAG, "Weight %f g", tmpWeight);

			}

		}
		else if(state == 2){
			if (stateChange == 1){
				lowWaterMSG();
				stateChange = 0;
			}
			if (gpio_get_level(WATER_LEVEL_PIN) == 1) {
				stateChange = 1;
				state = 0;
			}
			blinkLowWaterMSG(500);
		}
		else if(state == 3){
			if (stateChange == 1){
				showBrewSummary(brewNumber, targetWeight, tmpWeight, timer);
				stateChange = 0;
			}
			rotButtonPressed = getButtonPress();
			if(rotButtonPressed == 1)
			{
				state = 0;
				stateChange = 1;
			}
		}
		else if (state == 4){
			tareScaleMSG();
			HX711_tare( );
			HX711_tare_2( );

			state = 1;
			stateChange = 1;
		}
	}
	esp_restart();
}
