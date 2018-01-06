#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "printer.h"

#define MAXCLIENTS 16

class TcpPrintServer {
  private:
    WiFiServer server;
    WiFiClient clients[MAXCLIENTS];
    void handleClient(int index);
  public:
    TcpPrintServer(int port);
    void start(/*Printer* p*/);
    void process();
    void printInfo();
};
