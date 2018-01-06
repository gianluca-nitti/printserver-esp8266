#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "TcpPrintServer.h"

#define TCP_SERVER_PORT 12345
//WiFiServer tcpServer(TCP_SERVER_PORT);
TcpPrintServer server(TCP_SERVER_PORT);

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
  /*WiFiClient c = tcpServer.available();
  if (c) {
    Serial.println("---client " + c.remoteIP().toString() + ":" + c.remotePort() + " connected");
    printDataFromClient(c);
    c.stop();
    Serial.println("---connection closed---");
  }*/
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

void printDataFromClient(WiFiClient c) {
  while (c.connected()) {
    if (c.available()) {
      byte b = c.read();
      Serial.write(b);
      parallelPort_printByte(b);
    }
  }
}
