/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

#ifndef ACE_UTILS_CRC_EEPROM_EEPROM_INTERFACE_H
#define ACE_UTILS_CRC_EEPROM_EEPROM_INTERFACE_H

namespace ace_utils {
namespace crc_eeprom {

#if 0
/**
 * The base EEPROM API used by CrcEeprom class. Different platforms have
 * implemented the `EEPROM` object in different ways, and this interface hides
 * the implementation differences from the CrcEeprom class. There are at least 2
 * different EEPROM APIs:
 *
 *  * AVR-style (AVR, Teensy, STM32) and
 *  * ESP-style (ESP8266, ESP32).
 *
 * Sometimes, it makes sense to implement both versions, for example, on the
 * STM32 where the default `EEPROM` is horribly inefficient so a buffered
 * version (e.g. ../buffered_eeprom_stm32) should be used.
 */
class EepromInterface {
  public:
    /** Write thte byte at address, potentially buffered. */
    virtual void write(size_t address, uint8_t val) = 0;

    /** Return the byte at address. */
    virtual uint8_t read(size_t address) const = 0;

    /** Flush the buffer if it is used. */
    virtual bool commit() = 0;
};
#endif

/**
 * A wrapper class around an EEPROM class that follows the AVR-style API. This
 * is implemented as a template to prevent compile-time errors on platforms
 * which are not compatible with this API style. The application writer will
 * choose to use either the AvrStyleEeprom or the EspStyleEeprom.
 *
 * @tparam E type of the EEPROM class
 */
template <typename E>
class AvrStyleEeprom {
  public:
    /** Wrap around an AVR-style EEPROM object. */
    AvrStyleEeprom(E &eeprom) : mEeprom(eeprom) {}

    void write(size_t address, uint8_t val) {
      mEeprom.update(address, val);
    }

    uint8_t read(size_t address) const {
      return mEeprom.read(address);
    }

    bool commit() {
      return true;
    }

  private:
    E& mEeprom;
};

/**
 * A wrapper class around an EEPROM class that follows the ESP-style API. This
 * is implemented as a template to prevent compile-time errors on platforms
 * which are not compatible with this API style. The application writer will
 * choose to use either the AvrStyleEeprom or the EspStyleEeprom.
 *
 * @tparam E type of the EEPROM class
 */
template <typename E>
class EspStyleEeprom {
  public:
    /** Wrap around an ESP-style EEPROM object. */
    EspStyleEeprom(E &eeprom) : mEeprom(eeprom) {}

    uint8_t read(size_t address) const {
      return mEeprom.read(address);
    }

    void write(size_t address, uint8_t val) {
      mEeprom.write(address, val);
    }

    bool commit() {
      return mEeprom.commit();
    }

  private:
    E& mEeprom;
};

} // crc_eeprom
} // ace_utils

#endif
