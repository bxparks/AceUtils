/*
 * Demo of CrcEeprom class.
 *
 *  * Write the 'Info' struct into EEPROM using CRC check.
 *  * Clear the struct.
 *  * Read the struct back from EEPROM using CRC check.
 */

#include <Arduino.h>
#include <AceUtilsCrcEeprom.h>
using ace_utils::crc_eeprom::AvrEepromAdapter;
using ace_utils::crc_eeprom::EspEepromAdapter;
using ace_utils::crc_eeprom::CrcEeprom;

#if defined(ESP32) && ! defined(SERIAL_PORT_MONITOR)
#define SERIAL_PORT_MONITOR Serial
#endif

#if defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#elif defined(EPOXY_DUINO)

  // Two versions of EEPROM.h can be selected using the ARDUINO_LIBS list in
  // the Makefile: "EpoxyPromEsp" or "EpoxyPromAvr".
  #include <EEPROM.h>
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#elif defined(ARDUINO_ARCH_STM32)

  // STM32 can support 2 versions of EEPROM.h. The built-in <EEPROM.h> uses the
  // AVR-flavored API. The <AceUtilsStm32BufferedEeprom.h> library provides the
  // BufferedEEPROM object which uses the ESP-flavored API.
  #include <AceUtilsStm32BufferedEeprom.h>
  EspEepromAdapter<BufferedEEPROMClass> eepromAdapter(BufferedEEPROM);

#else

  #include <EEPROM.h>
  AvrEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);

#endif

CrcEeprom crcEeprom(eepromAdapter, CrcEeprom::toContextId('d', 'e', 'm', 'o'));

struct Info {
  int startTime = 100;
  int interval = 200;
};

void setup() {
#if !defined(EPOXY_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR);

  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(Info)));

  Info info;

  SERIAL_PORT_MONITOR.print("Original info: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(info.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(info.interval);

  SERIAL_PORT_MONITOR.println("Writing Info struct");
  uint16_t writtenSize = crcEeprom.writeWithCrc(0, info);
  SERIAL_PORT_MONITOR.print("Written size: ");
  SERIAL_PORT_MONITOR.println(writtenSize);

  SERIAL_PORT_MONITOR.println("Clearing info struct");
  info.startTime = 0;
  info.interval = 0;
  SERIAL_PORT_MONITOR.println("Reading back Info struct");
  bool isValid = crcEeprom.readWithCrc(0, info);
  SERIAL_PORT_MONITOR.print("isValid: ");
  SERIAL_PORT_MONITOR.println(isValid);

  SERIAL_PORT_MONITOR.print("info: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(info.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(info.interval);

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
