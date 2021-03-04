# Changelog

* Unreleased
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
    * **Breaking Change**: `writeWithCrc()` and `readWithCrc()` renamed to
      `writeDataWithCrc()` and `readDataWithCrc()`.
        * New `writeWithCrc(size_t address, const T& data)` and
          `readWithCrc(size_t address, T& data)` are convenience template
          functions which allows the compiler to automatically calculate the
          `sizeof(T)` then call the underlying `writeDataWithCrc()` and
          `readDataWithCrc()`.
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
