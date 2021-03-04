/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_UTILS_CRC_EEPROM_H
#define ACE_UTILS_CRC_EEPROM_H

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

    void begin(size_t size);

    /**
     * Convenience method that writes the given `data` of type `T` at given
     * `address`. The compiler figures out the `sizeof(T)` automatically before
     * calling `writeDataWithCrc()`.
     *
     * @tparam T type of `data`
     */
    template<typename T>
    size_t writeWithCrc(size_t address, const T& data) const {
      return writeDataWithCrc(address, &data, sizeof(T));
    }

    /**
     * Convenience function that reads the given `data` of type `T` at given
     * `address`. The compiler figures out the `sizeof(T)` automatically before
     * calling `readDataWithCrc()`.
     *
     * @tparam T type of `data`
     */
    template<typename T>
    bool readWithCrc(size_t address, T& data) const {
      return readDataWithCrc(address, &data, sizeof(T));
    }

    /**
     * Write the data with its CRC and its `contextId`. Returns the number of
     * bytes written, or 0 if a failure occurred.
     */
    size_t writeDataWithCrc(size_t address, const void* data, size_t dataSize)
        const;

    /**
     * Read the data from EEPROM along with its CRC and `contextId`. Return true
     * if both the CRC of the retrieved data and its `contextId` matches the
     * expected CRC and `contextId` when it was written.
     */
    bool readDataWithCrc(size_t address, void* data, size_t dataSize) const;

  protected:
    virtual void write(size_t address, uint8_t val) const;

    virtual uint8_t read(size_t address) const;

    virtual bool commit() const;

    void readData(size_t address, uint8_t* data, size_t size) const {
      while (size--) {
        *data++ = read(address++);
      }
    }

    void writeData(size_t address, const uint8_t* data, size_t size) const {
      while (size--) {
        write(address++, *data++);
      }
    }

    uint32_t const mContextId;
    Crc32Calculator const mCrc32Calculator;
};

} // crc_eeprom
} // ace_utils

#endif // defined(ACE_UTILS_CRC_EEPROM_H)
