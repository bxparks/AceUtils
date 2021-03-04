/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_UTILS_CRC_EEPROM_H
#define ACE_UTILS_CRC_EEPROM_H

// EEPROM is supported only on certain Arduino boards. In particular, many
// (most?) Arduino Zero compatible boards cannot support EEPROM even on Flash
// emulation because the version of the SAMD21 chip on the board doesn't
// support RWW (read-while-write).
#if !defined(ARDUINO_ARCH_AVR) \
    && !defined(ARDUINO_ARCH_STM32) \
    && !defined(ESP8266) \
    && !defined(ESP32) \
    && !defined(TEENSYDUINO) \
    && !defined(EPOXY_DUINO)
  #error Unsupported architecture
#endif

#include <AceCRC.h> // crc32_nibble

namespace ace_utils {
namespace crc_eeprom {

/**
 * Thin wrapper around the EEPROM object (from the the built-in EEPROM library)
 * to read and write a given block of data along with its CRC check. When the
 * data is read back, the CRC is recomputed and checked against the CRC stored
 * in EEPROM. If they do not match, the `readWithCrc()` method returns false.
 *
 * An optional `contextId` can also be stored with the data. It is provided by
 * the calling application to identify the context of the data being stored in
 * the EEPROM. If another app happens to store a different data, with the same
 * data length, the CRC would return valid even though the data is not
 * compatible. This application-defined identifier prevents the collision.
 *
 * The `contextId` is written *before* the data block because the `contextId`
 * will not change over multiple updates to the data block. This will help with
 * wear-leveling on the AVR platforms.
 *
 * The CRC is written *after* the data block, instead of at the beginning of
 * the data block to reduce flash write wear of the bytes corresonding to the
 * CRC. Over time, it is expected that the size of the data block will change,
 * as fields are added or deleted. Therefore, the location of the CRC bytes
 * will also change, which helps wear-leveling. If the CRC bytes were at the
 * beginning, those CRC byes would experience the highest level of writes, even
 * when the data block size changes.
 */
class CrcEeprom {
  public:
    /**
     * Function pointer to the CRC32 calculator. For example,
     * `ace_crc::crc32_nibble::crc_calculate` or
     * `ace_crc::crc32_byte::crc_calculate`.
     */
    typedef uint32_t (*Crc32Calculator)(const void* data, size_t dataSize);

    /**
     * Convert 4 characters into a uint32_t `contextId` Example
     * `toContextId('d', 'e', 'm', 'o')`. On little-endian processors, the
     * implemented formula will cause the uint32_t number to be written as
     * (a,b,c,d) in memory, which helps debugging.
     */
    static constexpr uint32_t toContextId(char a, char b, char c, char d) {
      return ((uint32_t) d << 24)
          | ((uint32_t) c << 16)
          | ((uint32_t) b << 8)
          | a;
    }

    /**
     * Return the actual number of bytes saved to EEPROM for the given dataSize.
     * This includes the `contextId` header and the CRC. This is the minimum
     * size that should be passed into the begin() method.
     */
    static constexpr size_t toSavedSize(size_t dataSize) {
      return dataSize + 8;
    }

    /**
     * Constructor with an optional `contextId` identifier and an
     * optional Crc32Calculator `crcCalc` function pointer.
     *
     * @param contextId an optional application-defined identifier of the data
     *    being stored. This prevents collisions between 2 different data which
     *    just happens to be the same size. The default is 0 for backwards
     *    compatibility but this field is highly recommended to be defined.
     *    Use the `toContextId()` function to convert 4 human-readable
     *    characters into a uint32_t.
     * @param crcCalc an optional CRC32 calculator. By default, the
     *    Crc32Calculator will be set to
     *    `ace_crc::crc32_nibble::crc_calculate()` except on ESP8266 where it
     *    will be set to `ace_crc::crc32_nibblem::crc_calculate()` because the
     *    latter is 2.7X faster on the ESP8266. Both of these algorithms use a
     *    4-bit (16 element) lookup table and has a good balance between flash
     *    memory consumption and speed. See https://github.com/bxparks/AceCRC
     *    for details.
     */
    explicit CrcEeprom(
      uint32_t contextId = 0,
      #if defined(ESP8266)
        Crc32Calculator crcCalc = ace_crc::crc32_nibblem::crc_calculate
      #else
        Crc32Calculator crcCalc = ace_crc::crc32_nibble::crc_calculate
      #endif
    ) :
        mContextId(contextId),
        mCrc32Calculator(crcCalc)
    {}

    /**
     * Call from global setup() function. A convenience method to smooth
     * over the API differences of the EEPROM classes. Calls `EEPROM.begin()`
     * on ESP8266/ESP32, does nothing for AVR and others.
     *
     * You can call `EEPROM.begin()` manually instead of calling this, but you
     * would then have to perform the compile-time check for ESP8266, ESP32 or
     * EpoxyPromEsp yourself, since the AVR, Teensy, STM32 and EpoxyPromAvr
     * versions of EEPROM do not have a version of begin() that takes a size
     * parameter.
     *
     * If you want to store 2 different data structures:
     *
     *  * Set `totalSize = toSavedSize(sizeof(data1)) +
     *     toSavedSize(sizeof(data2))`.
     *  * Call `begin(totalSize)`.
     *  * Set `address1 = 0` and `address2 = toSavedSize(sizeof(data1)`.
     *  * Call `writeWithCrc(address1, data1, sizeof(data1))`.
     *  * Call `writeWithCrc(address2, data2, sizeof(data2))`.
     *
     * @param size number of bytes to reserve for the EEPROM. This should be
     *    greater than or equal to the value returned by
     *    `toSavedSize(sizeof(data))`.
     */
    void begin(size_t size);

    /**
     * Write the data with its CRC and its `contextId`. Returns the number of
     * bytes written, or 0 if a failure occurred. The type of `address` is `int`
     * for consistency with the API of the EEPROM library.
     */
    size_t writeWithCrc(
        int address,
        const void* const data,
        const size_t dataSize
    ) const;

    /**
     * Read the data from EEPROM along with its CRC and `contextId`. Return true
     * if both the CRC of the retrieved data and its `contextId` matches the
     * expected CRC and `contextId` when it was written. The type of `address`
     * is `int` for consistency with the API of the EEPROM library.
     */
    bool readWithCrc(
        int address,
        void* const data,
        const size_t dataSize
    ) const;

  private:
    void write(int address, uint8_t val) const;

    uint8_t read(int address) const;

    bool commit() const;

    uint32_t const mContextId;
    Crc32Calculator const mCrc32Calculator;
};

} // crc_eeprom
} // ace_utils

#endif // defined(ACE_UTILS_CRC_EEPROM_H)
