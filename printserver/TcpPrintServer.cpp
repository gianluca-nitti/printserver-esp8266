#include "TcpPrintServer.h"

TcpPrintServer::TcpPrintServer(int port, Printer* p) : server(port) {
  printer = p;
}

void TcpPrintServer::handleClient(int index) {
  if (clients[index].connection.connected()) {
    if (clients[index].connection.available() > 0 && printer->canPrint(index)) {
      printer->printByte(index, clients[index].connection.read());
      clients[index].lastInteraction = millis();
    } else if (millis() - clients[index].lastInteraction > JOB_TIMEOUT_MS) {
      Serial.println("Cancelling print job and disconnecting client");
      clients[index].connection.stop();
      clients[index] = {WiFiClient(), 0};
      printer->endJob(index, true);
    }
  } else {
    Serial.println("Disconnected");
    clients[index].connection.stop();
    clients[index] = {WiFiClient(), 0};
    printer->endJob(index, false);
  }
}

void TcpPrintServer::start() {
  server.begin();
}

void TcpPrintServer::process() {
  int freeClientSlot = -1;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i].connection) {
      handleClient(i);
    } else {
      freeClientSlot = i;
    }
  }
  if (freeClientSlot != -1) {
    WiFiClient newClient = server.available();
    if (newClient) {
      Serial.println("Connected: " + newClient.remoteIP().toString() + ":" + newClient.remotePort());
      clients[freeClientSlot] = {newClient, millis()};
      printer->startJob(freeClientSlot);
    }
  }
}

void TcpPrintServer::printInfo() {
  int usedSlots = 0;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i].connection) {
      usedSlots++;
    }
  }
  Serial.printf("Server slots: %d/%d\n", usedSlots, MAXCLIENTS);
}
