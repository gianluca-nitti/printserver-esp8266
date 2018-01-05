#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#define TCP_SERVER_PORT 12345
WiFiServer tcpServer(TCP_SERVER_PORT);

void setup() {
  Serial.begin(115200);
  Serial.println("boot ok");
  parallelPort_setup();
  wifi_setup();
  wifi_waitOnline();
  tcpServer.begin();
  Serial.println("setup ok");
}

void loop() {
  Serial.println(wifi_info());
  WiFiClient c = tcpServer.available();
  if (c) {
    Serial.println("---client " + c.remoteIP().toString() + ":" + c.remotePort() + " connected");
    printDataFromClient(c);
    c.stop();
    Serial.println("---connection closed---");
  }
  delay(1000);
}

void printDataFromClient(WiFiClient c) {
  while (c.connected()) {
    if (c.available()) {
      byte b = c.read();
      Serial.write(b);
      parallelPort_printByte(b); // (atrent) disaccoppierei qui, applicando un pattern di delega in modo da poter configurare a posteriori anche una stampante seriale o USB
    }
  }
}
