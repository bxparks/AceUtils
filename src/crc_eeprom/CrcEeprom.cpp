/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include "CrcEeprom.h"
#if defined(ARDUINO_ARCH_STM32)
  #include <AceUtilsStm32BufferedEeprom.h>
  #define EEPROM BUFFERED_EEPROM
#else
  #include <EEPROM.h>
#endif

namespace ace_utils {
namespace crc_eeprom {

void CrcEeprom::begin(size_t size) {
#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_DUINO_EPOXY_PROM_ESP)
  EEPROM.begin(size);
#elif defined(ARDUINO_ARCH_STM32)
  (void) size; // disable unused variable compiler warning
  EEPROM.begin();
#else
  (void) size; // disable unused variable compiler warning
#endif
}

/**
  * Write the data with its CRC and its `contextId`. Returns the number of
  * bytes written, or 0 if a failure occurred. The type of `address` is `int`
  * for consistency with the API of the EEPROM library.
  */
size_t CrcEeprom::writeWithCrc(
    int address,
    const void* const data,
    const size_t dataSize
) const {
  const int address0 = address;

  // write the contextId
  EEPROM.put(address, mContextId);
  address += sizeof(mContextId);

  // write data block
  size_t byteCount = dataSize;
  const uint8_t* d = (const uint8_t*) data;
  while (byteCount-- > 0) {
    write(address++, *d++);
  }

  // write CRC at the end of the data block
  uint32_t crc = (*mCrc32Calculator)(data, dataSize);
  EEPROM.put(address, crc);
  address += sizeof(crc);

  bool success = commit();
  return (success) ? address - address0: 0;
}

/**
  * Read the data from EEPROM along with its CRC and `contextId`. Return true
  * if both the CRC of the retrieved data and its `contextId` matches the
  * expected CRC and `contextId` when it was written. The type of `address`
  * is `int` for consistency with the API of the EEPROM library.
  */
bool CrcEeprom::readWithCrc(
    int address,
    void* const data,
    const size_t dataSize
) const {
  // read contextId
  uint32_t retrievedContextId;
  EEPROM.get(address, retrievedContextId);
  if (retrievedContextId != mContextId) return false;
  address += sizeof(retrievedContextId);

  // read data block
  size_t byteCount = dataSize;
  uint8_t* d = (uint8_t*) data;
  while (byteCount-- > 0) {
    *d++ = read(address++);
  }

  // read the CRC
  uint32_t retrievedCrc;
  EEPROM.get(address, retrievedCrc);
  address += sizeof(retrievedCrc);

  // Verify CRC
  uint32_t expectedCrc = (*mCrc32Calculator)(data, dataSize);
  return expectedCrc == retrievedCrc;
}

void CrcEeprom::write(int address, uint8_t val) const {
#if defined(ESP8266) \
    || defined(ESP32) \
    || defined(EPOXY_DUINO_EPOXY_PROM_ESP) \
    || defined(ARDUINO_ARCH_STM32)
  EEPROM.write(address, val);
#else
  EEPROM.update(address, val);
#endif
}

uint8_t CrcEeprom::read(int address) const {
  return EEPROM.read(address);
}

bool CrcEeprom::commit() const {
#if defined(ESP8266) \
    || defined(ESP32) \
    || defined(EPOXY_DUINO_EPOXY_PROM_ESP) \
    || defined(ARDUINO_ARCH_STM32)
  return EEPROM.commit();
#else
  return true;
#endif
}

} // crc_eeprom
} // ace_utils
