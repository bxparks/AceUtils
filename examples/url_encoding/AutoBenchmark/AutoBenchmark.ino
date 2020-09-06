#include <Arduino.h>
#include <PrintString.h>
#include <UrlEncoding.h>
#include <TimingStats.h>

using namespace print_string;
using namespace url_encoding;
using namespace timing_stats;

const int NUM_SAMPLES = 20;

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

void benchmarkEncode(TimingStats& stats, const char* message) {
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

void printEncodingTime(uint16_t size) {
  yield();
  PrintStringN message(size);
  createMessage(message, size);
  TimingStats stats;
  benchmarkEncode(stats, message.getCstr());

  SERIAL_PORT_MONITOR.printf(
      "formUrlEncode(%4u)       | %6u | %6u | %6u |\n", 
      size, stats.getAvg(), stats.getMin(), stats.getMax());
}

void benchmarkDecode(TimingStats& stats, const char* encoded) {
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

void printDecodingTime(uint16_t size) {
  // Create a random message
  PrintStringN message(size);
  yield();
  createMessage(message, size);

  // encode it
  PrintStringN encoded(2 * size);
  yield();
  formUrlEncode(encoded, message.getCstr());

  TimingStats stats;
  benchmarkDecode(stats, encoded.getCstr());

  SERIAL_PORT_MONITOR.printf(
      "formUrlDecode(%4u)       | %6u | %6u | %6u |\n", 
      size, stats.getAvg(), stats.getMin(), stats.getMax());
}

void setup() {
#if ! defined(UNIX_HOST_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until ready - Leonardo/Micro

  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+"));
  SERIAL_PORT_MONITOR.println(
      F("Description               | micros |    min |    max |"));
  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+"));
  printEncodingTime(1000);
  printEncodingTime(2000);
  printEncodingTime(4000);
  printEncodingTime(8000);

  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+"));
  printDecodingTime(1000);
  printDecodingTime(2000);
  printDecodingTime(4000);
  printDecodingTime(8000);

  SERIAL_PORT_MONITOR.println(
      F("--------------------------+--------+--------+--------+"));
  SERIAL_PORT_MONITOR.print("Num iterations: ");
  SERIAL_PORT_MONITOR.println(NUM_SAMPLES);

#if defined(UNIX_HOST_DUINO)
  exit(0);
#endif
}

void loop() {
}
