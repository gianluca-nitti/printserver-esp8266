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
  protected:
    Printer(String _printerId);
    virtual bool canPrint() = 0;
    virtual void printByte(byte b) = 0;
  public:
    void init();
    bool canAcceptJob(int size);
    void startJob(int clientId);
    void endJob(int clientId);
    bool canPrint(int clientId);
    void printByte(int clientId, byte b);
    void processQueue();
    virtual String getInfo() = 0;
};
