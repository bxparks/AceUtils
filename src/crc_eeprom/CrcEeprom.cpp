// Cannot compile this on UnixHostDuino
#if ! defined(UNIX_HOST_DUINO)

#include <AceCRC.h>
#include "CrcEeprom.h"

using namespace ace_crc;

namespace crc_eeprom {

uint16_t CrcEeprom::crc16ccitt(const void* data, uint16_t dataSize) {
  crc16ccitt_nibble::crc_t crc = crc16ccitt_nibble::crc_init();
  crc = crc16ccitt_nibble::crc_update(crc, data, dataSize);
  crc = crc16ccitt_nibble::crc_finalize(crc);
  return crc;
}

uint32_t CrcEeprom::crc32(const void* data, uint16_t dataSize) {
  crc32_nibble::crc_t crc = crc32_nibble::crc_init();
  crc = crc32_nibble::crc_update(crc, data, dataSize);
  crc = crc32_nibble::crc_finalize(crc);
  return crc;
}

}

#endif
