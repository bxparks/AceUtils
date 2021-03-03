# AceUtils

[![AUnit Tests](https://github.com/bxparks/AceUtils/actions/workflows/aunit_tests.yml/badge.svg)](https://github.com/bxparks/AceUtils/actions/workflows/aunit_tests.yml)

This library contains Arduino utilities which are too small to be in separate
libraries, but too high-level (with external dependencies) to be included in the
AceCommon (https://github.com/bxparks/AceCommon) library. The AceCommon library
must remain self-contained without any external dependencies. The utilities in
AceUtils will generally have dependencies to my other libraries (e.g. AceCommon,
AceTime, AceRoutine, etc) or third party libraries.

* CrcEeprom
    * `#include <AceUtilsCrcEeprom.h>`
    * `using ace_utils::crc_eeprom::CrcEeprom`
    * Summary:
        * Store data structures in EEPROM with a CRC check.
    * Depends on:
        * AceCRC (https://github.com/bxparks/AceCRC)
* Command Line Interface (CLI)
    * [src/cli/README.md](src/cli/README.md)
    * `#include <AceUtilsCli.h>`
    * `using namespace ace_utils::cli`
    * Summary:
        * Implement a command line interface over the Serial port.
    * Depends on:
        * AceRoutine (https://github.com/bxparks/AceRoutine)
        * AceCommon (https://github.com/bxparks/AceCommon)
* ModeGroup
    * `#include <AceUtilsModeGroup.h>`
    * `using ace_utils::mode_group::ModeGroup`
    * `using ace_utils::mode_group::ModeNavigator`
    * Summary:
        * A data-driven mechanism to encode and navigate the hierarchy of
          view and change modes of the display of a clock controlled by 2
          buttons.
    * Depends on:
        * (none)

**Version**: 0.4.1 (2021-01-22)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Installation

The latest stable release will be available in the Arduino IDE Library
Manager. Search for "AceUtils". Click Install.

The development version can be installed by cloning the
[GitHub repository](https://github.com/bxparks/AceUtils), checking out the
`develop` branch, then manually copying over the contents to the `./libraries`
directory used by the Arduino IDE. (The result is a directory named
`./libraries/AceUtils`.) The `master` branch contains the stable release.

### Source Code

* `src/AceUtil.h`: A bookkeeping header file that includes the version and
  Doxygen docs. Individual utilities have their own header files.
* `src/*.h`: Header file for each utility listed above.
* `examples/*`: Example code for various utilities.
* `tests/*`: Unit tests using AUnit.

### Doxygen Docs

The [docs/](docs/) directory contains the
[Doxygen docs](https://bxparks.github.io/AceUtils/html) on GitHub Pages.
This may be useful to navigate the various classes in this library
and to lookup the signatures of the methods in those classes.

### Examples

* [examples/CrcEepromDemo](examples/CrcEepromDemo)
    * Demo of `CrcEeprom` class.
* [examples/CommandLineShell](examples/CommandLineShell)
    * Demo of the `<CommandLineInterface.h>` classes to implement a command line
      interface that accepts a number of commands on the serial port. In other
      words, it is a primitive "shell". The shell is non-blocking and uses
      coroutines so that other coroutines continue to run.

## Usage

The documentation is mostly in the code right now. I will add more as time
allows.

Unlike most of my other libraries, the header files of various submodules are
**not** consolidated into the main `AceUtils.h` header file. The reason is that
many of the submodules are unrelated to each other, and have dependencies on
different external libraries. If I included all the subheaders into `AceUtils.h`
then the end user would be force to install the union of all external libraries
which are needed by the entire collection. By splitting each module into
separate headers (e.g. `AceUtilsCrcEeprom.h`, `AceUtilsModeGroup.h`), the
external dependencies are minimized.

The header file of all submodules will begin with the prefix `AceUtils{xxx}.h`,
for example, `AceUtilsModeGroup.h`.

All modules are placed in separate namespaces, under the `ace_utils::` top-level
namespace.

## System Requirements

### Tool Chain

This library was developed and tested using:

* [Arduino IDE 1.8.13](https://www.arduino.cc/en/Main/Software)
* [Arduino AVR Boards 1.6.23](https://github.com/arduino/ArduinoCore-avr)
* [Arduino SAMD Boards 1.8.3](https://github.com/arduino/ArduinoCore-samd)
* [SparkFun AVR Boards 1.1.12](https://github.com/sparkfun/Arduino_Boards)
* [SparkFun SAMD Boards 1.6.2](https://github.com/sparkfun/Arduino_Boards)
* [ESP8266 Arduino 2.7.4](https://github.com/esp8266/Arduino)
* [ESP32 Arduino 1.0.4](https://github.com/espressif/arduino-esp32)
* [Teensydino 1.46](https://www.pjrc.com/teensy/td_download.html)

It should work with [PlatformIO](https://platformio.org/) but I have
not tested it.

Many utilities work on Linux or MacOS (using both g++ and clang++ compilers)
using the [EpoxyDuino](https://github.com/bxparks/EpoxyDuino) emulation
layer.

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
