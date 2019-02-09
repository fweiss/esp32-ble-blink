#pragma once

#include <functional>

typedef std::function<void(BLECharacteristic*)> CharacteristicCallback;

// helper class extension to support lambda callbacks
class BLECharacteristicCallbacksHelper : public BLECharacteristicCallbacks {
public:
    BLECharacteristicCallbacksHelper(CharacteristicCallback readCallback, CharacteristicCallback writeCallback) {
        this->readCallback = readCallback ? readCallback : nullCallback;
        this->writeCallback = writeCallback ? writeCallback : nullCallback;
    }
    ~BLECharacteristicCallbacksHelper() {}
    virtual void onWrite(BLECharacteristic* characteristic) {
        writeCallback(characteristic);
    }
    virtual void onRead(BLECharacteristic* characteristic) {
        readCallback(characteristic);
    }
    static BLECharacteristicCallbacks* createReadCallbacks(CharacteristicCallback readCallback) {
        return new BLECharacteristicCallbacksHelper(readCallback, nullptr);
    }
    static BLECharacteristicCallbacks* createWriteCallbacks(CharacteristicCallback readCallback) {
        return new BLECharacteristicCallbacksHelper(nullptr, readCallback);
    }

private:
    CharacteristicCallback readCallback;
    CharacteristicCallback writeCallback;
    const CharacteristicCallback nullCallback = [](BLECharacteristic*){};
};
