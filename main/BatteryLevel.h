#pragma once

#include <esp_gatts_api.h>

class BatteryLevel {
public:
   BatteryLevel();
   virtual ~BatteryLevel() {};

   int getCurrentLevel();
   uint8_t toPercent(int raw);
};
