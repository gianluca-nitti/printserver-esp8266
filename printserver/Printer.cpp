#include "Printer.h"

Printer::Printer(String _printerId): queue(_printerId) {
}

void Printer::init() {
  queue.init();
}

bool Printer::canAcceptJob(int size) {
  return true; //TODO
}

void Printer::startJob(int clientId) {
  if (status == IDLE) {
    status = PRINTING_FROM_SERVER;
    printingClientId = clientId;
  } else {
    queue.startJob(clientId);
  }
}

void Printer::endJob(int clientId) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    status = IDLE;
  } else {
    queue.endJob(clientId);
  }
}

bool Printer::canPrint(int clientId) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    return canPrint();
  } else {
    return true;
  }
}

void Printer::printByte(int clientId, byte b) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    printByte(b);
  } else {
    queue.printByte(clientId, b);
  }
}

void Printer::processQueue() {
  if (status == PRINTING_FROM_QUEUE) {
    if (queue.hasData()) {
      if(canPrint()) {
        printByte(queue.readData());
      }
    } else {
      status = IDLE;
    }
  } else if (status == IDLE && queue.hasData()) {
    status = PRINTING_FROM_QUEUE;
  }
}
