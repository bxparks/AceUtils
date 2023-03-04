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
#include <AceUtils.h>
#include <crc_eeprom/crc_eeprom.h>
using ace_utils::crc_eeprom::toContextId;
using ace_utils::crc_eeprom::CrcEepromAvr;
using ace_utils::crc_eeprom::CrcEepromEsp;

// The contextId can be anything you want, and should uniquely identify the
// application to avoid collisions with another applications that store data
// with the same length.
const uint32_t CONTEXT_ID = 0x664cb683;

#if defined(EPOXY_DUINO)
  #include <EpoxyEepromEsp.h>
  CrcEepromEsp<EpoxyEepromEsp> crcEeprom(EspStyleEepromInstance, CONTEXT_ID);

#elif defined(ESP8266) || defined(ESP32)
  #include <EEPROM.h>
  CrcEepromEsp<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);

#elif defined(ARDUINO_ARCH_STM32)

  // Use this for STM32. The 'buffered_eeprom_stm32' library provides
  // the BufferedEEPROM object which internally uses the buffered versions of
  // the low-level eeprom functions. The BufferedEEPROM object implements the
  // ESP-flavored EEPROM API.
  #include <AceUtils.h>
  #include <buffered_eeprom_stm32/buffered_eeprom_stm32.h>
  CrcEepromEsp<BufferedEEPROMClass> crcEeprom(BufferedEEPROM, CONTEXT_ID);

  // Don't do this for STM32 because the default EEPROM flashes the entire
  // page for *every* byte!
  //#include <EEPROM.h>
  //CrcEepromAvr<EEPROMClass> crcEeprom(EEPROM, CONTEXT_ID);

#else
  // Assume AVR-style EEPROM for all other cases.
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
  [...]
};

StoredInfo storedInfo;

const uint16_t EEPROM_ADDRESS = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupEeprom();

  // Write to EEPROM w/ CRC and contextId check.
  size_t writtenSize = crcEeprom.writeWithCrc(EEPROM_ADDRESS, storedInfo);
  if (!writtenSize) {
    Serial.println("Error writing to EEPROM");
  }

  // Read from EEPROM w/ CRC and contextId check.
  bool status = crcEeprom.readWithCrc(EEPROM_ADDRESS, storedInfo);
  if (!status) {
    Serial.println("Error reading from EEPROM");
  }
}

void loop() {
}
```

### Context ID

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

### Template Classes

A previous version of this used a `EepromInterface` pure abstract class that
provides a stable API to the `CrcEeprom` class. Two subclasses were
`AvrStyleEeprom` and `EspStyleEeprom`. Converting `CrcEeprom` to a template
class, templatized on `AvrStyleEeprom` and `EspStyleEeprom` eliminates the
virtual function calls, and seems to save between 150 to 950 bytes of flash
memory on AVR processors. On small processors, like the ATtiny85, this seemed
like an optimization worth making.

The complexity of `CrcEeprom<>` has been reduced by creating the following
helper classes:

* `CrcEepromAvr<E>`
* `CrcEepromEsp<E>`

New EEPROM implementations can be with with `CrcEeprom` by creating a new
instance of the `EepromInterface`, then using the raw `CrcEeprom` template
class.
