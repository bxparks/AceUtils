#include <Arduino.h>
#include <PrintString.h>
#include <UrlEncoding.h>
#include <TimingStats.h>
#include "url_coding.hpp"

using namespace print_string;
using namespace url_encoding;
using namespace timing_stats;

const int NUM_SAMPLES = 20;

#if defined(__linux__) || defined(__APPLE__)
  const unsigned FORM_URL_ENCODE_SIZES[] = {1000, 2000, 4000, 8000};
  const unsigned URL_ENCODE_NO_YIELD_SIZES[] = {1000, 2000, 4000, 8000};
  const unsigned URL_ENCODE_YIELD_SIZES[] = {5, 10, 20, 40};
#elif defined(ESP8266)
  const unsigned FORM_URL_ENCODE_SIZES[] = {1000, 2000, 4000, 8000};
  const unsigned URL_ENCODE_NO_YIELD_SIZES[] = {1000, 2000, 4000, 6000};
  const unsigned URL_ENCODE_YIELD_SIZES[] = {100, 200, 400, 800};
#else
  #error Unsupported microcontroller
#endif

// A volatile variable that's updated at the end of the benchmark routine
// to prevent the compiler from optimizing away the entire calculation because
// it determines that there is no side-effect.
volatile uint8_t guard;

/** Create a random message of length size. */
void createMessage(Print& message, uint16_t size) {
  for (uint16_t i = 0; i < size - 1; i++) {
    long ran = random(127 - 32);
    char c = ran + 32;
    message.write(c);
  }
}

//----------------------------------------------------------------------------
// formUrlEncode()
//----------------------------------------------------------------------------

void benchmarkFormUrlEncode(TimingStats& stats, const char* message) {
  size_t messageLength = strlen(message);
  PrintStringN printString(2 * messageLength);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    formUrlEncode(printString, message);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = printString.length();
    stats.update(elapsed);
  }
}

