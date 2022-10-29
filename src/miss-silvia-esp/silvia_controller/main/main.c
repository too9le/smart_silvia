#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

/*
 You have to set this config value with menuconfig
 CONFIG_INTERFACE

 for i2c
 CONFIG_MODEL
 CONFIG_SDA_GPIO
 CONFIG_SCL_GPIO
 CONFIG_RESET_GPIO

 for SPI
 CONFIG_CS_GPIO
 CONFIG_DC_GPIO
 CONFIG_RESET_GPIO
*/

#define tag "SSD1306"

void app_main(void)
{
	SSD1306_t dev;
	int center, top, bottom;
	char lineChar[20];

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SPI_INTERFACE
	ESP_LOGI(tag, "INTERFACE is SPI");
	ESP_LOGI(tag, "CONFIG_MOSI_GPIO=%d",CONFIG_MOSI_GPIO);
	ESP_LOGI(tag, "CONFIG_SCLK_GPIO=%d",CONFIG_SCLK_GPIO);
	ESP_LOGI(tag, "CONFIG_CS_GPIO=%d",CONFIG_CS_GPIO);
	ESP_LOGI(tag, "CONFIG_DC_GPIO=%d",CONFIG_DC_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_SPI_INTERFACE

#if CONFIG_FLIP
	dev._flip = true;
	ESP_LOGW(tag, "Flip upside down");
#endif

#if CONFIG_SSD1306_128x64
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
  ssd1306_display_text_x3(&dev, 3, "Silv1", 8, false);
  vTaskDelay(3000 / portTICK_PERIOD_MS);

#if CONFIG_SSD1306_128x64
	for(int i =0; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
	top = 2;
	center = 3;
	bottom = 8;
	ssd1306_display_text(&dev, 0, "Rancilio", 9, false);
	ssd1306_display_text(&dev, 1, "v0.1", 4, false);
	ssd1306_display_text(&dev, 3, "Timer:",6, false);
	ssd1306_display_text(&dev, 4, "0.0", 3, false);
	ssd1306_display_text(&dev, 6, "Weight", 6, false);
	ssd1306_display_text(&dev, 7, "0.0", 3, false);
#endif // CONFIG_SSD1306_128x64

	vTaskDelay(15000 / portTICK_PERIOD_MS);
	// while(1){
	// 	char * timer_val:
	// 	char * weight_val;

	// 	timer_val = "0.0"
	// 	weight_val = "0.0"

	// }

	// Fade Out
	ssd1306_fadeout(&dev);
	
	// Restart module
	esp_restart();
}
