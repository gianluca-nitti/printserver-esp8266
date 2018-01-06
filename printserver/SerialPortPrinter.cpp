#include "SerialPortPrinter.h"

SerialPortPrinter::SerialPortPrinter(Stream* s) {
  stream = s;
}

bool SerialPortPrinter::canPrint() {
  return true;
}

void SerialPortPrinter::printByte(byte b) {
  stream->write(b);
}

String SerialPortPrinter::getInfo() {
  return "Serial port printer";
}
