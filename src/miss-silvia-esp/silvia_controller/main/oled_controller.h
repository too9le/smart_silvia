#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define tag "SSD1306"

SSD1306_t dev;

int configure_oled(void);
int setTargetDisplay(void);
int updateTargetWeight(float weight);
int setBrewDisplay(float targetWeight);
int updateBrewTimer(float);
int updateBrewWeight(float weight);