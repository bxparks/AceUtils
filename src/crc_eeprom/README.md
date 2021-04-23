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
using ace_utils::crc_eeprom::AvrStyleEeprom;
using ace_utils::crc_eeprom::EspStyleEeprom;

#if defined(EPOXY_DUINO)
  #include <EpoxyEepromEsp.h>
  EspStyleEeprom<EpoxyEepromEsp> eeprom(EspStyleEepromInstance);

#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  EspStyleEeprom<EEPROMClass> eepromInterface(EEPROM);

#elif defined(ARDUINO_ARCH_STM32)

  // Use this for STM32. The <AceUtilsBufferedEepromStm32.h> library provides
  // the BufferedEEPROM object which internally uses the buffered versions of
  // the low-level eeprom functions. The BufferedEEPROM object implements the
  // ESP-flavored EEPROM API.
  #include <AceUtilsBufferedEepromStm32.h>
  EspStyleEeprom<BufferedEEPROMClass> eepromInterface(BufferedEEPROM);

  // Don't do this for STM32 because the default EEPROM flashes the entire
  // page for *every* byte!
  //#include <EEPROM.h>
  //AvrStyleEeprom<EEPROMClass> eepromInterface(EEPROM);

#else
  // Assume AVR-style EEPROM for all other cases.
  #include <EEPROM.h>
  AvrStyleEeprom<EEPROMClass> eepromInterface(EEPROM);

#endif

// The contextId should be some random 32-bit integer.
CrcEeprom crcEeprom(
    eepromInterface, CrcEeprom::toContextId('d', 'e', 'm', 'o'));

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
  [...]
};

StoredInfo storedInfo;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupEeprom();

  // Write to EEPROM w/ CRC and contextId check.
  size_t writtenSize = crcEeprom.writeWithCrc(0, storedInfo);
  if (!writtenSize) {
    Serial.println("Error writing to EEPROM");
  }

  // Read from EEPROM w/ CRC and contextId check.
  bool status = crcEeprom.readWithCrc(0, storedInfo);
  if (!status) {
    Serial.println("Error reading from EEPROM");
  }
}

void loop() {
}
```

The `contextId` is a 32-bit identifier that is designed to help collisions
between 2 applications which store different data, but with the **same** size.
Since the size of the data is the same, the CRC will be valid, but the 2
applications will certain not have compatible data. The `writeWithCrc()` writes
this `contextId` into the EEPROM, just like the CRC. The `readWithCrc()`
verifies that the `contextId` matches, just like the CRC.

You can generate the `contextId` in using any method. I wrote the
`CrcEeprom::toContextId()` helper function to convert 4-letter sequences (e.g.
'o', 'c', 'l', 'k') into a 32-bit number. But I found it difficult to create new
4-letter combos which don't collide with each other.

I now recommend that you simply use a 32-bit random number generator to get
the `contextId`. Here are some sources:

* Linux
    * `$ od -v -An -N4 -t x4 /dev/urandom`
    * You have to add the "0x" prefix manually.
* Bash
    * `$ printf "0x%08x\n" $(($RANDOM * 65536 + $RANDOM))`
    * The range of `$RANDOM` is only 15-bits, so this generates only a 30-bit
      random number instead of 32. This may be good enough for you.
* Python
    * `$ python3 -c 'import random; print(f"0x{random.randint(0,2**32-1):08x}")`
* Perl
    * `$ perl -e 'printf("0x%08x\n", int(rand(2**32)))'`
* Awk
    * `$ awk 'BEGIN{ srand(); printf("0x%08x\n", int(rand() * 2**32)) }'`
    * If you don't use `srand()`, you get the same number every time you run
      this.
    * If you use `srand()` within the same second, you will get the same number.
* Online web generators
    * It was actually difficult to find online random generators for this
      purpose. Here are some:
    * https://onlinerandomtools.com/generate-random-hexadecimal-numbers
    * https://numbergenerator.org/hex-code-generator
