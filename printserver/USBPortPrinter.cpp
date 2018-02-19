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
  if(!printerPort.init()) return false;
  if(!ch375.setBaudRate(115200, [this](int localBaudRate){ch375stream.begin(115200);})) return false;
  isInitialized = true;
  return true;
}

void USBPortPrinter::flushBuffer() {
  if(ensureInitialized()) {
    if(printerPort.sendData(buffer, 64)) {
      Serial.println("OK, packet sent");
    } else {
      Serial.println("Failed to send USB packet!");
    }
  }
  bufferIndex = 0;
}

bool USBPortPrinter::canPrint() {
  return true;
}

void USBPortPrinter::printByte(byte b) {
  if (bufferIndex >= 64) {
    flushBuffer();
  }
  buffer[bufferIndex++] = b;
}

String USBPortPrinter::getInfo() {
  if (!ensureInitialized()) return "USB port printer - initialization failed";
  return "USB port printer, correctly intialized";
}
