# Changelog

* Unreleased
    * CrcEeprom: Replace FastCRC with AceCRC library for a reduction of 4-5 kB
      of flash memory. Allow alternate CRC calculators to be passed in as a
      constructor argument to `CrcEeprom()`.
* 0.3 (2020-11-01)
    * Move `CrcEeprom` class from AceTime library.
    * Move `CommandLineInterface` package from AceRoutine library.
    * Move all previous utlities (with no external dependencies) into the
      AceCommon library.
