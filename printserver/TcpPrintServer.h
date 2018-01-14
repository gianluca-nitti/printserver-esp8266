#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <map>
#include "Settings.h"
#include "Printer.h"

typedef struct {
  WiFiClient connection;
  unsigned long lastInteraction;
} client_t;

typedef struct {
  bool parseSuccess;
  String httpMethod;
  String path;
} http_req_t;

class TcpPrintServer {
  private:
    WiFiServer socketServer;
    WiFiServer httpServer;
    client_t clients[MAXCLIENTS];
    Printer* printer;
    void handleClient(int index);
    http_req_t parseHttpRequest(WiFiClient* c);
    std::map<String, String> parseHttpUrlencoded(WiFiClient* c);
  public:
    TcpPrintServer(Printer* p);
    void start();
    void process();
    void printInfo();
};
