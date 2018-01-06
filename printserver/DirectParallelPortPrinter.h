#pragma once
#include "ParallelPortPrinter.h"

class DirectParallelPortPrinter: public ParallelPortPrinter {
  private:
    int dataPins[8];
  protected:
    void setDataBus(byte b);
  public:
    DirectParallelPortPrinter(int _dataPins[8], int _strobePin, int _busyPin);
};
