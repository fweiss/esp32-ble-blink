#include "MainBLEServer.h"

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

const char* MainBLEServer::LOG_TAG = "SampleServer";

void MainBLEServer::start() {
    ESP_LOGD(LOG_TAG, "Starting BLE work!");
    const BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");

    try {
        blink = new FlashingIndicator(BLINK_GPIO);
        blink->setBeatsPerMinute(120);
        blink->start();

        battery = new BatteryLevel();

        BLEDevice::init("ESP32");
        BLEServer* server = BLEDevice::createServer();

        BLEService* service = server->createService(serviceUUID);

        createBatteryLevelCharacteristic(service);
        createHeartRateCharacteristic(service);

        service->start();

        BLEAdvertisementData advertisementData;
        advertisementData.setManufacturerData("Prototype by feweiss@gmail.com");
        advertisementData.setName("ESP BLE Blink"); // this works

        BLEAdvertising* advertising = server->getAdvertising();
        advertising->setAdvertisementData(advertisementData);
        advertising->setScanResponseData(advertisementData);
        advertising->addServiceUUID(BLEUUID(service->getUUID()));
        advertising->start();

        ESP_LOGD(LOG_TAG, "Advertising started!");
    } catch (const std::invalid_argument &e) {
        ESP_LOGE(LOG_TAG, "could not start application: %s", e.what());
    }
    ESP_LOGI(LOG_TAG, "main() exiting");
}

void MainBLEServer::createBatteryLevelCharacteristic(BLEService* pService) {
    const uint16_t batteryLevelCharacteristicUUID = 0x2a19;

    BLECharacteristic* characteristic = pService->createCharacteristic(
        BLEUUID(batteryLevelCharacteristicUUID),
        BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
        BLECharacteristic::PROPERTY_NOTIFY    | //BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_INDICATE
    );

//    uint8_t batteryLevel = battery->getCurrentLevel(); //57;
//    characteristic->setValue(&batteryLevel, sizeof(batteryLevel));
    characteristic->setCallbacks(createReadCallbacks(
        [this](BLECharacteristic* characteristic) {
            uint8_t value = battery->toPercent(battery->getCurrentLevel());
            characteristic->setValue(&value, sizeof(value));
            ESP_LOGI(LOG_TAG, "battery level: %d", value);
    }));

    BLE2902* p2902Descriptor = new BLE2902();
    p2902Descriptor->setNotifications(true);
    characteristic->addDescriptor(p2902Descriptor);

    BLEUUID userUUID((uint16_t)0x2901);
    BLEDescriptor* user = new BLEDescriptor(userUUID);
    user->setValue("LiPo battery charge\0"); // note explicit null termination
    characteristic->addDescriptor(user);
}
void MainBLEServer::createHeartRateCharacteristic(BLEService* service) {
    const uint16_t restingHeartRateCharacteristicUUID = 0x2a92;

    BLECharacteristic* characteristic = service->createCharacteristic(
        BLEUUID(restingHeartRateCharacteristicUUID),
        BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
        BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_INDICATE
    );

    characteristic->setCallbacks(createReadWriteCallbacks(
        [this](BLECharacteristic* characteristic) {
            uint16_t value = blink->getBeatsPerMinute();
            characteristic->setValue(value);
            ESP_LOGI(LOG_TAG, "reading heart rate: %d", value);
        },
        [this](BLECharacteristic* characteristic) {
            // todo get 16 bits
            uint16_t value = characteristic->getValue()[0];
            blink->setBeatsPerMinute(value);
            ESP_LOGI(LOG_TAG, "setting heart rate: %d", value);
        }
    ));

//    uint8_t heartRate = 61;
//    characteristic->setValue(&heartRate, sizeof(heartRate));

    // Characteristic Presentation Format
    BLE2904* ble2904 = new BLE2904();
    ble2904->setFormat(BLE2904::FORMAT_UINT8);
    ble2904->setUnit(0x27af); // bpm
    characteristic->addDescriptor(ble2904);
}
