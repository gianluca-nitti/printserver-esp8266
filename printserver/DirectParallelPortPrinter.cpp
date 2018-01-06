#include "DirectParallelPortPrinter.h"

DirectParallelPortPrinter::DirectParallelPortPrinter(int _dataPins[8], int _strobePin, int _busyPin): ParallelPortPrinter(_strobePin, _busyPin) {
  for (int i = 0; i < 8; i++) {
    dataPins[i] = _dataPins[i];
    pinMode(dataPins[i], OUTPUT);
  }
}

void DirectParallelPortPrinter::setDataBus(byte b) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(dataPins[i], bitRead(b, i));
  }
}
