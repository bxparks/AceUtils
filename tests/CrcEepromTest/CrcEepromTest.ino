#line 2 "CrcEepromTest.ino"

#include <Arduino.h>
#include <AUnitVerbose.h>
#include <AceUtilsCrcEeprom.h>

using aunit::TestRunner;
using ace_utils::crc_eeprom::CrcEeprom;
using ace_utils::crc_eeprom::EspStyleEeprom;
using ace_utils::crc_eeprom::AvrStyleEeprom;

#if defined(EPOXY_DUINO)
  #include <EpoxyEepromEsp.h>
  EspStyleEeprom<EpoxyEepromEsp> eepromInterface(EpoxyEepromEspInstance);
#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  EspStyleEeprom<EEPROMClass> eepromInterface(EEPROM);
#elif defined(ARDUINO_ARCH_STM32)
  #include <AceUtilsBufferedEepromStm32.h>
  EspStyleEeprom<BufferedEEPROMClass> eepromInterface(BufferedEEPROM);
#else
  // Assume AVR
  #include <EEPROM.h>
  AvrStyleEeprom<EEPROMClass> eepromInterface(EEPROM);
#endif

struct Info {
  int startTime;
  int interval;
};

CrcEeprom crcEeprom(
    eepromInterface, CrcEeprom::toContextId('c', 't', 's', 't'));

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
  const uint32_t contextId2 = CrcEeprom::toContextId('t', 'e', 's', 't');
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