void printFormUrlEncodeTime(uint16_t size) {
  yield();
  PrintStringN message(size);
  createMessage(message, size);
  TimingStats stats;
  benchmarkFormUrlEncode(stats, message.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "formUrlEncode(%4u)       | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// formUrlDecode()
//----------------------------------------------------------------------------

void benchmarkFormUrlDecode(TimingStats& stats, const char* encoded) {
  size_t encodedLength = strlen(encoded);
  PrintStringN printString(encodedLength);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    formUrlDecode(printString, encoded);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = printString.length();
    stats.update(elapsed);
  }
}

void printFormUrlDecodeTime(uint16_t size) {
  // Create a random message
  PrintStringN message(size);
  yield();
  createMessage(message, size);

  // encode it
  PrintStringN encoded(2 * size);
  yield();
  formUrlEncode(encoded, message.getCstr());

  TimingStats stats;
  benchmarkFormUrlDecode(stats, encoded.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "formUrlDecode(%4u)       | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// urlencode_yield()
//----------------------------------------------------------------------------

void benchmarkUrlEncodeYield(TimingStats& stats, const char* message) {
  String msg(message);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    String encoded = urlencode_yield(msg);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = encoded.length();
    stats.update(elapsed);
  }
}

void printUrlEncodeYieldTime(uint16_t size) {
  yield();
  PrintStringN message(size);
  createMessage(message, size);
  TimingStats stats;
  benchmarkUrlEncodeYield(stats, message.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "urlencode_yield(%4u)     | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// urldecode_yield()
//----------------------------------------------------------------------------

void benchmarkUrlDecodeYield(TimingStats& stats, const char* encoded) {
  String encMsg(encoded);
  size_t encodedLength = strlen(encoded);
  PrintStringN printString(encodedLength);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    String decoded = urldecode_yield(encMsg);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = decoded.length();
    stats.update(elapsed);
  }
}

void printUrlDecodeYieldTime(uint16_t size) {
  // Create a random message
  PrintStringN message(size);
  yield();
  createMessage(message, size);

  // encode it
  PrintStringN encoded(2 * size);
  yield();
  formUrlEncode(encoded, message.getCstr());

  TimingStats stats;
  benchmarkUrlDecodeYield(stats, encoded.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "urldecode_yield(%4u)     | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// urlencode_no_yield()
//----------------------------------------------------------------------------

void benchmarkUrlEncodeNoYield(TimingStats& stats, const char* message) {
  String msg(message);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    String encoded = urlencode_no_yield(msg);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = encoded.length();
    stats.update(elapsed);
  }
}

void printUrlEncodeNoYieldTime(uint16_t size) {
  yield();
  PrintStringN message(size);
  createMessage(message, size);
  TimingStats stats;
  benchmarkUrlEncodeNoYield(stats, message.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "urlencode_no_yield(%4u)  | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// urldecode_no_yield()
//----------------------------------------------------------------------------

void benchmarkUrlDecodeNoYield(TimingStats& stats, const char* encoded) {
  String encMsg(encoded);
  size_t encodedLength = strlen(encoded);
  PrintStringN printString(encodedLength);
  yield();

  for (int i = 0; i < NUM_SAMPLES; i++) {
    uint16_t nowMicros = micros();
    String decoded = urldecode_no_yield(encMsg);
    uint16_t elapsed = micros() - nowMicros;

    yield();
    guard = decoded.length();
    stats.update(elapsed);
  }
}

void printUrlDecodeNoYieldTime(uint16_t size) {
  // Create a random message
  PrintStringN message(size);
  yield();
  createMessage(message, size);

  // encode it
  PrintStringN encoded(2 * size);
  yield();
  formUrlEncode(encoded, message.getCstr());

  TimingStats stats;
  benchmarkUrlDecodeNoYield(stats, encoded.getCstr());
  uint16_t perMil = (stats.getAvg() * 1000L) / size;

  SERIAL_PORT_MONITOR.printf(
      "urldecode_no_yield(%4u)  | %6u | %6u | %6u | %6u |\n",
      size, stats.getAvg(), perMil, stats.getMin(), stats.getMax());
}

//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

void setup() {
#if ! defined(UNIX_HOST_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until ready - Leonardo/Micro

  SERIAL_PORT_MONITOR.print("sizeof(PrintStringBase) = ");
  SERIAL_PORT_MONITOR.println(sizeof(PrintStringBase));
  SERIAL_PORT_MONITOR.print("sizeof(PrintStringN) = ");
  SERIAL_PORT_MONITOR.println(sizeof(PrintStringN));
  SERIAL_PORT_MONITOR.print("sizeof(PrintString<8>) = ");
  SERIAL_PORT_MONITOR.println(sizeof(PrintString<8>));


  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  SERIAL_PORT_MONITOR.println(
      F("Description               | micros | us/1000|    min |    max |"));

  // formUrlEncode()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(FORM_URL_ENCODE_SIZES) / sizeof(unsigned);
      i++) {
    printFormUrlEncodeTime(FORM_URL_ENCODE_SIZES[i]);
  }

  // formUrlDecode()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(FORM_URL_ENCODE_SIZES) / sizeof(unsigned);
      i++) {
    printFormUrlDecodeTime(FORM_URL_ENCODE_SIZES[i]);
  }

  // urlencode_no_yield()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(URL_ENCODE_NO_YIELD_SIZES) / sizeof(unsigned);
      i++) {
    printUrlEncodeNoYieldTime(URL_ENCODE_NO_YIELD_SIZES[i]);
  }

  // urldecode_no_yield()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(URL_ENCODE_NO_YIELD_SIZES) / sizeof(unsigned);
      i++) {
    printUrlDecodeNoYieldTime(URL_ENCODE_NO_YIELD_SIZES[i]);
  }

  // urlencode_yield()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(URL_ENCODE_YIELD_SIZES) / sizeof(unsigned);
      i++) {
    printUrlEncodeYieldTime(URL_ENCODE_YIELD_SIZES[i]);
  }

  // urldecode_yield()
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  for (unsigned i = 0; i < sizeof(URL_ENCODE_YIELD_SIZES) / sizeof(unsigned);
      i++) {
    printUrlDecodeYieldTime(URL_ENCODE_YIELD_SIZES[i]);
  }

  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+--------+"));
  SERIAL_PORT_MONITOR.print("Num iterations: ");
  SERIAL_PORT_MONITOR.println(NUM_SAMPLES);

#if defined(UNIX_HOST_DUINO)
  exit(0);
#endif
}

void loop() {
}
