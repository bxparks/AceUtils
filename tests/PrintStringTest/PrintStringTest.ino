#line 2 "PrintStringTest.ino"

#include <AUnit.h>
#include <AceUtils.h>

using namespace aunit;
using namespace print_string;

//----------------------------------------------------------------------------
// Test PrintString can handle strings >= 255 but <= 65534.
//----------------------------------------------------------------------------

static const int BUF_SIZE = 300;
static char STRING[BUF_SIZE];

static void fillString() {
  for (size_t i = 0; i < BUF_SIZE; i++) {
    char c = (i % 10) + '0';
    STRING[i] = c;
  }
  STRING[BUF_SIZE - 1] = '\0';
}

static void truncateString(int length) {
  STRING[length] = '\0';
}

test(PrintStringTest, flush) {
  PrintString<10> printString;
  size_t n = printString.print('a');
  assertEqual((size_t) 1, n);
  assertEqual((size_t) 1, printString.length());

  printString.flush();
  assertEqual("", printString.getCstr());
}

test(PrintStringTest, print_underSized) {
  fillString();
  PrintString<310> printString;
  size_t n = printString.print(STRING);

  // Verify that the entire STRING got copied
  assertEqual((size_t) BUF_SIZE - 1, n);
  assertEqual((size_t) BUF_SIZE - 1, printString.length());
  assertEqual(STRING, printString.getCstr());
}

test(PrintStringTest, print_overSized) {
  fillString();
  PrintString<270> printString;
  size_t n = printString.print(STRING);

  // Verify that the STRING was truncated to 269
  assertEqual((size_t) 269, n);
  assertEqual((size_t) 269, printString.length());
  truncateString(269);
  assertEqual(STRING, printString.getCstr());
}

test(PrintStringTest, write_overSized) {
  fillString();
  PrintString<270> printString;
  size_t n = printString.write((const uint8_t*) STRING, BUF_SIZE - 1);

  assertEqual((size_t) 269, n);
  assertEqual((size_t) 269, printString.length());
  truncateString(269);
  assertEqual(STRING, printString.getCstr());
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
