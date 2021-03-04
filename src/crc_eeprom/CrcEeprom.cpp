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

size_t CrcEeprom::writeDataWithCrc(
    size_t address,
    const void* const data,
    size_t const dataSize
) const {
  const size_t address0 = address;

  // write the contextId
  writeData(address, (const uint8_t*) &mContextId, sizeof(mContextId));
  address += sizeof(mContextId);

  // write data block
  writeData(address, (const uint8_t*) data, dataSize);
  address += dataSize;

  // write CRC at the end of the data block
  uint32_t crc = (*mCrc32Calculator)(data, dataSize);
  writeData(address, (const uint8_t*) &crc, sizeof(crc));
  address += sizeof(crc);

  bool success = commit();
  return (success) ? address - address0: 0;
}

bool CrcEeprom::readDataWithCrc(
    size_t address,
    void* const data,
    size_t const dataSize
) const {
  // read contextId
  uint32_t retrievedContextId;
  readData(address, (uint8_t*) &retrievedContextId, sizeof(retrievedContextId));
  if (retrievedContextId != mContextId) return false;
  address += sizeof(retrievedContextId);

  // read data block
  readData(address, (uint8_t*) data, dataSize);
  address += dataSize;

  // read the CRC
  uint32_t retrievedCrc;
  readData(address, (uint8_t*) &retrievedCrc, sizeof(retrievedCrc));
  address += sizeof(retrievedCrc);

  // Verify CRC
  uint32_t expectedCrc = (*mCrc32Calculator)(data, dataSize);
  return expectedCrc == retrievedCrc;
}

void CrcEeprom::write(size_t address, uint8_t val) const {
#if defined(ESP8266) \
    || defined(ESP32) \
    || defined(EPOXY_DUINO_EPOXY_PROM_ESP) \
    || defined(ARDUINO_ARCH_STM32)
  EEPROM.write(address, val);
#else
  EEPROM.update(address, val);
#endif
}

uint8_t CrcEeprom::read(size_t address) const {
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
