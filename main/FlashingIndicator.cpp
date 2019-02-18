#include "FlashingIndicator.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <string>
#include <stdexcept>
#include "sdkconfig.h"

FlashingIndicator::FlashingIndicator(gpio_num_t pin) {
    this->pin = pin;
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void FlashingIndicator::setBeatsPerMinute(uint16_t beatsPerMinute) {
    unsigned long periodMillis = 60 * 1000 / beatsPerMinute;
    if (periodMillis < onPeriodMillis * 2) {
        throw std::invalid_argument("beats per minute too high for pulse width");
    }
    this->beatsPerMinute = beatsPerMinute;
}

void FlashingIndicator::run(void* data) {
    if (false) {
        while (true) {
            unsigned long offPeriodMillis = 60 * 1000 / beatsPerMinute;
            stateForNextMillis(true, onPeriodMillis);
            stateForNextMillis(false, offPeriodMillis);
        }
    } else {
        runWithDelayUntil();
    }
}

// note that vTaskDelayUntil only provides 10 ms resolution
// using a hardware timer would provide finer resolution and reduce task switching overhead
void FlashingIndicator::runWithDelayUntil() {
    unsigned long basePeriodMillis = 60 * 1000 / beatsPerMinute;
    TickType_t basePeriodWakeTime = xTaskGetTickCount();
    TickType_t onTime = onPeriodMillis / portTICK_PERIOD_MS;
    TickType_t basePeriodTicks = basePeriodMillis / portTICK_PERIOD_MS;
    while (true) {
        gpio_set_level(pin, 1);
        logEvent(true);
        TickType_t onWakeTime = basePeriodWakeTime;
        vTaskDelayUntil(&onWakeTime, onTime);

        gpio_set_level(pin, 0);
        logEvent(false);
        ESP_LOGI("FlashingIndicator", "before %d %d", basePeriodWakeTime, portTICK_PERIOD_MS);
        vTaskDelayUntil(&basePeriodWakeTime, basePeriodTicks);
        ESP_LOGI("FlashingIndicator", "after %d", basePeriodWakeTime);
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
