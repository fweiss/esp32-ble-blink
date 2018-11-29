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

typedef std::function<void(BLECharacteristic*)> CharacteristicCallback;


void MainBLEServer::run(void *data) {
    ESP_LOGD(LOG_TAG, "Starting BLE work!");
    const BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");

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
    // todo find a better way to suspend here
    delay(1000000);
}

void MainBLEServer::createBatteryLevelCharacteristic(BLEService* pService) {
    const char* characteristicUUID = "00002A19-0000-1000-8000-00805F9B34FB";
    BLECharacteristic* characteristic = pService->createCharacteristic(
        BLEUUID(characteristicUUID),
        BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
        BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_INDICATE
    );

    uint8_t batteryLevel = battery->getCurrentLevel(); //57;
    characteristic->setValue(&batteryLevel, sizeof(batteryLevel));
    characteristic->setCallbacks(createReadCallbacks(
        [this](BLECharacteristic* characteristic) {
            uint8_t value = battery->toPercent(battery->getCurrentLevel());
            characteristic->setValue(&value, 1);
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
    const char* characteristicUUID = "00002A92-0000-1000-8000-00805F9B34FB";
    BLECharacteristic* characteristic = service->createCharacteristic(
        BLEUUID(characteristicUUID),
        BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
        BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_INDICATE
    );

    characteristic->setCallbacks(createWriteCallbacks(
        [this](BLECharacteristic* characteristic) {
            int value = characteristic->getValue()[0];
            ESP_LOGI(LOG_TAG, "setting heart rate: %d", value);
            blink->setBeatsPerMinute(value);
    }));

    uint8_t heartRate = 61;
    characteristic->setValue(&heartRate, sizeof(heartRate));

    // Characteristic Presentation Format
    BLE2904* ble2904 = new BLE2904();
    ble2904->setFormat(BLE2904::FORMAT_UINT8);
    ble2904->setUnit(0x27af); // bpm
    characteristic->addDescriptor(ble2904);
}
BLECharacteristicCallbacks* MainBLEServer::createReadCallbacks(CharacteristicCallback readCallback) {
    return new Ch(readCallback, nullptr);
}
BLECharacteristicCallbacks* MainBLEServer::createWriteCallbacks(CharacteristicCallback readCallback) {
    return new Ch(nullptr, readCallback);
}
