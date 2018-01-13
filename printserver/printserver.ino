#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <FS.h>

#include "TcpPrintServer.h"
#include "DirectParallelPortPrinter.h"
#include "ShiftRegParallelPortPrinter.h"
#include "SerialPortPrinter.h"
#include "PrintQueue.h"

#define TCP_SERVER_PORT 12345

/*#define STROBE 10
#define BUSY 9
int DATA[8] = {D0, D1, D2, D3, D4, D5, D6, D7};
DirectParallelPortPrinter printer("lpt1", DATA, STROBE, BUSY);*/

#define LPT_DATA D2
#define LPT_LATCH D3
#define LPT_CLK D4
#define LPT_BUSY D5
#define LPT_STROBE D6
ShiftRegParallelPortPrinter printer("lpt1", LPT_DATA, LPT_CLK, LPT_LATCH, LPT_STROBE, LPT_BUSY);

//SerialPortPrinter printer("serial", &Serial);

TcpPrintServer server(TCP_SERVER_PORT, &printer);

void setup() {
  Serial.begin(115200);
  Serial.println("boot ok");
  SPIFFS.begin();
  printer.init();
  wifi_setup();
  wifi_waitOnline();
  //tcpServer.begin();
  server.start();
  Serial.println("setup ok");
}

void loop() {
  printDebugAndYield();
  server.process();
  printer.processQueue();
}

inline void printDebugAndYield() {
  static unsigned long lastCall = 0;
  if (millis() - lastCall > 5000) {
    Serial.println(wifi_info());
    server.printInfo();
    FSInfo fsinfo;
    SPIFFS.info(fsinfo);
    Serial.printf("Total bytes: %d, Used bytes: %d, Max open files: %d\n", fsinfo.totalBytes, fsinfo.usedBytes, fsinfo.maxOpenFiles);

    PrintQueue::updateAvailableFlashSpace();
    yield();

    lastCall = millis();
  }
}
