#include "SerialPortPrinter.h"

SerialPortPrinter::SerialPortPrinter(String _printerId, Stream* s): Printer(_printerId) {
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
