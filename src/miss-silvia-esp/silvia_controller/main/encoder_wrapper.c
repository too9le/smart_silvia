#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rotary_encoder.h"
#include "button.h"
#include "esp_log.h"


#define TAG "app"


#define ROT_ENC_A_GPIO 9
#define ROT_ENC_B_GPIO 10
#define ROT_BUTTON 13

#define ENABLE_HALF_STEPS false  // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT          0      // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION    false  // Set to true to reverse the clockwise/counterclockwise sense

QueueHandle_t event_queue;
rotary_encoder_info_t info = { 0 };

button_event_t ev;
QueueHandle_t button_events;

int rotery_init(){

    // esp32-rotary-encoder requires that the GPIO ISR service is installed before calling rotary_encoder_register()
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Initialise the rotary encoder device with the GPIOs for A and B signals
	ESP_LOGI(TAG, "Time To setup the Rotery encoder");

    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
	ESP_LOGI(TAG, "Encoder Set Up");

    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif

    ESP_LOGI(TAG, "Encoder Button Set Up");

    button_events = button_init(PIN_BIT(ROT_BUTTON));

    ESP_LOGI(TAG, "Encoder Button Set Up DONE");

    // Create a queue for events from the rotary encoder driver.
    // Tasks can read from this queue to receive up to date position information.
    event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));

	// -----------------------------------
    return 1;
}

int getButtonPress(){
    int buttonState = 0;
    if (xQueueReceive(button_events, &ev, 100/portTICK_PERIOD_MS)) {
        if ((ev.pin == ROT_BUTTON) && (ev.event == BUTTON_DOWN)) {
            buttonState = 1;
        }
    }
    return buttonState;
}


float getRoteryPossition(){
		// Wait for incoming events on the event queue.
        rotary_encoder_event_t event = { 0 };
        int pos = 0;
        if (xQueueReceive(event_queue, &event, 100 / portTICK_PERIOD_MS) == pdTRUE)
        {
            // ESP_LOGI(TAG, "Event: position %d, direction %s", event.state.position,
            //          event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
            pos = event.state.position;
        }
        else
        {
            // Poll current position and direction
            rotary_encoder_state_t state = { 0 };
            ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));
            // ESP_LOGI(TAG, "Poll: position %d, direction %s", state.position,
            //          state.direction ? (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
            pos = state.position;
        }
        return 40 + pos * 0.25;
    }