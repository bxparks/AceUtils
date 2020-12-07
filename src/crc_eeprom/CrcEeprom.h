/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_UTILS_CRC_EEPROM_H
#define ACE_UTILS_CRC_EEPROM_H

// Cannot compile this on UnixHostDuino
#if ! defined(UNIX_HOST_DUINO)

// EEPROM is supported only on certain Arduino boards. In particular, many
// (most?) Arduino Zero compatible boards cannot support EEPROM even on Flash
// emulation because the version of the SAMD21 chip on the board doesn't
// support RWW (read-while-write).
#if !defined(AVR) && !defined(ESP8266) && !defined(ESP32) && \
    !defined(TEENSYDUINO)
  #error Unsupported architecture
#endif

#include <EEPROM.h>
#include <AceCRC.h> // crc32_nibble
#include <string.h> // memcpy()

namespace ace_utils {
namespace crc_eeprom {

/**
 * Thin wrapper around the EEPROM object (from the the built-in EEPROM library)
 * to read and write a given block of data along with its CRC check. The CRC is
 * written *after* the data block, instead of at the beginning of the data
 * block to reduce flash write wear of the bytes corresonding to the CRC. Over
 * time, it is expected that the size of the data block will change, as fields
 * are added or deleted. Therefore, the location of the CRC bytes will also
 * change, which helps wear leveling. If the CRC bytes were at the beginning,
 * those CRC byes would experience the highest level of writes, even when the
 * data block size changes.
 */
class CrcEeprom {
  public:
    /**
     * Function pointer to the CRC32 calculator. For example,
     * `ace_crc::crc32_nibble::crc_calculate` or
     * `ace_crc::crc32_byte::crc_calculate`.
     */
    typedef uint32_t (*Crc32Calculator)(const void* data, size_t dataSize);

    /** Size of the crc32 type, uint32_t. Should always be 4. */
    const size_t kCrcSize = sizeof(uint32_t);

    /**
     * Constructor with an optional Crc32Calculator parameter. By default, the
     * Crc32Calculator will be set to `ace_crc::crc32_nibble::crc_calculate()`
     * which uses a 4-bit (16 element) lookup table and has a good balance
     * between flash memory consumption and speed. You can choose to provide an
     * alternate CRC32 calculator for faster speed or for smaller flash memory
     * consumption.
     */
    explicit CrcEeprom(
        Crc32Calculator crcCalc = ace_crc::crc32_nibble::crc_calculate)
      : mCrc32Calculator(crcCalc)
      {}

    /**
     * Call from global setup() function. Needed for ESP8266 and ESP32,
     * does nothing for AVR and others.
     */
#if defined(ESP8266) || defined(ESP32)
    void begin(size_t size) {
      EEPROM.begin(size);
    }
#else
    void begin(size_t /*size*/) {
    }
#endif

    /**
     * Write the data with its CRC. Returns the number of bytes written.
     * The type of `address` is `int` for consistency with the API of the
     * EEPROM library.
     */
    size_t writeWithCrc(
        int address,
        const void* const data,
        const size_t dataSize
    ) const {
      size_t byteCount = dataSize;
      const uint8_t* d = (const uint8_t*) data;

      // write data blcok
      while (byteCount-- > 0) {
        write(address++, *d++);
      }

      // write CRC at the end of the data block
      uint32_t crc = (*mCrc32Calculator)(data, dataSize);
      EEPROM.put(address, crc);
      bool success = commit();
      return (success) ? dataSize + kCrcSize : 0;
    }

    /**
     * Read the data from EEPROM along with its CRC. Return true if the CRC of
     * the data retrieved matches the CRC of the data when it was written.
     * The type of `address` is `int` for consistency with the API of the
     * EEPROM library.
     */
    bool readWithCrc(
        int address,
        void* const data,
        const size_t dataSize
    ) const {
      size_t byteCount = dataSize;
      uint8_t* d = (uint8_t*) data;

      // read data block
      while (byteCount-- > 0) {
        *d++ = read(address++);
      }

      // Read data and verify same CRC
      uint32_t retrievedCrc;
      EEPROM.get(address, retrievedCrc);
      uint32_t expectedCrc = (*mCrc32Calculator)(data, dataSize);
      return expectedCrc == retrievedCrc;
    }

  private:
    void write(int address, uint8_t val) const {
#if defined(ESP8266) || defined(ESP32)
      EEPROM.write(address, val);
#else
      EEPROM.update(address, val);
#endif
    }

    uint8_t read(int address) const {
      return EEPROM.read(address);
    }

    bool commit() const {
#if defined(ESP8266) || defined(ESP32)
      return EEPROM.commit();
#else
      return true;
#endif
    }

    Crc32Calculator const mCrc32Calculator;
};

} // crc_eeprom
} // ace_utils

#endif // ! defined(UNIX_HOST_DUINO)

#endif // defined(ACE_UTILS_CRC_EEPROM_H)
