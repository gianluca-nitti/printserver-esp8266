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

#pragma once
#include <CH375USBPrinter.h>
#include <CH375.h>
#include "Printer.h"

class USBPortPrinter: public Printer {
  private:
    CH375 ch375;
    CH375USBPrinter printerPort;
    bool isInitialized = false;;
    byte buffer[64]; //TODO: retrieve size from library and initialize dinamically on heap
    byte bufferIndex = 0;
    bool ensureInitialized();
    void flushBuffer();
  protected:
    bool canPrint();
    void printByte(byte b);
  public:
    USBPortPrinter(String _printerId, Stream& ch375stream, int ch375IntPin);
    String getInfo();
};
