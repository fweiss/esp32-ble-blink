#pragma once

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "BLE2904.h"
#include <esp_log.h>
#include <string>
#include <Task.h>

#include "FlashingIndicator.h"
#include "BatteryLevel.h"

#include "sdkconfig.h"

#include <functional>

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO ((gpio_num_t)CONFIG_BLINK_GPIO)

class MainBLEServer: public Task {
public:
    static const char* LOG_TAG;

    FlashingIndicator* blink;
    BatteryLevel* battery;

    void run(void *data);
    void createBatteryLevelCharacteristic(BLEService* pService);

    void createHeartRateCharacteristic(BLEService* service);
};
