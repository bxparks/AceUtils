#ifndef ACE_UTILS_FREEMEM_H
#define ACE_UTILS_FREEMEM_H

#include <stddef.h> // size_t

/**
 * @file freemem.h
 *
 * Usage:
 *
 * @verbatim
 * #include <AceUtils.h>
 * #include <freemem/freemem.h>
 * using ace_utils::freemem::freeMemory;
 *
 * size_t free = freeMemory();
 * @endverbatim
 *
 * Supported platforms: AVR, SAMD, STM32, ESP8266, ESP32, and Linux
 */

namespace ace_utils {
namespace freemem {

/** Return the free memory on the heap. */
extern size_t freeMemory();

}
}

#endif
