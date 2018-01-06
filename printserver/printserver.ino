#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "TcpPrintServer.h"
#include "DirectParallelPortPrinter.h"
#include "SerialPortPrinter.h"

#define STROBE 10
#define BUSY 9
#define TCP_SERVER_PORT 12345
int DATA[8] = {D0, D1, D2, D3, D4, D5, D6, D7};

//DirectParallelPortPrinter printer(DATA, STROBE, BUSY);
SerialPortPrinter printer(&Serial);
TcpPrintServer server(TCP_SERVER_PORT, &printer);

void setup() {
  Serial.begin(115200);
  Serial.println("boot ok");
  parallelPort_setup();
  wifi_setup();
  wifi_waitOnline();
  //tcpServer.begin();
  server.start();
  Serial.println("setup ok");
}

void loop() {
  printDebugAndYield();
  server.process();
}

inline void printDebugAndYield() {
  static unsigned long lastCall = 0;
  if (millis() - lastCall > 5000) {
    Serial.println(wifi_info());
    server.printInfo();
    yield();
    lastCall = millis();
  }
}
