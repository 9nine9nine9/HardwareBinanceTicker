# HardwareBinanceTracker
Simple STM32 OLED hardware monitor for cryptocurrencies using the Binance API

## Requirements
Requires the Arduino IDE to build (or an arduino makefile if you're brave).

Hardware components:
o Heltec STM32 WiFi Kit 32 (~£15)

[Their library](https://github.com/HelTecAutomation/Heltec_ESP32) is basically a wrapper around the Adafruit OLED library, so it could be (with relative effort) ported to other OLEDs for more standard STM32 boards.

## Future Improvements
I'd like to port this over to the standard STM32 libraries (which is possible since the wireless library is already standard, and the OLED is connected through I2C already). This would give access to better docs and easier maintainability since it's community managed.
