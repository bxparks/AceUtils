#line 2 "CrcEepromTest.ino"

#include <Arduino.h>
#include <EEPROM.h>
#include <AUnitVerbose.h>
#include <AceUtilsCrcEeprom.h>

using aunit::TestRunner;
using ace_utils::crc_eeprom::CrcEeprom;
using ace_utils::crc_eeprom::EspEepromAdapter;

struct Info {
  int startTime;
  int interval;
};

test(CrcEepromTest, readWrite) {
  const uint32_t contextId = CrcEeprom::toContextId('I', 'n', 'f', 'o');
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);
  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(Info)));
  CrcEeprom crcEeprom(eepromAdapter, contextId);

  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);

  assertTrue(status);
  assertEqual(1, info.startTime);
  assertEqual(2, info.interval);
}

test(CrcEepromTest, readWriteInvalidCrc_shouldFail) {
  const uint32_t contextId = CrcEeprom::toContextId('I', 'n', 'f', 'o');
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);
  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(Info)));
  CrcEeprom crcEeprom(eepromAdapter, contextId);

  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  // Write directly into stored Info to invalidate the CRC
#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_DUINO_EPOXY_PROM_ESP)
  EEPROM.write(4, 0x55);
  EEPROM.commit();
#else
  EEPROM.update(4, 0x55);
#endif

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);
  assertFalse(status);
}

test(CrcEepromTest, readWriteInvalidContextId_shouldFail) {
  const uint32_t contextId = CrcEeprom::toContextId('I', 'n', 'f', 'o');
  EspEepromAdapter<EEPROMClass> eepromAdapter(EEPROM);
  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(Info)));
  CrcEeprom crcEeprom(eepromAdapter, contextId);

  Info info = {1, 2};
  crcEeprom.writeWithCrc(0, info);

  // Write directly into 'contextId' to invalidate it
  const uint32_t contextId2 = CrcEeprom::toContextId('t', 'e', 's', 't');
#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_DUINO_EPOXY_PROM_ESP)
  EEPROM.put(0, contextId2);
  EEPROM.commit();
#else
  EEPROM.put(0, contextId2);
#endif

  info = {0, 0};
  bool status = crcEeprom.readWithCrc(0, info);
  assertFalse(status);
}

// ---------------------------------------------------------------------------

void setup() {
#if defined(ARDUINO)
  delay(1000); // some boards reboot twice
#endif
  Serial.begin(115200);
  while (!Serial); // Leonardo/Micro

}

void loop() {
  TestRunner::run();
}
