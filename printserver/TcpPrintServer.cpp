#include "TcpPrintServer.h"

TcpPrintServer::TcpPrintServer(int port, Printer* p) : server(port) {
  printer = p;
}

void TcpPrintServer::handleClient(int index) {
  if (clients[index].connected()) {
    if (clients[index].available() > 0 && printer->canPrint(index)) {
      printer->printByte(index, clients[index].read());
    }
  } else {
    Serial.println("Disconnected");
    clients[index].stop();
    clients[index] = WiFiClient();
    printer->endJob(index);
  }
}

void TcpPrintServer::start() {
  server.begin();
}

void TcpPrintServer::process() {
  int freeClientSlot = -1;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i]) {
      handleClient(i);
    } else {
      freeClientSlot = i;
    }
  }
  if (freeClientSlot != -1) {
    WiFiClient newClient = server.available();
    if (newClient) {
      Serial.println("Connected: " + newClient.remoteIP().toString() + ":" + newClient.remotePort());
      clients[freeClientSlot] = newClient;
      printer->startJob(freeClientSlot);
    }
  }
}

void TcpPrintServer::printInfo() {
  int usedSlots = 0;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i]) {
      usedSlots++;
    }
  }
  Serial.printf("Server slots: %d/%d\n", usedSlots, MAXCLIENTS);
}
