#include "PrintQueue.h"

PrintQueue::PrintQueue(String _printerId) {
  printerId = _printerId;
}

void PrintQueue::init() {
  loadInfo();
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

void PrintQueue::endJob(int clientId) {
  fileWriters[clientId].close();
}

void PrintQueue::printByte(int clientId, byte b) {
  fileWriters[clientId].write(b);
}

bool PrintQueue::hasData() {
  if (fileReader && fileReader.available() > 0) {
    return true;
  } else {
    if (fileReader) {
      String fName = fileReader.name();
      fileReader.close();
      //SPIFFS.remove(fName); //non qua, potrebbe essere ancora aperto in scrittura
    }
    if (head == tail) {
      return false;
    } else {
      tail++;
      fileReader = SPIFFS.open(printerId + String(tail), "r");
      saveInfo();
      return fileReader.available() > 0;
    }
  }
}

byte PrintQueue::readData() {
  return fileReader.read();
}
