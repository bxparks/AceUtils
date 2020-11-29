#include "CrcEeprom.h"
#include "crc16ccitt.h"
#include "crc32.h"

// TODO: Move the underyling implementations (crc16ccitt(), crc32(), etc)
// to AceCommon, or maybe even a new library, AceCrc.

namespace crc_eeprom {

uint16_t CrcEeprom::crc16ccitt(const void* data, uint16_t dataSize) {
  crc32::crc_t crc = crc16ccitt::crc_init();
  crc = crc16ccitt::crc_update(crc, data, dataSize);
  crc = crc16ccitt::crc_finalize(crc);
  return crc;
}

uint32_t CrcEeprom::crc32(const void* data, uint16_t dataSize) {
  crc32::crc_t crc = crc32::crc_init();
  crc = crc32::crc_update(crc, data, dataSize);
  crc = crc32::crc_finalize(crc);
  return crc;
}

}
