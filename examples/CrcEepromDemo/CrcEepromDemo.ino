/*
 * Demo of CrcEeprom class.
 *
 *  * Write the 'StoredInfo' struct into EEPROM using CRC check.
 *  * Clear the struct.
 *  * Read the struct back from EEPROM using CRC check.
 */

#include <Arduino.h>
#include <AceUtilsCrcEeprom.h>
using ace_utils::crc_eeprom::AvrEepromAdapter;
using ace_utils::crc_eeprom::EspEepromAdapter;
using ace_utils::crc_eeprom::CrcEeprom;

#if ! defined(SERIAL_PORT_MONITOR)
  #define SERIAL_PORT_MONITOR Serial
#endif

#if defined(EPOXY_DUINO)

  // Two versions of EEPROM.h can be selected using the ARDUINO_LIBS list in
  // the Makefile: "EpoxyPromEsp" or "EpoxyPromAvr".
  #include <EEPROM.h>
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#elif defined(ARDUINO_ARCH_STM32)

  // Use this for STM32. The <AceUtilsStm32BufferedEeprom.h> library provides
  // the BufferedEEPROM object which internally uses the buffered versions of
  // the low-level eeprom functions. The BufferedEEPROM object implements the
  // ESP-flavored EEPROM API.
  #include <AceUtilsStm32BufferedEeprom.h>
  EspEepromAdapter<BufferedEEPROMClass> eepromAdapter(BufferedEEPROM);

  // Don't do this for STM32 because the default EEPROM flashes the entire
  // page for *every* byte!
  //#include <EEPROM.h>
  //AvrEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#else

  #include <EEPROM.h>
  AvrEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#endif

// The contextId is generated from "demo". This can be anything you want, and
// should uniquely identify the application to avoid collisions with another
// applications that store data with the same length.
CrcEeprom crcEeprom(eepromAdapter, CrcEeprom::toContextId('d', 'e', 'm', 'o'));

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

  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(StoredInfo)));

  StoredInfo storedInfo;

  SERIAL_PORT_MONITOR.print("Original storedInfo: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(storedInfo.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(storedInfo.interval);

  SERIAL_PORT_MONITOR.println("Writing StoredInfo struct");
  size_t writtenSize = crcEeprom.writeWithCrc(0, storedInfo);
  SERIAL_PORT_MONITOR.print("Written size: ");
  SERIAL_PORT_MONITOR.println(writtenSize);

  SERIAL_PORT_MONITOR.println("Clearing storedInfo struct");
  storedInfo.startTime = 0;
  storedInfo.interval = 0;
  SERIAL_PORT_MONITOR.println("Reading back StoredInfo struct");
  bool isValid = crcEeprom.readWithCrc(0, storedInfo);
  SERIAL_PORT_MONITOR.print("isValid: ");
  SERIAL_PORT_MONITOR.println(isValid);

  SERIAL_PORT_MONITOR.print("storedInfo: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(storedInfo.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(storedInfo.interval);

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
