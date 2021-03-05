# CRC EEPROM

Save data to EEPROM along with a CRC32 value, and an optional `contextId`. When
the data is retrieved, the CRC32 is calculated and compared with the expected
CRC32. The `contextId` is also compared with the expected contextId to verify
that the data came from the same application.

## Examples

* [examples/CrcEepromDemo](../../examples/CrcEepromDemo)

## Usage

```C++
#include <Arduino.h>
#include <AceUtilsCrcEeprom.h>
using ace_utils::crc_eeprom::AvrEepromAdapter;
using ace_utils::crc_eeprom::EspEepromAdapter;

#if defined(ESP8266) || defined(ESP32)
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
  [...]
};

StoredInfo storedInfo;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  eepromAdapter.begin(CrcEeprom::toSavedSize(sizeof(StoredInfo)));

  size_t writtenSize = crcEeprom.writeWithCrc(0, info);

  ...

  bool status = crcEeprom.readWithCrc(0, info);
  if (!status) {
    Serial.println("Error reading EEPROM");
  }
}

void loop() {
}
```
