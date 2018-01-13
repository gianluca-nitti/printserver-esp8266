#pragma once
#include "ParallelPortPrinter.h"

class ShiftRegParallelPortPrinter: public ParallelPortPrinter {
  private:
    int dataPin;
    int clkPin;
    int latchPin;
  protected:
    void setDataBus(byte b);
  public:
    ShiftRegParallelPortPrinter(String _printerId, int _dataPin, int _clkPin, int _latchPin, int _strobePin, int _busyPin);
};
