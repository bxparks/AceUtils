#include <Arduino.h>
#include <TimingStats.h>

using namespace timing_stats;

TimingStats stats;

void collectData() {
  stats.update(10);
  stats.update(20);
  stats.update(30);
}

void printStats() {
  Serial.print("Min: ");
  Serial.println(stats.getMin());

  Serial.print("Max: ");
  Serial.println(stats.getMax());

  Serial.print("Avg: ");
  Serial.println(stats.getAvg());

  Serial.print("Exp Decay Avg: ");
  Serial.println(stats.getExpDecayAvg());

  Serial.print("Count: ");
  Serial.println(stats.getCount());
}

void setup() {
  Serial.begin(115200);
  while (! Serial); // Leonardo/ProMicro

  collectData();
  printStats();

  #ifdef UNIX_HOST_DUINO
    exit(0);
  #endif
}

void loop() {
}
