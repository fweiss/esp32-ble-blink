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
        // todo switch to vTaskDelayUntil for more accurate timing
        // reluctant to change this without a way to test
        unsigned long offPeriodMillis = 60 * 1000 / beatsPerMinute;
        stateForNextMillis(true, onPeriodMillis);
        stateForNextMillis(false, offPeriodMillis);
    }
}

// turns it on or off for the given period without busy-blocking.
void FlashingIndicator::stateForNextMillis(bool onOff, unsigned long millis) {
    uint32_t level = onOff ? 1 : 0;
    gpio_set_level(pin, level);
    vTaskDelay(millis / portTICK_PERIOD_MS);
}

void FlashingIndicator::setBeatsPerMinute(unsigned int bpm) {
    this->beatsPerMinute = bpm;
}
