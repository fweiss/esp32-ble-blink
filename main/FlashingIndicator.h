#pragma once

#include "driver/gpio.h"
#include <Task.h>

class FlashingIndicator : public Task {
public:
    FlashingIndicator(gpio_num_t pin);
    virtual ~FlashingIndicator() {};

    gpio_num_t pin;
    const unsigned long onPeriodMillis = 20;
    uint16_t beatsPerMinute = 80;

    void setBeatsPerMinute(uint16_t beatsPerMinute);
    uint16_t getBeatsPerMinute() { return this->beatsPerMinute; }

    virtual void run(void* data) override;

    void logEvent(bool onOff);
protected:
    void stateForNextMillis(bool onOff, unsigned long millis);
private:
    void runWithDelayUntil();
};
