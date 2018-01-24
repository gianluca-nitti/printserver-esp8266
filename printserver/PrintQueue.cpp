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

#include "PrintQueue.h"

size_t PrintQueue::availableFlashSpace = 0;

void PrintQueue::updateAvailableFlashSpace() {
  FSInfo fsinfo;
  SPIFFS.info(fsinfo);
  availableFlashSpace = fsinfo.totalBytes - fsinfo.usedBytes;
}

PrintQueue::PrintQueue(String _printerId) {
  printerId = _printerId;
}

void PrintQueue::init() {
  loadInfo();
  updateAvailableFlashSpace();
}

void PrintQueue::saveInfo() {
  File infoFile = SPIFFS.open(printerId, "w");
  infoFile.write(head);
  infoFile.write(tail);
  infoFile.close();
}

void PrintQueue::loadInfo() {
  if (SPIFFS.exists(printerId)) {
    File infoFile = SPIFFS.open(printerId, "r");
    head = infoFile.read();
    tail = infoFile.read();
    infoFile.close();
  } else {
    head = 0;
    tail = 0;
  }
}

void PrintQueue::startJob(int clientId) {
  head++;
  fileWriters[clientId] = SPIFFS.open(printerId + String(head), "w");
  saveInfo();
}

void PrintQueue::endJob(int clientId, bool cancel) {
  String fName = fileWriters[clientId].name();
  fileWriters[clientId].close();
  if (cancel) {
    if(!SPIFFS.remove(fName)) {
      Serial.println("Warning: failed to remove " + fName);
    }
  } else {
    SPIFFS.rename(fName, fName + "OK");
  }
}

bool PrintQueue::canStoreByte() {
  return availableFlashSpace > 4096; //4KiB margin
}

void PrintQueue::printByte(int clientId, byte b) {
  fileWriters[clientId].write(b);
  availableFlashSpace--;
}

bool PrintQueue::hasData() {
  if (fileReader && fileReader.available() > 0) {
    return true;
  } else {
    if (fileReader) {
      String fName = fileReader.name();
      fileReader.close();
      if(!SPIFFS.remove(fName)) {
        Serial.println("Warning: failed to remove " + fName);
      }
    }
    if (head == tail || SPIFFS.exists(printerId + String(tail + 1))) {
      return false;
    } else {
      tail++;
      fileReader = SPIFFS.open(printerId + String(tail) + "OK", "r");
      saveInfo();
      return fileReader.available() > 0;
    }
  }
}

byte PrintQueue::readData() {
  return fileReader.read();
}
