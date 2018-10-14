#include "FlashingIndicator.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

FlashingIndicator::FlashingIndicator(gpio_num_t pin) {
    this->pin = pin;
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void FlashingIndicator::run(void* data) {
    while (true) {
        gpio_set_level(pin, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(pin, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
