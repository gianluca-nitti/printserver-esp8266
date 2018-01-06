#include "ParallelPortPrinter.h"

#define STROBE_DELAY 10

ParallelPortPrinter::ParallelPortPrinter(int _strobePin, int _busyPin) {
  strobePin = _strobePin;
  busyPin = _busyPin;
  pinMode(busyPin, INPUT);
  pinMode(strobePin, OUTPUT);
  digitalWrite(strobePin, HIGH);
}

bool ParallelPortPrinter::canPrint() {
  return digitalRead(busyPin) == LOW;
}

void ParallelPortPrinter::printByte(byte b) {
  while(!canPrint()) {
    //shouldn't happen - the caller should not call this function if canWrite returned false
    Serial.println("Printer busy!");
    delay(100);
  }
  setDataBus(b);
  digitalWrite(strobePin, LOW);
  delayMicroseconds(STROBE_DELAY);
  digitalWrite(strobePin, HIGH);
}

String ParallelPortPrinter::getInfo() {
  //TODO: get meaningful info from the printer using IEEE 1284 nibble mode
  return "Parallel port printer";
}
