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
