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

typedef std::function<void(BLECharacteristic*)> CharacteristicCallback;

class MainBLEServer: public Task {
public:
    static const char* LOG_TAG;

    // helper class extension to support lambda callbacks
    class Ch : public BLECharacteristicCallbacks {
    public:
        Ch(CharacteristicCallback readCallback, CharacteristicCallback writeCallback) {
            this->readCallback = readCallback ? readCallback : nullCallback;
            this->writeCallback = writeCallback ? writeCallback : nullCallback;
        }
        ~Ch() {}
        virtual void onWrite(BLECharacteristic* characteristic) {
            writeCallback(characteristic);
        }
        virtual void onRead(BLECharacteristic* characteristic) {
            readCallback(characteristic);
        }
    private:
        CharacteristicCallback readCallback;
        CharacteristicCallback writeCallback;
        const CharacteristicCallback nullCallback = [](BLECharacteristic*){};
    };

    FlashingIndicator* blink;
    BatteryLevel* battery;


    void run(void *data);
    void createBatteryLevelCharacteristic(BLEService* pService);

    void createHeartRateCharacteristic(BLEService* service);
    BLECharacteristicCallbacks* createReadCallbacks(CharacteristicCallback readCallback);
    BLECharacteristicCallbacks* createWriteCallbacks(CharacteristicCallback readCallback);
};
