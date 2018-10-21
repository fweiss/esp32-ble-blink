#include "BatteryLevel.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <esp_log.h>

// atten_11db reportedly gives 0-3.4
// https://www.esp32.com/viewtopic.php?t=1045

BatteryLevel::BatteryLevel() {
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_11db); // Measure up to 2.2V
}

// Adafruit Huzzah32 has ADC1_7 aka I35 connected to a pair of 100k voltage
// dividers, which would provide (vbat-gnd)/2 to the adc pin
// 0-4095 ~ 0-3.3 V
int BatteryLevel::getCurrentLevel() {
    int battery_voltage = adc1_get_raw(ADC1_CHANNEL_7);
    if (battery_voltage < 0) {
        ESP_LOGE("battery", "Failed to read battery voltage");
        return -1;
    } else {
        ESP_LOGI("battery", "voltage: %d", battery_voltage);
        return battery_voltage;
//        return 2.2*(float)(battery_voltage)/0xFFF;
    }
}
