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
