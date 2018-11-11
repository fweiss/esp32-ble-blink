#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <string>
#include <Task.h>

#include "FlashingIndicator.h"
#include "BatteryLevel.h"

#include "sdkconfig.h"

#include <functional>

static char LOG_TAG[] = "SampleServer";

typedef std::function<void(BLECharacteristic*)> CharacteristicCallback;

class MainBLEServer: public Task {

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

//    class HeartRateCallbacks : public BLECharacteristicCallbacks {
//    public:
//        FlashingIndicator* blinker;
//        BatteryLevel* battery;
//
//        HeartRateCallbacks(FlashingIndicator* blinker) {
//            this->blinker = blinker;
//        }
//        virtual void onWrite(BLECharacteristic* characteristic) override {
//            // convert first octet to int, kind of a kludge
//            int value = characteristic->getValue()[0];
//            ESP_LOGI(LOG_TAG, "set heart rate: %d", value);
//            blinker->setBeatsPerMinute(value);
//        }
//    };
//    class BatteryCallbacks : public BLECharacteristicCallbacks {
//    public:
//        BatteryLevel* battery;
//        BatteryCallbacks(BatteryLevel* battery) {
//            this->battery = battery;
//        }
//        virtual void onRead(BLECharacteristic* characteristic) override {
////            int value = battery->getCurrentLevel();
//            uint8_t value = battery->toPercent(battery->getCurrentLevel());
//            characteristic->setValue(&value, 1);
//            ESP_LOGI(LOG_TAG, "battery level: %d", value);
//        }
//    };
    FlashingIndicator* blink;
    BatteryLevel* battery;

    const char* characteristicUUID = "00002A19-0000-1000-8000-00805F9B34FB";
    const uint8_t batteryVoltagePin = 13; // A13
    void run(void *data) {
        ESP_LOGD(LOG_TAG, "Starting BLE work!");

        blink = new FlashingIndicator((gpio_num_t) 13);
        blink->setBeatsPerMinute(120);
        blink->start();

        battery = new BatteryLevel();

        BLEDevice::init("ESP32");
        BLEServer* pServer = BLEDevice::createServer();

        BLEService* pService = pServer->createService("91bad492-b950-4226-aa2b-4ede9fa42f59");

//        BLECharacteristic* pCharacteristic = pService->createCharacteristic(
//            BLEUUID(characteristicUUID),
//            BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
//            BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
//            BLECharacteristic::PROPERTY_INDICATE
//        );
//
//        uint8_t batteryLevel = battery->getCurrentLevel(); //57;
//        pCharacteristic->setValue(&batteryLevel, sizeof(batteryLevel));
//        pCharacteristic->setCallbacks(new BatteryCallbacks(battery));
//
//        BLE2902* p2902Descriptor = new BLE2902();
//        p2902Descriptor->setNotifications(true);
//        pCharacteristic->addDescriptor(p2902Descriptor);

        createBatteryLevelCharacteristic(pService);
        createHeartRateCharacteristic(pService);

        pService->start();

        BLEAdvertising* pAdvertising = pServer->getAdvertising();
        pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
        pAdvertising->start();

        ESP_LOGD(LOG_TAG, "Advertising started!");
        // todo find a better way to suspend here
        delay(1000000);
    }

    void createBatteryLevelCharacteristic(BLEService* pService) {
        BLECharacteristic* pCharacteristic = pService->createCharacteristic(
            BLEUUID(characteristicUUID),
            BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
            BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_INDICATE
        );

        uint8_t batteryLevel = battery->getCurrentLevel(); //57;
        pCharacteristic->setValue(&batteryLevel, sizeof(batteryLevel));
//        pCharacteristic->setCallbacks(new BatteryCallbacks(battery));
        pCharacteristic->setCallbacks(createReadCallbacks(
            [this](BLECharacteristic* characteristic) {
                uint8_t value = battery->toPercent(battery->getCurrentLevel());
                characteristic->setValue(&value, 1);
                ESP_LOGI(LOG_TAG, "battery level: %d", value);
        }));

        BLE2902* p2902Descriptor = new BLE2902();
        p2902Descriptor->setNotifications(true);
        pCharacteristic->addDescriptor(p2902Descriptor);
    }
    void createHeartRateCharacteristic(BLEService* service) {
        const char* characteristicUUID = "00002A92-0000-1000-8000-00805F9B34FB";
        BLECharacteristic* pCharacteristic = service->createCharacteristic(
            BLEUUID(characteristicUUID),
            BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
            BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_INDICATE
        );

//        pCharacteristic->setCallbacks(new HeartRateCallbacks(blink));
        pCharacteristic->setCallbacks(createWriteCallbacks(
            [this](BLECharacteristic* characteristic) {
                int value = characteristic->getValue()[0];
                ESP_LOGI(LOG_TAG, "setting heart rate: %d", value);
                blink->setBeatsPerMinute(value);
        }));

        uint8_t heartRate = 61;
        pCharacteristic->setValue(&heartRate, sizeof(heartRate));
    }
    BLECharacteristicCallbacks* createReadCallbacks(CharacteristicCallback readCallback) {
        return new Ch(readCallback, nullptr);
    }
    BLECharacteristicCallbacks* createWriteCallbacks(CharacteristicCallback readCallback) {
        return new Ch(nullptr, readCallback);
    }
};
