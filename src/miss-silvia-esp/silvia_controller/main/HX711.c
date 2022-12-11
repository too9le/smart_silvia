// imported from https://github.com/akshayvernekar/HX711_IDF/blob/master/main/HX711.c

#include "HX711.h"
#include "esp_log.h"
#include <rom/ets_sys.h>

#define HIGH 1
#define LOW 0
#define CLOCK_DELAY_US 20

#define DEBUGTAG "HX711"

static gpio_num_t GPIO_PD_SCK_1 = 16;	// Power Down and Serial Clock Input Pin
static gpio_num_t GPIO_DOUT_1 = 17;		// Serial Data Output Pin

static HX711_GAIN GAIN_1 = eGAIN_128;		// amplification factor
static unsigned long OFFSET_1 = 0;	// used for tare weight

static gpio_num_t GPIO_PD_SCK_2 = 16;	// Power Down and Serial Clock Input Pin
static gpio_num_t GPIO_DOUT_2 = 17;		// Serial Data Output Pin

static HX711_GAIN GAIN_2 = eGAIN_128;		// amplification factor
static unsigned long OFFSET_2 = 0;	// used for tare weight

static float SCALE = 1000;	// used to return weight in grams, kg, ounces, whatever

void HX711_init(gpio_num_t dout, gpio_num_t pd_sck, HX711_GAIN gain )
{
	GPIO_PD_SCK_1 = pd_sck;
	GPIO_DOUT_1 = dout;

	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1UL<<GPIO_PD_SCK_1);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1UL<<GPIO_DOUT_1);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
	gpio_config(&io_conf);

	HX711_set_gain(gain);
}

bool HX711_is_ready()
{
	return gpio_get_level(GPIO_DOUT_1);
}

void HX711_set_gain(HX711_GAIN gain)
{
	GAIN_1 = gain;
	gpio_set_level(GPIO_PD_SCK_1, LOW);
	HX711_read();
}

uint8_t HX711_shiftIn()
{
	uint8_t value = 0;

    for(int i = 0; i < 8; ++i) 
    {
        gpio_set_level(GPIO_PD_SCK_1, HIGH);
        ets_delay_us(CLOCK_DELAY_US);
        value |= gpio_get_level(GPIO_DOUT_1) << (7 - i); //get level returns 
        gpio_set_level(GPIO_PD_SCK_1, LOW);
        ets_delay_us(CLOCK_DELAY_US);
    }

    return value;
}

unsigned long HX711_read()
{
	gpio_set_level(GPIO_PD_SCK_1, LOW);
	// wait for the chip to become ready
	while (HX711_is_ready()) 
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	unsigned long value = 0;

	//--- Enter critical section ----
	portDISABLE_INTERRUPTS();

	for(int i=0; i < 24 ; i++)
	{   
		gpio_set_level(GPIO_PD_SCK_1, HIGH);
        ets_delay_us(CLOCK_DELAY_US);
        value = value << 1;
        gpio_set_level(GPIO_PD_SCK_1, LOW);
        ets_delay_us(CLOCK_DELAY_US);

        if(gpio_get_level(GPIO_DOUT_1))
        	value++;
	}

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN_1; i++) 
	{	
		gpio_set_level(GPIO_PD_SCK_1, HIGH);
		ets_delay_us(CLOCK_DELAY_US);
		gpio_set_level(GPIO_PD_SCK_1, LOW);
		ets_delay_us(CLOCK_DELAY_US);
	}	
	portENABLE_INTERRUPTS();
	//--- Exit critical section ----

	value =value^0x800000;

	return (value);
}



unsigned long  HX711_read_average(char times) 
{
	ESP_LOGI(DEBUGTAG, "===================== READ AVERAGE START ====================");
	unsigned long sum = 0;
	for (char i = 0; i < times; i++) 
	{
		sum += HX711_read();
	}
	ESP_LOGI(DEBUGTAG, "===================== READ AVERAGE END : %ld ====================",(sum / times));
	return sum / times;
}

unsigned long HX711_get_value(char times) 
{
	unsigned long avg = HX711_read_average(times);
	if(avg > OFFSET_1)
		return avg - OFFSET_1;
	else
		return 0;
}

float HX711_get_units(char times) 
{
	return HX711_get_value(times) / SCALE;
}

void HX711_tare( ) 
{
	ESP_LOGI(DEBUGTAG, "===================== START TARE ====================");
	unsigned long sum = 0; 
	sum = HX711_read_average(20);
	HX711_set_offset(sum);
	ESP_LOGI(DEBUGTAG, "===================== END TARE: %ld ====================",sum);
}

void HX711_set_scale(float scale ) 
{
	SCALE = scale;
}

float HX711_get_scale()
 {
	return SCALE;
}

