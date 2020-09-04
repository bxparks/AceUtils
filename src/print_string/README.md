# Print String

An implementation of `Print` that writes to an in-memory buffer supporting
strings less than 65535 in length. It is intended to be an alternative to the
`String` class to help avoid heap fragmentation due to repeated creation and
deletion of small String objects. The 'PrintString' object inherit the methods
from the 'Print' interface which can be used to build an internal string
representation of various objects. Instead of using the `operator+=()` or the
`concat()` method, use the `print()`, `println()` (or sometimes the `printf()`
method) of the `Print` class. After the internal string is built, the
NUL-terminated c-string representation can be retrieved using `getCstr()`.

## Usage

```C++
#include <Arduino.h>
#include <PrintString.h>

using namesapce print_string;

void buildMessage(PrintStringBase& message) {
  if (message.length() > 0) {
    message.print("; ");
  }
  message.print("x=");
  message.print(3);
  message.print("; s=")
  message.print("a-string");
}

void doSomething(const char* message) {
  // Do something with message
}

void doStuff() {
  PrintString<30> message;
  buildMessage(message);

  const char* cstr = message.getCstr();
  doSomething(cstr);

  message.flush();
  buildMessage(message);
  ...
}
```
