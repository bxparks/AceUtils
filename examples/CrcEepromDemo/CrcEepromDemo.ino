/*
 * Demo of CrcEeprom class.
 *
 *  * Write the 'Info' struct into EEPROM using CRC check.
 *  * Clear the struct.
 *  * Read the struct back from EEPROM using CRC check.
 */

#include <Arduino.h>
#include <AceUtilsCrcEeprom.h>
using ace_utils::crc_eeprom::CrcEeprom;

#if defined(ESP32) && ! defined(SERIAL_PORT_MONITOR)
#define SERIAL_PORT_MONITOR Serial
#endif

struct Info {
  int startTime = 100;
  int interval = 200;
};

CrcEeprom crcEeprom;

void setup() {
#if !defined(EPOXY_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR);

  crcEeprom.begin(32);

  Info info;

  SERIAL_PORT_MONITOR.print("Original info: ");
  SERIAL_PORT_MONITOR.print("startTime: ");
  SERIAL_PORT_MONITOR.print(info.startTime);
  SERIAL_PORT_MONITOR.print("; interval: ");
  SERIAL_PORT_MONITOR.println(info.interval);

  SERIAL_PORT_MONITOR.println("Writing Info struct");
  uint16_t writtenSize = crcEeprom.writeWithCrc(0, &info, sizeof(info));
  SERIAL_PORT_MONITOR.print("Written size: ");
  SERIAL_PORT_MONITOR.println(writtenSize);

  SERIAL_PORT_MONITOR.println("Clearing info struct");
  info.startTime = 0;
  info.interval = 0;
  SERIAL_PORT_MONITOR.println("Reading back Info struct");
  bool isValid = crcEeprom.readWithCrc(0, &info, sizeof(info));
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