void HX711_set_offset(unsigned long offset)
 {
	OFFSET_1 = offset;
}

unsigned long HX711_get_offset(unsigned long offset) 
{
	return OFFSET_1;
}

void HX711_power_down() 
{
	gpio_set_level(GPIO_PD_SCK_1, LOW);
	ets_delay_us(CLOCK_DELAY_US);
	gpio_set_level(GPIO_PD_SCK_1, HIGH);
	ets_delay_us(CLOCK_DELAY_US);
}

void HX711_power_up() 
{
	gpio_set_level(GPIO_PD_SCK_1, LOW);
}



// secondary sensor this is a hacky approach but works for now:

void HX711_init_2(gpio_num_t dout, gpio_num_t pd_sck, HX711_GAIN gain )
{
	GPIO_PD_SCK_2 = pd_sck;
	GPIO_DOUT_2 = dout;

	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1UL<<GPIO_PD_SCK_2);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1UL<<GPIO_DOUT_2);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
	gpio_config(&io_conf);

	HX711_set_gain(gain);
}

bool HX711_is_ready_2()
{
	return gpio_get_level(GPIO_DOUT_2);
}

void HX711_set_gain_2(HX711_GAIN gain)
{
	GAIN_2 = gain;
	gpio_set_level(GPIO_PD_SCK_2, LOW);
	HX711_read_2();
}

uint8_t HX711_shiftIn_2()
{
	uint8_t value = 0;

    for(int i = 0; i < 8; ++i) 
    {
        gpio_set_level(GPIO_PD_SCK_2, HIGH);
        ets_delay_us(CLOCK_DELAY_US);
        value |= gpio_get_level(GPIO_DOUT_2) << (7 - i); //get level returns 
        gpio_set_level(GPIO_PD_SCK_2, LOW);
        ets_delay_us(CLOCK_DELAY_US);
    }

    return value;
}

unsigned long HX711_read_2()
{
	gpio_set_level(GPIO_PD_SCK_2, LOW);
	// wait for the chip to become ready
	while (HX711_is_ready_2()) 
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	unsigned long value = 0;

	//--- Enter critical section ----
	portDISABLE_INTERRUPTS();

	for(int i=0; i < 24 ; i++)
	{   
		gpio_set_level(GPIO_PD_SCK_2, HIGH);
        ets_delay_us(CLOCK_DELAY_US);
        value = value << 1;
        gpio_set_level(GPIO_PD_SCK_2, LOW);
        ets_delay_us(CLOCK_DELAY_US);

        if(gpio_get_level(GPIO_DOUT_2))
        	value++;
	}

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN_2; i++) 
	{	
		gpio_set_level(GPIO_PD_SCK_2, HIGH);
		ets_delay_us(CLOCK_DELAY_US);
		gpio_set_level(GPIO_PD_SCK_2, LOW);
		ets_delay_us(CLOCK_DELAY_US);
	}	
	portENABLE_INTERRUPTS();
	//--- Exit critical section ----

	value =value^0x800000;

	return (value);
}



unsigned long  HX711_read_average_2(char times) 
{
	ESP_LOGI(DEBUGTAG, "===================== READ AVERAGE START ====================");
	unsigned long sum = 0;
	for (char i = 0; i < times; i++) 
	{
		sum += HX711_read_2();
	}
	ESP_LOGI(DEBUGTAG, "===================== READ AVERAGE END : %ld ====================",(sum / times));
	return sum / times;
}

unsigned long HX711_get_value_2(char times) 
{
	unsigned long avg = HX711_read_average_2(times);
	if(avg > OFFSET_2)
		return avg - OFFSET_2;
	else
		return 0;
}

float HX711_get_units_2(char times) 
{
	return HX711_get_value_2(times) / SCALE;
}

void HX711_tare_2( ) 
{
	ESP_LOGI(DEBUGTAG, "===================== START TARE ====================");
	unsigned long sum = 0; 
	sum = HX711_read_average_2(20);
	HX711_set_offset_2(sum);
	ESP_LOGI(DEBUGTAG, "===================== END TARE: %ld ====================",sum);
}

void HX711_set_offset_2(unsigned long offset)
 {
	OFFSET_2 = offset;
}

unsigned long HX711_get_offset_2(unsigned long offset) 
{
	return OFFSET_2;
}

void HX711_power_down_2() 
{
	gpio_set_level(GPIO_PD_SCK_2, LOW);
	ets_delay_us(CLOCK_DELAY_US);
	gpio_set_level(GPIO_PD_SCK_2, HIGH);
	ets_delay_us(CLOCK_DELAY_US);
}

void HX711_power_up_2() 
{
	gpio_set_level(GPIO_PD_SCK_2, LOW);
}