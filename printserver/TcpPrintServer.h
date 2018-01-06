#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "Printer.h"

#define MAXCLIENTS 16

class TcpPrintServer {
  private:
    WiFiServer server;
    WiFiClient clients[MAXCLIENTS];
    Printer* printer;
    void handleClient(int index);
  public:
    TcpPrintServer(int port, Printer* p);
    void start();
    void process();
    void printInfo();
};
