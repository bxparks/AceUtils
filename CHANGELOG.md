# Changelog

* Unreleased
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
