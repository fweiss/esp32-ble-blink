# ESP32 BLE Blink

This project is an example of using Bluetooth with an ESP32 MCU.

- esp-idf toolchain
- Adafruit HUZZAH32 Feather board
- Neil Kolban BLE library
- Nordic nRF Connect

## Kolban BLE integration
Neil Kolban has developed an object-oriented wrapper for the esp-idf Bluetooth API.

https://github.com/nkolban/esp32-snippets

There are several ways to use the library. In this case, the following procedure was used:

- clone the repo
- create a component folder
- copy the ccp_utils
- enable bluetooth and bluedroid
- rename blink.c to blink.cpp
- add extern "C" to app_main()
- edit CMakeLists.txt
- compile

Library usage

copy from the tests directory

https://www.bluetooth.com/specifications/gatt/characteristics

https://www.oreilly.com/library/view/getting-started-with/9781491900550/ch04.html

