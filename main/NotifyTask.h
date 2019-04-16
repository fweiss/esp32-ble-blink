#pragma once

#include "Task.h"
#include "BLECharacteristic.h"
#include "BatteryLevel.h"

class NotifyTask : public Task {
public:
    NotifyTask(BLECharacteristic *characteristic, BatteryLevel *batteryLevel);
    virtual ~NotifyTask() {}
    void run(void *data) override;
private:
    BLECharacteristic *characteristic;
    BatteryLevel *batteryLevel;

};
