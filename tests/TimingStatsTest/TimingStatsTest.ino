#line 2 "TimingStatsTest.ino"

#include <AUnit.h>
#include <TimingStats.h>

using namespace aunit;
using namespace timing_stats;

//----------------------------------------------------------------------------
// Test PrintString can handle strings >= 255 but <= 65534.
//----------------------------------------------------------------------------

test(TimingStatsTest, one) {
  TimingStats stats;
  stats.update(10);

  assertEqual(1, stats.getCount());
  assertEqual(10, stats.getMin());
  assertEqual(10, stats.getMax());
  assertEqual(10, stats.getAvg());
  //assertEqual(10, stats.getExpDecayAvg());
}

test(TimingStatsTest, two) {
  TimingStats stats;
  stats.update(10);
  stats.update(20);

  assertEqual(2, stats.getCount());
  assertEqual(10, stats.getMin());
  assertEqual(20, stats.getMax());
  assertEqual(15, stats.getAvg());
  //assertEqual(10, stats.getExpDecayAvg());
}


test(TimingStatsTest, three) {
  TimingStats stats;
  stats.update(10);
  stats.update(20);
  stats.update(30);

  assertEqual(3, stats.getCount());
  assertEqual(10, stats.getMin());
  assertEqual(30, stats.getMax());
  assertEqual(20, stats.getAvg());
  //assertEqual(10, stats.getExpDecayAvg());
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
