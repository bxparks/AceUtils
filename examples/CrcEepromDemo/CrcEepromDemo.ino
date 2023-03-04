/*
 * Demo of CrcEeprom class.
 *
 *  * Write the 'StoredInfo' struct into EEPROM using CRC check.
 *  * Clear the struct.
 *  * Read the struct back from EEPROM using CRC check.
 */

#include <Arduino.h>
#include <AceUtils.h>
#include <crc_eeprom/crc_eeprom.h>
using ace_utils::crc_eeprom::CrcEepromAvr;
using ace_utils::crc_eeprom::CrcEepromEsp;

#if ! defined(SERIAL_PORT_MONITOR)
  #define SERIAL_PORT_MONITOR Serial
#endif

// The contextId can be anything you want, and should uniquely identify the
// application to avoid collisions with another applications that store data
// with the same length.
const uint32_t CONTEXT_ID = 0xeca4c474;

#if defined(EPOXY_DUINO)
  // Two versions of EEPROM.h can be selected using the ARDUINO_LIBS list in
  // the Makefile: "EpoxyEepromEsp" or "EpoxyEepromAvr".
  #include <EpoxyEepromEsp.h>
  CrcEepromEsp<EpoxyEepromEsp> crcEeprom(EpoxyEepromEspInstance, CONTEXT_ID);

#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  CrcEepromEsp<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);

#elif defined(ARDUINO_ARCH_STM32)
  #if 1
    // Use this for STM32. The buffered_eeprom_stm32 library provides
    // the BufferedEEPROM object which internally uses the buffered versions of
    // the low-level eeprom functions. The BufferedEEPROM object implements the
    // ESP-flavored EEPROM API.
    #include <AceUtils.h>
    #include <buffered_eeprom_stm32/buffered_eeprom_stm32.h> // AceUtils
    CrcEepromEsp<BufferedEEPROMClass> crcEeprom(BufferedEEPROM, CONTEXT_ID);
  #else
    // Don't do this for STM32 because the default EEPROM flashes the entire
    // page for *every* byte!
    #include <EEPROM.h>
    CrcEepromAvr<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);
  #endif

#else
  // Assume AVR
  #include <EEPROM.h>
  CrcEepromAvr<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);

#endif

void setupEeprom() {
#if defined(EPOXY_DUINO)
  EpoxyEepromEspInstance.begin(1024);
#elif defined(ESP8266) || defined(ESP32)
  EEPROM.begin(256);
#elif defined(ARDUINO_ARCH_STM32)
  BufferedEEPROM.begin();
#else
  // Assume AVR style EEPOM and do nothing.
#endif
}

struct StoredInfo {
  int startTime = 100;
  int interval = 200;
};

void setup() {
#if !defined(EPOXY_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR);

  setupEeprom();

  StoredInfo storedInfo;

  SERIAL_PORT_MONITOR.print("Generating StoredInfo: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(storedInfo.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(storedInfo.interval);

  SERIAL_PORT_MONITOR.println("Writing StoredInfo");
  size_t writtenSize = crcEeprom.writeWithCrc(0, storedInfo);
  SERIAL_PORT_MONITOR.print("Written size: ");
  SERIAL_PORT_MONITOR.println(writtenSize);

  SERIAL_PORT_MONITOR.println("Clearing storedInfo");
  storedInfo.startTime = 0;
  storedInfo.interval = 0;
  SERIAL_PORT_MONITOR.println("Reading back StoredInfo");
  bool isValid = crcEeprom.readWithCrc(0, storedInfo);
  SERIAL_PORT_MONITOR.print("isValid: ");
  SERIAL_PORT_MONITOR.println(isValid);

  SERIAL_PORT_MONITOR.print("StoredInfo: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(storedInfo.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(storedInfo.interval);

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
