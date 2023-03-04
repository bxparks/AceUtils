# Changelog

* Unreleased
* 0.6.0 (2023-03-04)
    * `AceUtils/buffered_eeprom_stm32`
        * Rename `AceUtilsStm32BufferedEeprom.h` to
          `buffered_eeprom_stm32/buffered_eeprom_stm32.h`
    * `AceUtils/crc_eeprom`
        * `CrcEeprom`: Recommend using a 32-bit random number for the
          `contextId` instead of using `toContextId()` helper function.
        * Rename `IEepromAdapter` to `EepromInterface`; `EspEepromAdapter` to
          `EspStyleEeprom`; `AvrEepromAdapter` to `AvrStyleEeprom`.
        * Rename `EpoxyPromAvr` to `EpoxyEepromAvr`; `EpoxyPromEsp` to
          `EpoxyEepromEsp`; tracks the changes in EpoxyDuino library.
        * Convert `CrcEeprom` into a template class, templatized on
          `EspStyleEeprom` and `AvrStyleEeprom`. Seems to save between 150-950
          bytes of flash memory by avoiding virtual function calls and class
          hierarchies.
    * `AceUtils/freemem`
        * Move `freeMemory()` function from `examples/CommandLineShell.ino`
          into `freemem/freemem.h` so that it can be used elsewhere.
        * Add support for STM32, and clean up header imports.
    * `AceUtils/cli`
        * Move `AceUtilsCli.h` to `cli/cli.h`.
        * Split `CommandDispatcher` from `ChannelDispatcher` so that it can be
          used without using an AceRoutine `Channel`.
        * Rename `CommandManager` to `ChannelProcessorManager`.
        * Rename `ChannelDispatcher` to `ChannelProcessorCoroutine`.
        * Rename `StreamLineReader` to `StreamReaderCoroutine`.
        * Add `StreamProcessorCoroutine` and `StreamProcessorManager`.
        * Add `DirectProcessor` and `DirectProcessorManager`.
        * Split `examples/CommandLineShell.ino` into 3 versions:
            * `ChannelCommandLineShell`
            * `StreamCommandLineShell`
            * `DirectCommandLineShell`
    * `AceUtils/mode_group`
        * Add `ModeRecord` to keep the modeId and its `ModeGroup*` pointer
          together in a single data structure.
        * Add some notes to [mode_group/README.md](src/mode_group/README.md)
          because I had a hard time understanding my own code.
        * **Deprecated**: This code is deprecated before it is released. See
          notes at the top of [src/mode_group](src/mode_group/README.md). I'll
          keep it around for historical reference.
* 0.5.0 (2021-03-08)
    * Add unit tests for `CrcEeprom` using `EpoxyPromAvr` and `EpoxyPromEsp`
      libraries from EpoxyDuino.
    * Change default CRC32 calculator to `crc32_nibblem` for the ESP8266
      only, since it is 2.7X faster than `crc32_nibble` on the ESP8266 platform.
    * Add an optional `contextId` to `CrcEeprom()` constructor. This is an
      application-provided identifier that is stored into EEPROM along with the
      data and its CRC. When the data is read by `CrcEeprom::readWithCrc()`, the
      `contextId` must match. This prevents data from a different application
      that happen to have the same length from being marked as valid.
    * Add `CrcEeprom::toContextId()` helper to generate the `contextId`.
    * Add `CrcEeprom::toSavedSize()` to calculate the minimum size to pass to
      `CrcEeprom::begin()`.
    * Add `BufferedEEPROM` global object for STM32 which implements the
      ESP-flavored API, and uses the "buffered" versions of the low-level EEPROM
      functions.
        * Accessed through `#include <AceUtilsStm32BufferedEeprom.h>`.
        * The default `EEPROM` is unbuffered, which means every `EEPROM.write()`
          operation causes the entire flash page to be wiped and rewritten.
    * Add `EspEepromAdapter` and `AvrEepromAdapter` template classes,
      implementing `IEepromAdapter` interface. These smooth over the API
      differences between the AVR-flavored EEPROM API and the ESP-flavored
      EEPROM API and allow the `CrcEeprom` class to support both API flavors.
    * **Breaking Change**: The constructor of `CrcEeprom` now accepts an
      instance of `IEepromAdapter`, instead of using the global `EEPROM`
      object directly.
        * Creating the `BufferedEEPROM` object for the STM32 meant that there
          can be 2 `EEPROM` objects active concurrently. So the `CrcEeprom`
          needs to be told which one to use.
        * Add `contextId` to the `CrcEeprom` constructor, defaulting to 0x00.
    * **Breaking Change**: Rename `CrcEeprom::writeWithCrc()` and
      `readWithCrc()` to `writeDataWithCrc()` and `readDataWithCrc()`.
        * Replace them with signatures which are easier to use:
            * `writeWithCrc(size_t address, const T& data)`
            * `readWithCrc(size_t address, T& data)`.
        * The compiler compiler automatically figures out the `sizeof(T)` then
          calls the underlying `writeDataWithCrc()` and `readDataWithCrc()`.
* 0.4.1 (2021-01-22)
    * Convert `SHIFT_ARGC_ARGV()` macro in `src/cli/CommandHandler.h` to
      an inlined static function `CommandHandler::shiftArgcArgv()` using
      references. I forgot that I was programming in C++ when I created the
      macro. Add `SHIFT_ARGC_ARGV()` function for backwards compatibility.
    * Update UnixHostDuino 0.4 to EpoxyDuino 0.5.
    * No functional change in this release.
* 0.4 (2020-12-09)
    * update `AceUtilsCrcEeprom.h`
        * Replace FastCRC with AceCRC library for a reduction of 4-5 kB of flash
          memory. Allow alternate CRC calculators to be passed in as a
          constructor argument to `CrcEeprom()`.
        * Rename `CrcEeprom.h` to `AceUtilsCrcEeprom.h`, and move namespace to
          `ace_utils::crc_eeprom`::CrcEeprom`.
    * create `AceUtilsModeGroup.h`
        * `ace_utils::mode_group::ModeGroup`
        * `ace_utils::mode_group::GroupNavigator`
        * a data-driven mechanism to encode and navigate the hierarchy of view
          and change modes of the display of a clock controlled by 2 buttons.
    * rename `CommandLineInterface.h` to `AceUtilsCli.h`
        * move `cli/*` classes into `ace_utils::cli::` namespace
* 0.3 (2020-11-01)
    * Move `CrcEeprom` class from AceTime library.
    * Move `CommandLineInterface` package from AceRoutine library.
    * Move all previous utlities (with no external dependencies) into the
      AceCommon library.
