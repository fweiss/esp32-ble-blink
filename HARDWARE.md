# Hardware

This project uses an Adafruit HUZZAH32 feather board.

## Battery level
The HUZZAH can provide the battery voltage with the ESP32's ADC.
The ADC is read from channel 7.

The battery level is provided via the GATT profile:

battery level 0x2a19, uit8, 0-100, 
`org.bluetooth.characteristic.battery_level`
0x27AD	percentage	`org.bluetooth.unit.percentage`
0x2728	electric potential difference (volt)	`org.bluetooth.unit.electric_potential_difference.volt`

## Measurements
In order to calibrate the ADC-to-percent, several measurements were made with
the LiPo battery discharged and fully charged.

The ADC was configured for 11db.

| Raw | Actual | GATT % |
| ---:| ---:| ---:|--- |
| 1719 | | |
| 1761 | 3.281 V | 78% |
| 1840 | 3.385 V | 81% |
| 2143 | 3.868 V | |
| 2268 | 4.075 V | |
| 2339 | 4.201 V | |
| 2365 | 4.236 V | |
