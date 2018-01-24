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

#include "ParallelPortPrinter.h"

#define STROBE_DELAY 10

ParallelPortPrinter::ParallelPortPrinter(String _printerId, int _strobePin, int _busyPin): Printer(_printerId) {
  strobePin = _strobePin;
  busyPin = _busyPin;
  pinMode(busyPin, INPUT);
  pinMode(strobePin, OUTPUT);
  digitalWrite(strobePin, HIGH);
}

bool ParallelPortPrinter::canPrint() {
  return digitalRead(busyPin) == LOW;
}

void ParallelPortPrinter::printByte(byte b) {
  while(!canPrint()) {
    //shouldn't happen - the caller should not call this function if canWrite returned false
    Serial.println("Printer busy!");
    delay(100);
  }
  setDataBus(b);
  digitalWrite(strobePin, LOW);
  delayMicroseconds(STROBE_DELAY);
  digitalWrite(strobePin, HIGH);
}

String ParallelPortPrinter::getInfo() {
  //TODO: get meaningful info from the printer using IEEE 1284 nibble mode
  return "Parallel port printer";
}
