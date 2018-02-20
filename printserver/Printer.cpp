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

#include "Printer.h"

Printer::Printer(String _printerId): queue(_printerId) {
  name = _printerId;
}

void Printer::init() {
  queue.init();
}

void Printer::startJob() {
}

void Printer::endJob() {
}

void Printer::startJob(int clientId) {
  if (status == IDLE) {
    status = PRINTING_FROM_SERVER;
    printingClientId = clientId;
    startJob();
  } else {
    queue.startJob(clientId);
  }
}

void Printer::endJob(int clientId, bool cancel) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    status = IDLE;
    endJob();
  } else {
    queue.endJob(clientId, cancel);
  }
}

bool Printer::canPrint(int clientId) {
  if (status == PRINTING_FROM_SERVER && printingClientId == clientId) {
    return canPrint();
  } else {
    return queue.canStoreByte();
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

String Printer::getName() {
  return name;
}
