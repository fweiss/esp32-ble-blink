#include "NotifyTask.h"
#include "BLECharacteristic.h"
#include <esp_log.h>

#define TAG "NotifyTask"

NotifyTask::NotifyTask(BLECharacteristic *characteristic, BatteryLevel *batteryLevel) {
    this->characteristic = characteristic;
    this->batteryLevel = batteryLevel;
}

void NotifyTask::run(void *data) {
    while (1) {
        delay(2000);

        // a lot like read
        uint8_t value = batteryLevel->toPercent(batteryLevel->getCurrentLevel());
        characteristic->setValue(&value, sizeof(value));

        characteristic->notify();
        ESP_LOGI(TAG, "notify battery level sent");
    }
}
