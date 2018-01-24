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
#include <Arduino.h>
#include <FS.h>
#include "Settings.h"

class PrintQueue {
  private:
    static size_t availableFlashSpace;

    String printerId;
    File fileWriters[MAXCLIENTS];
    File fileReader;
    byte head;
    byte tail;
    void saveInfo();
    void loadInfo();
  public:
    static void updateAvailableFlashSpace();

    PrintQueue(String _printerId);
    void init();
    void startJob(int clientId);
    void endJob(int clientId, bool cancel);
    bool canStoreByte();
    void printByte(int clientId, byte b);
    bool hasData();
    byte readData();
};
