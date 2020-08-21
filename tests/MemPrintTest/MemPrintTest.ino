#line 2 "MemPrintTest.ino"

#include <AUnit.h>
#include <AceUtils.h>

using namespace aunit;
using namespace mem_print;

//----------------------------------------------------------------------------
// MemPrint can handle strings <= 254 in length
//----------------------------------------------------------------------------

test(MemPrintTest, flush) {
  MemPrint<10> memPrint;
  size_t n = memPrint.print('a');
  assertEqual((size_t) 1, n);

  memPrint.flush();
  assertEqual("", memPrint.getCstr());
}

test(MemPrintTest, print_underSized) {
  MemPrint<10> memPrint;
  size_t n = memPrint.print("012345678");

  assertEqual((size_t) 9, n);
  assertEqual((size_t) 9, memPrint.length());
  assertEqual("012345678", memPrint.getCstr());
}

test(MemPrintTest, print_overSized) {
  MemPrint<10> memPrint;
  size_t n = memPrint.print("0123456789");

  assertEqual((size_t) 9, n);
  assertEqual((size_t) 9, memPrint.length());
  assertEqual("012345678", memPrint.getCstr());
}

test(MemPrintTest, write_overSized) {
  MemPrint<10> memPrint;
  size_t n = memPrint.write((const uint8_t*) "0123456789", 10);

  assertEqual((size_t) 9, n);
  assertEqual((size_t) 9, memPrint.length());
  assertEqual("012345678", memPrint.getCstr());
}

//----------------------------------------------------------------------------
// Test MemPrintLarge can handle strings >= 255 but <= 65534.
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

test(MemPrintLargeTest, flush) {
  MemPrintLarge<10> memPrint;
  size_t n = memPrint.print('a');
  assertEqual((size_t) 1, n);

  memPrint.flush();
  assertEqual("", memPrint.getCstr());
}

test(MemPrintLargeTest, print_underSized) {
  fillString();
  MemPrintLarge<310> memPrint;
  size_t n = memPrint.print(STRING);

  // Verify that the entire STRING got copied
  assertEqual((size_t) BUF_SIZE - 1, n);
  assertEqual((size_t) BUF_SIZE - 1, memPrint.length());
  assertEqual(STRING, memPrint.getCstr());
}

test(MemPrintLargeTest, print_overSized) {
  fillString();
  MemPrintLarge<270> memPrint;
  size_t n = memPrint.print(STRING);

  // Verify that the STRING was truncated to 269
  assertEqual((size_t) 269, n);
  assertEqual((size_t) 269, memPrint.length());
  truncateString(269);
  assertEqual(STRING, memPrint.getCstr());
}

test(MemPrintLargeTest, write_overSized) {
  fillString();
  MemPrintLarge<270> memPrint;
  size_t n = memPrint.write((const uint8_t*) STRING, BUF_SIZE - 1);

  assertEqual((size_t) 269, n);
  assertEqual((size_t) 269, memPrint.length());
  truncateString(269);
  assertEqual(STRING, memPrint.getCstr());
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
