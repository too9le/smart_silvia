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
#include <stdio.h>

#include "oled_controller.h"


int configure_oled(void){
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

	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    // commented out no need for a boot menu
    // ssd1306_display_text_x3(&dev, 3, "Boot", 8, false);

    return 1;
}

int setTargetDisplay(){
    for(int i =0; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
    ssd1306_display_text(&dev, 0, "Smart Silvia v1", 18, false);
    ssd1306_display_text(&dev, 2, " Target Weight", 18, false);
    ssd1306_display_text_x3(&dev, 4, "40.00", 5, false);

    return 1;
}

int updateTargetWeight(float weight){
    char buf[10];
    if(weight > 10)
       sprintf(buf, "%.2fg", weight);
    else
       sprintf(buf, "%.3fg", weight);

    ssd1306_display_text_x3(&dev, 4, buf,7, false);

    return 1;

}
int setBrewDisplay(float targetWeight){

    char buf[7];
    sprintf(buf, "%.3fg", targetWeight);


    for(int i =0; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
	ssd1306_display_text(&dev, 0, "Target Weight:", 18, false);
	ssd1306_display_text(&dev, 1, buf, 7, false);
	ssd1306_display_text_x3(&dev, 2, "0.00s",5, false);
	ssd1306_display_text_x3(&dev, 5, "0.00g", 5, false);
    return 1;
}

// this function assumes we are in brew mode
int updateBrewTimer(float time){
    char buf[10];
    if(time > 10)
       sprintf(buf, "%.1fs", time);
    else
       sprintf(buf, "%.2fs", time);

    ssd1306_display_text_x3(&dev, 2, buf,5, false);

    return 1;

}

int updateBrewWeight(float weight){
    char buf[10];
    if(weight > 10)
       sprintf(buf, "%.1fg", weight);
    else
       sprintf(buf, "%.2fg", weight);

    ssd1306_display_text_x3(&dev, 5, buf,5, false);

    return 1;


}

int lowWaterMSG(){
	for(int i =0; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
    ssd1306_display_text(&dev, 0, "Smart Silvia v1", 18, false);
    ssd1306_display_text_x3(&dev, 2, " LOW", 5, false);
    ssd1306_display_text_x3(&dev, 5, "WATER", 5, false);

    return 1;
}

//note this is a blocking call
int blinkLowWaterMSG(int blinkMs){
	for(int i =2; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
	vTaskDelay(blinkMs * 0.25 / portTICK_PERIOD_MS); //wait for a second
	ssd1306_display_text_x3(&dev, 2, " LOW", 5, false);
    ssd1306_display_text_x3(&dev, 5, "WATER", 5, false);
	vTaskDelay(blinkMs * 0.75 / portTICK_PERIOD_MS); //wait for a second

	return 1;
}

int tareScaleMSG(){
    for(int i =0; i < 8; i++){
		ssd1306_clear_line(&dev, i, false);
	}
    ssd1306_display_text(&dev, 0, "Smart Silvia v1", 18, false);
    ssd1306_display_text_x3(&dev, 2, "Tare", 4, false);
    ssd1306_display_text(&dev, 5, "Please Wait", 11, false);
    return 1;
}

int showBrewSummary(int brewNumber, float targetWeight, float weight, float time){
    for(int i =2; i < 8; i++){
       ssd1306_clear_line(&dev, i, false);
	}

    char buf[20];

	ssd1306_display_text(&dev, 0, "Summary:", 8, false);
    sprintf(buf, "Brew Number: %i", brewNumber);
	ssd1306_display_text(&dev, 1, buf, 15, false);

    ssd1306_display_text(&dev, 2, "Target Weight:", 14, false);
    sprintf(buf, "%.3fg", targetWeight);
    ssd1306_display_text(&dev, 3, buf, 7, false);

    ssd1306_display_text(&dev, 4, "End Weight:", 11, false);
    sprintf(buf, "%.3fg", weight);
    ssd1306_display_text(&dev, 5, buf, 7, false);

    ssd1306_display_text(&dev, 6, "Time:", 5, false);
    sprintf(buf, "%.3fs", time);
    ssd1306_display_text(&dev, 7, buf, 8, false);

    return 1;
}