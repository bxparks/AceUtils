#line 2 "UrlEncodingTest.ino"

#include <AUnit.h>
#include <AceUtils.h>

using namespace aunit;
using namespace print_string;
using namespace url_encoding;

//----------------------------------------------------------------------------
// Test formUrlEncode()
//----------------------------------------------------------------------------

test(formUrlEncode, normal) {
  PrintString<10> memPrint;
  formUrlEncode(memPrint, "0aA %");
  assertEqual("0aA+%25", memPrint.getCstr());
}

test(formUrlEncode, tooLong) {
  PrintString<10> memPrint;
  // truncated to 9 characters because PrintString is only 10-characters big
  formUrlEncode(memPrint, "0aA %0123456");
  assertEqual("0aA+%2501", memPrint.getCstr());
}

//----------------------------------------------------------------------------
// Test formUrlDecode()
//----------------------------------------------------------------------------

test(formUrlDecode, normal) {
  PrintString<10> memPrint;
  formUrlDecode(memPrint, "0aA+%25");
  assertEqual("0aA %", memPrint.getCstr());
}

test(formUrlDecode, truncated1) {
  PrintString<10> memPrint;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(memPrint, "0aA+%2");
  assertEqual("0aA ", memPrint.getCstr());
}

test(formUrlDecode, truncated2) {
  PrintString<10> memPrint;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(memPrint, "0aA+%");
  assertEqual("0aA ", memPrint.getCstr());
}

test(formUrlDecode, invalidHex) {
  PrintString<10> memPrint;
  // Missing 2-digit of hex after %, which is ignored.
  formUrlDecode(memPrint, "0aA+%0Z");
  assertEqual("0aA ", memPrint.getCstr());
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
