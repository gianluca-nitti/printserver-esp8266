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
#include "PrintQueue.h"

typedef enum {
  IDLE,
  PRINTING_FROM_SERVER,
  PRINTING_FROM_QUEUE
} printer_status;

class Printer {
  private:
    printer_status status = IDLE;
    int printingClientId = 0;
    PrintQueue queue;
    String name;
  protected:
    Printer(String _printerId);
    // startJob() and endJob() do nothing by default, and can be overriden if a specifica
    // type of printer port has specific tasks to do before or after a print job
    virtual void startJob();
    virtual void endJob();
    virtual bool canPrint() = 0;
    virtual void printByte(byte b) = 0;
  public:
    void init();
    void startJob(int clientId);
    void endJob(int clientId, bool cancel);
    bool canPrint(int clientId);
    void printByte(int clientId, byte b);
    void processQueue();
    String getName();
    virtual String getInfo() = 0;
};
