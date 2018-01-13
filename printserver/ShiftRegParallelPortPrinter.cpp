#include "ShiftRegParallelPortPrinter.h"

ShiftRegParallelPortPrinter::ShiftRegParallelPortPrinter(String _printerId, int _dataPin, int _clkPin, int _latchPin, int _strobePin, int _busyPin): ParallelPortPrinter(_printerId, _strobePin, _busyPin) {
  dataPin = _dataPin;
  clkPin = _clkPin;
  latchPin = _latchPin;
  pinMode(dataPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void ShiftRegParallelPortPrinter::setDataBus(byte b) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clkPin, MSBFIRST, b);
  digitalWrite(latchPin, HIGH);
}
