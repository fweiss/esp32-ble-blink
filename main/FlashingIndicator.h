#pragma once

#include "driver/gpio.h"
#include <Task.h>

class FlashingIndicator : public Task {
public:
    FlashingIndicator(gpio_num_t pin);
    virtual ~FlashingIndicator() {};

    gpio_num_t pin;
    const unsigned long onPeriodMillis = 20;
    unsigned long beatsPerMinute = 80;
    void setBeatsPerMinute(unsigned int bpm);

    virtual void run(void* data) override;

    void logEvent(boolean onOff);
protected:
    void stateForNextMillis(bool onOff, unsigned long millis);
};
