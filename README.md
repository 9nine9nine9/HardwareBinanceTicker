# HardwareBinanceTicker
Simple STM32 OLED hardware monitor for cryptocurrencies using the Binance API.

By default it tracks ADA/BUSD. It shows the current price and the percentage change over the last 24 hours. It doesn't need an API key, so if you have the hardware you can just set the WiFi details and upload it immediately!

## Requirements
Requires the Arduino IDE to build (or an arduino makefile if you're brave).

Hardware components:
- Heltec STM32 WiFi Kit 32 (~£15)

[Their library](https://github.com/HelTecAutomation/Heltec_ESP32) is basically a wrapper around the Adafruit OLED library, so it could be (with relative effort) ported to other OLEDs for more standard STM32 boards.

Also jsonlib (can be found through the Arduino IDE)

## Future Improvements
- I'd like to port this over to the standard STM32 libraries (which is possible since the wireless library is already standard, and the OLED is connected through I2C already). This would give access to better docs and easier maintainability since it's community managed.

- It'd be nice to be able to display the ADA logo, but I couldn't get it to convert nicely :(

- Since the board supports a battery, it'd be nice to have a little display of the estimated remaining charge in the corner or something.
