/*
  Copyright (c) 2021 Brian T. Park.

  BufferedEEPROMClass.cpp - Buffered EEPROM emulation for STM32duino. Based on
  the API from EEPROM.h from the ESP8266 Arduino Core.
*/

#include "BufferedEEPROMClass.h"

#if defined(ARDUINO_ARCH_STM32)

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
BufferedEEPROMClass BufferedEEPROM;
#endif

#endif // defined(ARDUINO_ARCH_STM32)
