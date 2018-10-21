# Blink Example

Starts a FreeRTOS task to blink an LED

See the README.md file in the upper level 'examples' directory for more information about examples.

## Measuerements
battery level 0x2a19, uit8, 0-100, 
`org.bluetooth.characteristic.battery_level`
0x27AD	percentage	`org.bluetooth.unit.percentage`
0x2728	electric potential difference (volt)	`org.bluetooth.unit.electric_potential_difference.volt`
raw, 11db
uncharged lipo          1719, 0x06b7, 47%
and then it ramps up
partial charge (90 sec) 1911, 0x0777, 119%
full charge             2363, 0x093b, 59%
                        2365, 61%
                        2365, 61%, 4.236 V
                        2339, 35%, 4.201 V
                        2268, 220%, 4.075 V
                        2143, 95%, 3.868 V



## Kolban BLE integration

- clone the repo
- create a component folder
- copy the ccp_utils
- enable bluetooth and bludroid
- rename blink.c to blink.cpp
- add extern "C" to app_main()
- edit CMakeLists.txt
- compile

Library usage

copy from the tests directory

https://www.bluetooth.com/specifications/gatt/characteristics

https://www.oreilly.com/library/view/getting-started-with/9781491900550/ch04.html

