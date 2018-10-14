#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <string>
#include <Task.h>

#include "FlashingIndicator.h"

#include "sdkconfig.h"

static char LOG_TAG[] = "SampleServer";


class MainBLEServer: public Task {
    class HeartRateCallbacks : public BLECharacteristicCallbacks {
    public:
        FlashingIndicator* blinker;
        HeartRateCallbacks(FlashingIndicator* blinker) {
            this->blinker = blinker;
        }
        virtual void onWrite(BLECharacteristic* characteristic) override {
    //        const char* value = characteristic->getValue().c_str();
    //        ESP_LOGI(LOG_TAG, "set heart rate: %s", value);

            // convert first octet to int, kind of a kludge
            int value = characteristic->getValue()[0];
            ESP_LOGI(LOG_TAG, "set heart rate: %d", value);
            blinker->setBeatsPerMinute(value);
        }
    };
    FlashingIndicator* blink;

//    const char* characteristicUUID = "0d563a58-196a-48ce-ace2-dfec78acc814";
    const char* characteristicUUID = "00002A19-0000-1000-8000-00805F9B34FB";
    const uint8_t batteryVoltagePin = 13; // A13
    void run(void *data) {
        ESP_LOGD(LOG_TAG, "Starting BLE work!");

        blink = new FlashingIndicator((gpio_num_t) 13);
        blink->setBeatsPerMinute(120);
        blink->start();

        BLEDevice::init("ESP32");
        BLEServer* pServer = BLEDevice::createServer();

        BLEService* pService = pServer->createService("91bad492-b950-4226-aa2b-4ede9fa42f59");

        BLECharacteristic* pCharacteristic = pService->createCharacteristic(
            BLEUUID(characteristicUUID),
            BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
            BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_INDICATE
        );

//        pCharacteristic->setValue("Hello World!");
        uint8_t batteryLevel = 57;
        pCharacteristic->setValue(&batteryLevel, sizeof(batteryLevel));

        BLE2902* p2902Descriptor = new BLE2902();
        p2902Descriptor->setNotifications(true);
        pCharacteristic->addDescriptor(p2902Descriptor);

        createHeartRateCharacteristic(pService);

        pService->start();

        BLEAdvertising* pAdvertising = pServer->getAdvertising();
        pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
        pAdvertising->start();

        ESP_LOGD(LOG_TAG, "Advertising started!");
        delay(1000000);
    }
    void createHeartRateCharacteristic(BLEService* service) {
        const char* characteristicUUID = "00002A92-0000-1000-8000-00805F9B34FB";
        BLECharacteristic* pCharacteristic = service->createCharacteristic(
            BLEUUID(characteristicUUID),
            BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
            BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_INDICATE
        );

        pCharacteristic->setCallbacks(new HeartRateCallbacks(blink));

        uint8_t heartRate = 61;
        pCharacteristic->setValue(&heartRate, sizeof(heartRate));

    }
};
