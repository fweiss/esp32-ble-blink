#pragma once

class BatteryLevel {
public:
   BatteryLevel();
   virtual ~BatteryLevel() {};
   int getCurrentLevel();
};
