/*
  Copyright (c) 2021 Brian T. Park.

  BufferedEeprom.cpp - Buffered EEPROM emulation for STM32duino. Based on
  the API from EEPROM.h from the ESP8266 Arduino Core.
*/

#include "BufferedEeprom.h"

#if defined(ARDUINO_ARCH_STM32)

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
BufferedEeprom BUFFERED_EEPROM;
#endif

#endif // defined(ARDUINO_ARCH_STM32)
