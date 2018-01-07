#pragma once
#include "Printer.h"

class ParallelPortPrinter: public Printer {
  private:
    int strobePin;
    int busyPin;
  protected:
    ParallelPortPrinter(String _printerId, int _strobePin, int _busyPin);
    bool canPrint();
    void printByte(byte b);
    virtual void setDataBus(byte b) = 0;
  public:
    String getInfo();
};
