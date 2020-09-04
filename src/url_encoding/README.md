# Url Encoding

Quick and dirty implementation of the URL encoding used for HTML forms:

* See https://en.wikipedia.org/wiki/Percent-encoding and
* https://stackoverflow.com/questions/1634271.

Quick summary of Form URL encoding:

* Alpha numeric `[a-zA-Z0-9]` are preserved,
* Spaces are converted to `+`,
* all other characters are percent-encoded (`%` followed by the 2-digit
  hexademimal value of the character)

Two functions are provided:

* `void formUrlEncode(Print& output, const char* str);`
* `void formUrlDecode(Print& output, const char* str);`

These functions do *not* use the `String` class to avoid heap fragmentation
associated with numerous creation and deletion of small `String` objects.
Instead, these functions print to the `output` parameter implementing the
`Print` interface. The most convenient implementation of the `Print` interface
is the [PrintString](../print_string) class provided in the `AceUtils` library,
which allows the encoded or decoded string to be captured in memory. The
`PrintString` object can be converted into a normal c-string (`const char*`)
using the `getCstr()` method. It's also possible to pass a `Serial` object as
the `output` if that is more convenient.

## Usage

```C++
#include <Arduino.h>
#include <PrintString.h>
#include <UrlEncoding.h>

using namespace print_string;
using namespace url_encoding;

const char MESSAGE[] = "0aA %";

const char ENCODED_MESSAGE[] = "0aA+%25";

void encodeToSerial() {
  formUrlEncode(Serial, MESSAGE);
  Serial.println();
}

void encodeToMemory() {
  PrintString<20> printString;
  formUrlEncode(printString, MESSAGE);
  Serial.println(printString.getCstr());
}

void decodeToSerial() {
  formUrlDecode(Serial, ENCODED_MESSAGE);
  Serial.println();
}

void decodeToMemory() {
  PrintString<20> printString;
  formUrlDecode(printString, ENCODED_MESSAGE);
  Serial.println(printString.getCstr());
}

void setup() {
  Serial.begin(115200);
  ...
}

void loop() {
  ...
}
```

## References

Inspired by:

* https://github.com/TwilioDevEd/twilio_esp8266_arduino_example
* https://github.com/zenmanenergy/ESP8266-Arduino-Examples/blob/master/helloWorld_urlencoded/urlencode.ino.

Rewritten in the following way:

* Use the `PrintString` object instead of `String` to avoid repeated
allocation of small strings which causes heap fragmentation.
* Remove the calls to `yield()` on every iteration because this
implementation should be so efficient that `yield()` should not be necessary
for any reasonable sized strings. TODO(brian): Do some benchmarks for
large strings.
* Fix buffer overrun bug for decoding strings which are malformed.
* Apply consistent style and code formatting.
