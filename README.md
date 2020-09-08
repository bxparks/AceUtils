# AceUtils

Arduino utilities which are too small to be in separate libraries, but are used
in multiple projects or other libraries. To avoid duplication, I have collected
them into this library. Most of the following utilities can often be used
independently of other utilities within this library. Some utlities do depend on
actually use another utility within this library. I have tried to document those
dependencies as well as possible.

The following utilities are provided. Click through the hyperlink to see the
documentation for each utility in their respective `README.md` file:

* [PrintString](src/print_string/)
    * An object of a fixed size that implements the `Print` interface so that
      quantities can be printed into it, then extracted as human-readable
      string. It is meant to be an alternative to the Arduino `String` class
      without the risk of heap fragmentation because the entire `PrintString`
      object can be created on the stack.
* [UrlEncoding](src/url_encoding/)
    * Encodes and decodes strings using "form URL encoding" which converts
      spaces `' '` into `'+'`, and non-alphnumerics into percent-hex digits.
* [TimingStats](src/timing_stats/)
    * Helper class to collect data (often durations in milliseconds) and
      then print out various statistics such as min, max, average, and count.
* [PrintUtils](src/print_utils/)
    * Useful utilities on `Print` objects such as `Serial`. Includes
      `printfTo()`, `printPad2To()` to `printPad5To()` functions.

Version: 0.1 (2020-08-27)

Changelog: [CHANGELOG.md](CHANGELOG.md)

## Installation

The latest stable release will be available in the Arduino IDE Library
Manager soon. Search for "AceUtil". Click Install. (Not yet).

The development version can be installed by cloning the
[GitHub repository](https://github.com/bxparks/AceUtils), checking out the
`develop` branch, then manually copying over the contents to the `./libraries`
directory used by the Arduino IDE. (The result is a directory named
`./libraries/AceUtils`.) The `master` branch contains the stable release.

### Source Code

* `src/AceUtil.h`: A bookkeeping header file that includes the version and
  Doxygen docs. Individual utilities have their own header files.
* `src/PrintString.h`: Header file for the PrintString utility.
* `src/UrlEncoding.h`: Header file for the UrlEncoding utility.
* `src/TimingStats.h`: Header file for the TimingStats utility.
* `examples/*`: Example code for various utilities.
* `tests/*`: Unit tests using AUnit.

### Doxygen Docs

The [docs/](docs/) directory contains the
[Doxygen docs](https://bxparks.github.io/AceUtils/html) on GitHub Pages.
This may be useful to navigate the various classes in this library
and to lookup the signatures of the methods in those classes.

## Usage

See the `README.md` for each of the various utilties listed above.

## System Requirements

### Tool Chain

This library was developed and tested using:

* [Arduino IDE 1.8.13](https://www.arduino.cc/en/Main/Software)
* [Arduino AVR Boards 1.6.23](https://github.com/arduino/ArduinoCore-avr)
* [Arduino SAMD Boards 1.8.3](https://github.com/arduino/ArduinoCore-samd)
* [SparkFun AVR Boards 1.1.12](https://github.com/sparkfun/Arduino_Boards)
* [SparkFun SAMD Boards 1.6.2](https://github.com/sparkfun/Arduino_Boards)
* [ESP8266 Arduino 2.7.4](https://github.com/esp8266/Arduino)
* [ESP32 Arduino 1.0.2](https://github.com/espressif/arduino-esp32)
* [Teensydino 1.46](https://www.pjrc.com/teensy/td_download.html)

It should work with [PlatformIO](https://platformio.org/) but I have
not tested it.

The library works on Linux or MacOS (using both g++ and clang++ compilers) using
the [UnixHostDuino](https://github.com/bxparks/UnixHostDuino) emulation layer.

### Operating System

I use Ubuntu 18.04 and 20.04 for the vast majority of my development. I expect
that the library will work fine under MacOS and Windows, but I have not tested
them.

### Hardware

Most utilities will work on various Arduino boards. I test most utilities on
on the following boards:

* Arduino Nano clone (16 MHz ATmega328P)
* SparkFun Pro Micro clone (16 MHz ATmega32U4)
* WeMos D1 Mini clone (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)

Some utlities work only on microcontrollers with built-in WiFi. The header
files for those utilities will contain preprpocessor directives using `#ifdef`
to print out a warning if the board is not one of the following:

* ESP8266
* ESP32

I will occasionally test on the following hardware as a sanity check:

* Teensy 3.2 (72 MHz ARM Cortex-M4)
* Mini Mega 2560 (Arduino Mega 2560 compatible, 16 MHz ATmega2560)
* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+) (compatible with Arduino Zero)

## License

[MIT License](https://opensource.org/licenses/MIT)

## Feedback and Support

If you have any questions, comments, bug reports, or feature requests, please
file a GitHub ticket instead of emailing me unless the content is sensitive.
(The problem with email is that I cannot reference the email conversation when
other people ask similar questions later.) I'd love to hear about how this
software and its documentation can be improved. I can't promise that I will
incorporate everything, but I will give your ideas serious consideration.

## Authors

* Created by Brian T. Park (brian@xparks.net).
