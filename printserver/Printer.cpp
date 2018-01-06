#include "Printer.h"

bool Printer::canAcceptJob(int size) {
  return true; //TODO
}

void Printer::startJob(int clientId) {
  if (status == IDLE) {
    status = PRINTING_FROM_SERVER;
    printingClientId = clientId;
  } else {
    //TODO: call queue.startJob
  }
}

void Printer::endJob(int clientId) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    status = IDLE;
  } else {
    //TODO: call queue.endJob
  }
}

bool Printer::canPrint(int clientId) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    return canPrint();
  } else if (status == PRINTING_FROM_QUEUE) {
    return true; //TODO: ask queue object
  } else {
    return false;
  }
}

void Printer::printByte(int clientId, byte b) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    printByte(b);
  } else if (status == PRINTING_FROM_QUEUE) {
    //TODO
  }
}
