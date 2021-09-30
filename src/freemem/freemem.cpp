/*
MIT License
Copyright (c) 2018 Brian T. Park
*/

#include "freemem.h"

#if defined(EPOXY_DUINO)
  #include <unistd.h> // _SC_PHYS_PAGES, _SC_PAGE_SIZE
#elif defined(ARDUINO_ARCH_AVR)
  extern "C" {
    extern char *__brkval;
    extern char *__malloc_heap_start;
  }
#elif defined(ARDUINO_ARCH_SAMD)
  extern "C" char* sbrk(int incr);
#elif defined(ARDUINO_ARCH_STM32)
  extern "C" char* sbrk(int incr);
#elif defined(ESP8266)
  extern "C" {
    #include "user_interface.h"
  }
#elif defined(ESP32)
  #include "Esp.h" // ESP
#else
  // Try a default implementation.
  extern "C" char* sbrk(int incr);
#endif

namespace ace_utils {
namespace freemem {

/**
 * Return the amount of free memory. See various tutorials and references:
 *
 * * https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
 * * https://arduino.stackexchange.com/questions/30497
 * * https://github.com/mpflaga/Arduino-MemoryFree/blob/master/MemoryFree.cpp
 *
 * For a Nano:
 * * The original code returns 2252, which seems too high since it has only
 *   2048 of static RAM. Changed to always test for non-zero value of __brkval,
 *   which gives 1553 which seems more accurate because the Arduino IDE says
 *   that the sketch leaves 1605 bytes for RAM.
 *
 * For STM32:
 * * The sbrk() code seems to work. But see
 *   https://github.com/stm32duino/STM32Examples/blob/master/examples/Benchmarking/MemoryAllocationStatistics/MemoryAllocationStatistics.ino
 *   for more info.
 *
 * For ESP8266:
 * * https://github.com/esp8266/Arduino/issues/81
 *
 * For ESP32:
 * * https://techtutorialsx.com/2017/12/17/esp32-arduino-getting-the-free-heap/
 *
 * For Unix:
 * * https://stackoverflow.com/questions/2513505
 */
size_t freeMemory() {
#if defined(ARDUINO_ARCH_AVR)
  char top;
  return &top - (__brkval ? __brkval : __malloc_heap_start);
#elif defined(ARDUINO_ARCH_SAMD)
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(ARDUINO_ARCH_STM32)
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(TEENSYDUINO)
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(ESP8266)
  return system_get_free_heap_size();
#elif defined(ESP32)
  return ESP.getFreeHeap();
#elif defined(EPOXY_DUINO)
  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  return pages * page_size;
#else
  #warning Untested platform, using default implementation of freeMemory()
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#endif
}

}
}
