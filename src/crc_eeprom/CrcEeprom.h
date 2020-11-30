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
#include <string.h> // memcpy()

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
    typedef uint32_t (*Crc32Calculator)(const void* data, uint16_t dataSize);
    typedef uint16_t (*Crc16Calculator)(const void* data, uint16_t dataSize);

    /**
     * Constructor that uses a Crc16Calculator. The default
     * CrcEeprom::crc16ccitt() method is used by default.
     */
    explicit CrcEeprom(Crc16Calculator crcCalc = crc16ccitt)
      : mCrc16Calculator(crcCalc),
        mCrcCalculatorSize(2)
      {}

    /**
     * Constructor that uses a Crc32Calculator. An example is the
     * CrcEeprom::crc32() method.
     */
    explicit CrcEeprom(Crc32Calculator crcCalc)
      : mCrc32Calculator(crcCalc),
        mCrcCalculatorSize(4)
      {}

    /**
     * Call from global setup() function. Needed for ESP8266 and ESP32,
     * does nothing for AVR and others.
     */
#if defined(ESP8266) || defined(ESP32)
    void begin(uint16_t size) {
      EEPROM.begin(size);
    }
#else
    void begin(uint16_t /*size*/) {
    }
#endif

    /**
     * Write the data with its CRC. Returns the number of bytes written.
     */
    uint16_t writeWithCrc(int address, const void* const data,
        const uint16_t dataSize) const {
      uint16_t byteCount = dataSize;
      const uint8_t* d = (const uint8_t*) data;

      // write data blcok
      while (byteCount-- > 0) {
        write(address++, *d++);
      }

      // write CRC at the end of the data block
      if (mCrcCalculatorSize == 2) {
        uint8_t buf[2];
        uint16_t crc = (*mCrc16Calculator)(data, dataSize);
        memcpy(buf, &crc, 2);
        write(address++, buf[0]);
        write(address++, buf[1]);
      } else {
        uint8_t buf[4];
        uint32_t crc = (*mCrc32Calculator)(data, dataSize);
        memcpy(buf, &crc, 4);
        write(address++, buf[0]);
        write(address++, buf[1]);
        write(address++, buf[2]);
        write(address++, buf[3]);
      }
      bool success = commit();
      return (success) ? dataSize + mCrcCalculatorSize : 0;
    }

    /**
     * Read the data from EEPROM along with its CRC. Return true if the CRC of
     * the data retrieved matches the CRC of the data when it was written.
     */
    bool readWithCrc(int address, void* const data,
        const uint16_t dataSize) const {
      uint16_t byteCount = dataSize;
      uint8_t* d = (uint8_t*) data;

      // read data block
      while (byteCount-- > 0) {
        *d++ = read(address++);
      }

      // Read data and verify same CRC
      if (mCrcCalculatorSize == 2) {
        uint8_t buf[2];
        buf[0] = read(address++);
        buf[1] = read(address++);
        uint16_t retrievedCrc;
        memcpy(&retrievedCrc, buf, 2);
        uint16_t expectedCrc = (*mCrc16Calculator)(data, dataSize);
        return expectedCrc == retrievedCrc;
      } else {
        uint8_t buf[4];
        buf[0] = read(address++);
        buf[1] = read(address++);
        buf[2] = read(address++);
        buf[3] = read(address++);
        uint32_t retrievedCrc;
        memcpy(&retrievedCrc, buf, 4);
        uint32_t expectedCrc = (*mCrc32Calculator)(data, dataSize);
        return expectedCrc == retrievedCrc;
      }
    }

    /** A CRC calculator that uses the CRC16-CCITT algorithm. */
    static uint16_t crc16ccitt(const void* data, uint16_t dataSize);

    /** A CRC calculator that uses the CRC32 algorithm. */
    static uint32_t crc32(const void* data, uint16_t dataSize);

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

    union {
      Crc16Calculator const mCrc16Calculator;
      Crc32Calculator const mCrc32Calculator;
    };
    uint8_t const mCrcCalculatorSize; // 2 or 4

};

}

#endif

#endif
