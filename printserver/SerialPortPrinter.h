#pragma once
#include "Printer.h"

class SerialPortPrinter: public Printer {
  private:
    Stream* stream;
  protected:
    bool canPrint();
    void printByte(byte b);
  public:
    SerialPortPrinter(String _printerId, Stream* s);
    String getInfo();
};
