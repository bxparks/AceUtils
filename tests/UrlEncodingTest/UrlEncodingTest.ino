#line 2 "UrlEncodingTest.ino"

#include <AUnit.h>
#include <PrintString.h>
#include <UrlEncoding.h>

using namespace aunit;
using namespace print_string;
using namespace url_encoding;

//----------------------------------------------------------------------------
// Test formUrlEncode()
//----------------------------------------------------------------------------

test(formUrlEncode, normal) {
  PrintString<10> printString;
  formUrlEncode(printString, "0aA %");
  assertEqual("0aA+%25", printString.getCstr());
}

test(formUrlEncode, tooLong) {
  PrintString<10> printString;
  // truncated to 9 characters because PrintString is only 10-characters big
  formUrlEncode(printString, "0aA %0123456");
  assertEqual("0aA+%2501", printString.getCstr());
}

//----------------------------------------------------------------------------
// Test formUrlDecode()
//----------------------------------------------------------------------------

test(formUrlDecode, normal) {
  PrintString<10> printString;
  formUrlDecode(printString, "0aA+%25");
  assertEqual("0aA %", printString.getCstr());
}

test(formUrlDecode, truncated1) {
  PrintString<10> printString;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(printString, "0aA+%2");
  assertEqual("0aA ", printString.getCstr());
}

test(formUrlDecode, truncated2) {
  PrintString<10> printString;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(printString, "0aA+%");
  assertEqual("0aA ", printString.getCstr());
}

test(formUrlDecode, invalidHex) {
  PrintString<10> printString;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(printString, "0aA+%0Z");
  assertEqual("0aA ", printString.getCstr());
}

//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

void setup() {
#ifndef UNIX_HOST_DUINO
  delay(1000); // wait for stability on some boards to prevent garbage Serial
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while(!SERIAL_PORT_MONITOR); // for the Arduino Leonardo/Micro only
}

void loop() {
  aunit::TestRunner::run();
}
