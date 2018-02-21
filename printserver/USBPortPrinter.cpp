/*
    This file is part of printserver-esp8266.

    printserver-esp8266 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    printserver-esp8266 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with printserver-esp8266.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "USBPortPrinter.h"

USBPortPrinter::USBPortPrinter(String _printerId, SoftwareSerial& _ch375stream, int ch375IntPin): Printer(_printerId), ch375stream(_ch375stream), ch375(ch375stream, ch375IntPin), printerPort(ch375), isInitialized(false) {
  pinMode(ch375IntPin, INPUT);
}

bool USBPortPrinter::ensureInitialized() {
  if (isInitialized) return true;
  if(!ch375.init()) return false;
  if(!ch375.setBaudRate(100000, [this](){ch375stream.begin(100000);})) return false;
  if(!printerPort.init()) return false;
  isInitialized = true;
  return true;
}

void USBPortPrinter::startJob() {
  ensureInitialized();
}

void USBPortPrinter::endJob() {
  printerPort.flush();
}

bool USBPortPrinter::canPrint() {
  return true;
}

void USBPortPrinter::printByte(byte b) {
  printerPort.write(b);
}

String USBPortPrinter::getInfo() {
  if (!ensureInitialized()) return "USB port printer - initialization failed";
  return "USB port printer, correctly intialized";
}
