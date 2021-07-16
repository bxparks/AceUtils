# AceUtils

[![AUnit Tests](https://github.com/bxparks/AceUtils/actions/workflows/aunit_tests.yml/badge.svg)](https://github.com/bxparks/AceUtils/actions/workflows/aunit_tests.yml)

This library contains Arduino utilities which are too small or too experimental
to be in separate libraries, but too high-level (with external dependencies) to
be included in the AceCommon (https://github.com/bxparks/AceCommon) library.
There will be several types of code in this library:

1) Shared utilities which are too small to be its own library, but too big to
   copy-and-paste across multiple applications.
2) Shared utilities which have external dependencies so cannot be included in
   AceCommon library which must remain self-contained.
3) Experimental code that seems useful across different Arduino applications,
   but not yet ready to be turned into an independent Arduino library.

**Caution**:

Unlike my other libraries, the API of the code in this library will often change
and evolve over time. When a particular feature becomes more stable and
polished, and if the feature becomes substantial enough, the functionality may
be migrated to a separate independent Arduino library. In that case, the
original code here may become deprecated and removed to avoid the overhead of
maintaining duplicated code.

If you find something useful in this library, I suggest copying the piece of
code instead of depending on this library. This will avoid breakages of your
Arduino application if the API of this library is changed. To reflect the
transient and experimental nature of this library, it is likely that the version
will always remain in the `v0.xx.yy` form.

**Version**: 0.5.0 (2021-03-08)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Features

* CrcEeprom
    * See [src/crc_eeprom/README.md](src/crc_eeprom/README.md).
    * Header files
        * `#include <AceUtils.h>`
        * `#include <crc_eeprom/crc_eeprom.h>`
        * `using ace_utils::crc_eeprom::CrcEepromAvr`
        * `using ace_utils::crc_eeprom::CrcEepromEsp`
    * Summary:
        * Store data structures in EEPROM with a CRC check.
    * Depends on:
        * AceCRC (https://github.com/bxparks/AceCRC)
* Command Line Interface (CLI)
    * [src/cli/README.md](src/cli/README.md)
    * Header files
        * `#include <AceUtils.h>`
        * `#include <cli/cli.h>
        * `using namespace ace_utils::cli`
    * Summary:
        * Implement a command line interface over the Serial port.
    * Depends on:
        * AceRoutine (https://github.com/bxparks/AceRoutine)
        * AceCommon (https://github.com/bxparks/AceCommon)
* ModeGroup
    * Header files
        * `#include <AceUtils.h>`
        * `#include <mode_group/mode_group.h>`
        * `using ace_utils::mode_group::ModeGroup`
        * `using ace_utils::mode_group::ModeNavigator`
    * Summary:
        * A data-driven mechanism to encode and navigate the hierarchy of
          view and change modes of the display of a clock controlled by 2
          buttons.
    * Depends on:
        * (none)
* STM32 Buffered EEPROM
    * Header files
        * `#include <AceUtils.h>`
        * `#include <buffered_eeprom_stm32/buffered_eeprom_stm32.h>
    * A version of `EEPROM` on STM32 that uses a buffer to avoid
      writing to the flash page on every byte update.
        * Implements an API compatible with the `EEPROM` object on ESP8266 and
          ESP32.
        * Creates `BufferedEEPROM` instance in the global namespace, just like
          the `EEPROM` instance.
        * Can be used with `CrcEeprom` through the `CrcEepromEsp` class.
    * API
        * `BufferedEEPROM.begin()`
        * `BufferedEEPROM.write()`, `read()`, `put()`, `get()`, `length()`
        * `BufferedEEPROM.commit()`
* Free memory
    * Header files
        * `#include <AceUtils.h>`
        * `#include <freemem/freemem.h>`
        * `using ace_utils::freemem::freeMemory;`
    * Returns the amount of free memory in the heap.

## Installation

The latest stable release will be available in the Arduino IDE Library
Manager. Search for "AceUtils". Click Install.

The development version can be installed by cloning the
[GitHub repository](https://github.com/bxparks/AceUtils), checking out the
`develop` branch, then manually copying over the contents to the `./libraries`
directory used by the Arduino IDE. (The result is a directory named
`./libraries/AceUtils`.)

The `master` branch contains the stable release. (But see the cautionary note
above about the instability of this library.)

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
* [examples/SimpleCommandLineShell](examples/SimpleCommandLineShell)
    * Demo of the `<cli/cli.h>` classes to implement a command line
      interface that accepts a number of commands on the serial port. In other
      words, it is a primitive "shell". The shell is non-blocking and uses
      coroutines so that other coroutines continue to run.
* [examples/ChannelCommandLineShell](examples/ChannelCommandLineShell)
    * Deprecated version of `SimpleCommandLineShell` using
      `ace_routine::Channels`.

## Usage

The documentation is mostly in the code right now. I will add more as time
allows.

Unlike most of my other libraries, the header files of various submodules are
**not** consolidated into the main `AceUtils.h` header file. The reason is that
many of the submodules are unrelated to each other, and have dependencies on
different external libraries. If I included all the subheaders into `AceUtils.h`
then the end user would be force to install the union of all external libraries
which are needed by the entire collection. Instead, the headers for each
submodule in the various subdirectories must be included explicitly.

For example, to use the `crc_eeprom.h` header, use

```C++
#include <AceUtils.h>
#include <crc_eeprom/crc_eeprom.h>
using namespace ace_utils::crc_eeprom;
```

The first include file (`<AceUtils.h>`) tells the Arduino IDE which library
should be searched (more technically, this determines the `-I` flag of the `g++`
compiler). The second include file (`<crc_eeprom/crc_eeprom.h>`) brings in the
actual header file information of the submodule. All modules are placed in
separate namespaces, which means that a `using namespace ace_utils::crc_eeprom`
is required.

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
* STM32 Blue Pill (STM32F103C8, 72 MHz ARM Cortex-M3)
* WeMos D1 Mini clone (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)

Some utlities work only on microcontrollers with built-in WiFi. The header
files for those utilities will contain preprpocessor directives using `#ifdef`
to print out a warning if the board is not one of the following:

* ESP8266
* ESP32

I will occasionally test on the following hardware as a sanity check:

* Mini Mega 2560 (Arduino Mega 2560 compatible, 16 MHz ATmega2560)
* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+) (compatible with Arduino Zero)
* Teensy 3.2 (72 MHz ARM Cortex-M4)

The following boards are *not* supported (although sometimes, something may
accidentally work on these boards):

* megaAVR (e.g. Nano Every)
* SAMD21 boards w/ `arduino:samd` version >= 1.8.10 (e.g. MKRZero)

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
