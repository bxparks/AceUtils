#line 2 "CrcEepromTest.ino"

#include <Arduino.h>
#include <AUnitVerbose.h>
#include <AceUtils.h>
#include <crc_eeprom/crc_eeprom.h> // from AceUtils

using aunit::TestRunner;
using ace_utils::crc_eeprom::CrcEepromEsp;
using ace_utils::crc_eeprom::CrcEepromAvr;

// The contextId can be anything you want, and should uniquely identify the
// application to avoid collisions with another applications that store data
// with the same length.
const uint32_t CONTEXT_ID = 0x812e4519;

#if defined(EPOXY_DUINO)
  #include <EpoxyEepromEsp.h>
  CrcEepromEsp<EpoxyEepromEsp> crcEeprom(EpoxyEepromEspInstance, CONTEXT_ID);
#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  CrcEepromEsp<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);
#elif defined(ARDUINO_ARCH_STM32)
  #include <AceUtils.h>
  #include <buffered_eeprom_stm32/buffered_eeprom_stm32.h>
  CrcEepromEsp<BufferedEEPROMClass> crcEeprom(BufferedEEPROM, CONTEXT_ID);
#else // Assume AVR
  #include <EEPROM.h>
  CrcEepromAvr<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);
#endif

struct Info {
  int startTime;
  int interval;
};

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

//----------------------------------------------------------------------------

test(CrcEepromTest, readWrite) {
  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);

  assertTrue(status);
  assertEqual(1, info.startTime);
  assertEqual(2, info.interval);
}

test(CrcEepromTest, readWriteInvalidCrc_shouldFail) {
  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  // Write directly into stored Info to invalidate the CRC
#if defined(EPOXY_DUINO)
  EpoxyEepromEspInstance.write(4, 0x55);
  EpoxyEepromEspInstance.commit();
#elif defined(ESP8266) || defined(ESP32)
  EEPROM.write(4, 0x55);
  EEPROM.commit();
#elif defined(ARDUINO_ARCH_STM32)
  BufferedEEPROM.write(4, 0x55);
  BufferedEEPROM.commit();
#else
  EEPROM.update(4, 0x55);
#endif

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);
  assertFalse(status);
}

test(CrcEepromTest, readWriteInvalidContextId_shouldFail) {
  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  // Write directly into 'contextId' to invalidate it
  const uint32_t contextId2 = 0x12345678;
#if defined(EPOXY_DUINO_EPOXY_EEPROM_ESP)
  EpoxyEepromEspInstance.put(0, contextId2);
  EpoxyEepromEspInstance.commit();
#elif defined(ESP8266) || defined(ESP32)
  EEPROM.put(0, contextId2);
  EEPROM.commit();
#elif defined(ARDUINO_ARCH_STM32)
  BufferedEEPROM.put(0, contextId2);
  BufferedEEPROM.commit();
#else
  EEPROM.put(0, contextId2);
#endif

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);
  assertFalse(status);
}

//----------------------------------------------------------------------------

void setup() {
#if defined(ARDUINO)
  delay(1000); // some boards reboot twice
#endif
  Serial.begin(115200);
  while (!Serial); // Leonardo/Micro

  setupEeprom();
}

void loop() {
  TestRunner::run();
}
