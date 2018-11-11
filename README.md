# ESP32 BLE Blink

This project is an example of using Bluetooth with an ESP32 MCU.

- esp-idf toolchain
- Adafruit HUZZAH32 Feather board
- Neil Kolban BLE library
- Nordic nRF Connect client

## Concept
Communicate with the ESP32 MCU via Bluetooth. 
Keep it simple with the following MVP:

- adjust the blink of the onboard user LED
- read the charge level of the attached LiPo battery
- try to use existing GATT profiles
- use the Nordic nRF Connect Bluetooth client for Android

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

## BLE clients
Besides writing a custom client, there are many generic clients for testing a BLE server. They are available for either desktop or mobile.

- Nordic Semiconductor nRF Connect
- Adafruit Bluefruit LE Connect
- Lightblue
- Bluesee
- XCode Bluetooth Explorer

Also XCode

The ESP32 did not show up i scan until I disconnected from the nrf Connect client on Android.

### BlueSee
Available from the Apple App Store. It's free.

Mac desktop version:

- Scans for BT devices
- Shows device info
- Shows device connection log
- write characteristic only hex or ascii
- not sure if battery level updates are working

## LightBlue
Available from the Apple App Store. It's free.

Mac desktop version 1.2:

- Scans for BT devices
- doesn't show much device info besides name
- show device connect log
- swimlane navigation for devices, services, characteristics, details
- not showing characteristics

This app did not show the characteristics of this BT project on ESP32.

## nRF Connect
For Mac, available as DMG download. It's free.

Mac desktop version 2.6.0:

It would not work. Reported an error finding `JLINKARM_DLL_NOT_FOUND`. The log shows it was trying to get a device serial number via the USB serial ports, including /dev/tty.Bluetooth-Incoming-Port. Perhaps it's necessary to connect the device first. However, the Mac bluetooth does not sow the ESP32 in its scan. Since other Mac desktop apps did find the ESP32, this doesn't seem to be an underlying OS X problem.

This is very disappointing since the Android version works quite well. Perhaps it's the wrong app, since it seems to be about USB instead of BT. This may well be the case. It looks like it needs a dongle for BT connectivity instead of the Mac's built in BT stack.

Android version 4.21.0:

- scans for devices
- show name and RF power
- shows generic attribute, generic access, and service
- tree navigation
- shows characteristics and info
- read and write characteristic
- write dialog permits many data types, including float and text

## XCode Bluetooth Explorer
Additional Tools for XCode 10, seperate download at https://developer.apple.com/downloads/.
 
Devices > Show low energy devices.

- scans for devices
- connects to device
- shows service and characteristics
- only shows UUIDs
- read or write characteristic
 
## Notes and references
It's only possible to connect one client at a time.
