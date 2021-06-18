/*
  Copyright (c) 2021 Brian T. Park.

  BufferedEEPROMClass.h - Buffered EEPROM emulation for STM32duino Based on the
  API from EEPROM.h from the ESP8266 Arduino Core.
*/

#ifndef BUFFERED_EEPROM_CLASS_H
#define BUFFERED_EEPROM_CLASS_H

#if defined(ARDUINO_ARCH_STM32)

#include <stdint.h>
#include <Arduino.h>
#include <EEPROM.h> // eeprom_buffered_*()

/**
 * An alternative implementation of `EEPROM` object on STM32 using the
 * buffered methods so that multiple bytes can be updated with a single flash
 * of the EEPROM page. The default `EEPROM` implementation flashes the page for
 * every single byte! See
 * https://github.com/stm32duino/wiki/wiki/API#EEPROM-Emulation and
 * https://github.com/stm32duino/Arduino_Core_STM32/tree/master/libraries/EEPROM/src.
 *
 * The data buffer is already allocated by the STM32duino core, and already
 * used by the normal `EEPROM` so this class does not add any additional
 * memory. Instead of using the `EEPROM` global object, use the
 * `BUFFERED_EEPROM` global object:
 *
 *  * Call `begin()` to setup.
 *  * Call `read()`, `write()`, `get()`and `put()`.
 *  * Call `commit()` or `end()` to flush the buffer to flash.
 *
 * The API of this class follows the one used on the ESP8266 and ESP32
 * platforms, not the AVR platform. Using an existing API allows the CrcEeprom
 * class to support the STM32 easier. There are some small API differences from
 * the ESP8266/ESP32 versions however:
 *
 *  * `uint8_t& operator[]()` and `const uint8_t& operator[]()` are not
 *    implemented.
 *  * The `begin()` method takes no arguments, instead of the `size` argument
 *    because the size of flash page is determined by the hardware and is fixed.
 *    Use the BufferedEEPROMclass::length() method to determine the size of the
 *    EEPROM flash page.
 */
class BufferedEEPROMClass {
public:
  BufferedEEPROMClass() = default;

  void begin() {
    eeprom_buffer_fill();
  }

  uint8_t read(int const address) {
    return eeprom_buffered_read_byte(address);
  }

  void write(int const address, uint8_t const val) {
    eeprom_buffered_write_byte(address, val);
  }

  bool commit() { eeprom_buffer_flush(); return true; }

  void end() { commit(); }

  template <typename T>
  T &get(int address, T &t) {
    size_t dataSize = sizeof(T);
    uint8_t* data = (uint8_t*) &t;
    while (dataSize--) {
      *data++ = read(address++);
    }
    return t;
  }

  template <typename T>
  const T &put(int address, const T &t) {
    size_t dataSize = sizeof(T);
    uint8_t* data = (uint8_t*) &t;
    while (dataSize--) {
      write(address++, *data++);
    }
    return t;
  }

  /**
   * Return the size of the EEPROM emulation flash page. On most STM32, this
   * will be equal to to FLASH_PAGE_SIZE. There are apparently some STM32
   * processors (e.g. STM32MP1xx) which don't have flash, so the EEPROM
   * emulation will use something called RETRAM, and FLASH_PAGE_SIZE is not
   * defined. However, it seems like even on those processors, E2END is defined
   * to be 1 smaller than the size of the EEPROM emulation size.
   */
  size_t length() const {
    return E2END + 1;
  }

  // Not implemented because the buffer address is not exposed and it's
  // too much trouble to work around this using a helper class.
  #if 0
  uint8_t * getDataPtr();
  uint8_t const * getConstDataPtr() const;

  uint8_t& operator[](int const address) {
    return getDataPtr()[address];
  }
  uint8_t const & operator[](int const address) const {
    return getConstDataPtr()[address];
  }
  #endif
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern BufferedEEPROMClass BufferedEEPROM;
#endif

#endif // defined(ARDUINO_ARCH_STM32)
#endif // BUFFERED_EEPROM_CLASS_H
