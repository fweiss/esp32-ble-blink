#include "FlashingIndicator.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <string>
#include "sdkconfig.h"

FlashingIndicator::FlashingIndicator(gpio_num_t pin) {
    this->pin = pin;
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void FlashingIndicator::run(void* data) {
    if (false) {
        while (true) {
            unsigned long offPeriodMillis = 60 * 1000 / beatsPerMinute;
            stateForNextMillis(true, onPeriodMillis);
            stateForNextMillis(false, offPeriodMillis);
        }
    } else {
        unsigned long offPeriodMillis = 60 * 1000 / beatsPerMinute;
        TickType_t basePeriodWakeTime = xTaskGetTickCount();
        TickType_t onTime = onPeriodMillis / portTICK_PERIOD_MS;
        TickType_t offTime= offPeriodMillis / portTICK_PERIOD_MS;
        while (true) {
            gpio_set_level(pin, 1);
            logEvent(true);
            TickType_t onWakeTime = basePeriodWakeTime;
            vTaskDelayUntil(&onWakeTime, onTime);

            gpio_set_level(pin, 0);
            logEvent(false);
            vTaskDelayUntil(&basePeriodWakeTime, onTime + offTime);
        }
    }
}

// turns it on or off for the given period without busy-blocking.
// todo switch to vTaskDelayUntil for more accurate timing
// reluctant to change this without a way to test
void FlashingIndicator::stateForNextMillis(bool onOff, unsigned long millis) {
    logEvent(onOff);
    uint32_t level = onOff ? 1 : 0;
    gpio_set_level(pin, level);
    vTaskDelay(millis / portTICK_PERIOD_MS);
}

void FlashingIndicator::logEvent(bool onOff) {
    if (onOff) {
        ESP_LOGI("FlashingIndicator", "ON");
    } else {
        ESP_LOGI("FlashingIndicator", "OFF");
    }
}
