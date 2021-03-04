/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include "CrcEeprom.h"

namespace ace_utils {
namespace crc_eeprom {

size_t CrcEeprom::writeDataWithCrc(
    size_t address,
    const void* const data,
    size_t const dataSize
) {
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

} // crc_eeprom
} // ace_utils
