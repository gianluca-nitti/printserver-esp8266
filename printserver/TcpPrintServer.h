#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <map>
#include "Settings.h"
#include "TcpStream.h"
#include "Printer.h"

class TcpPrintServer {
  private:
    WiFiServer socketServer;
    WiFiServer ippServer;
    WiFiServer httpServer;
    TcpStream* clients[MAXCLIENTS];
    int clientTargetPrinters[MAXCLIENTS];
    Printer** printers;
    int printerCount;

    void handleClient(int index);

    int getFreeClientSlot();
    void processNewSocketClients();
    void processNewIppClients();
    void processNewWebClients();
  public:
    TcpPrintServer(Printer** _printers, int _printerCount);
    void start();
    void process();
    void printInfo();
};
