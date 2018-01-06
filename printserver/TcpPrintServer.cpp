#include "TcpPrintServer.h"

TcpPrintServer::TcpPrintServer(int port) : server(port) {
}

void TcpPrintServer::handleClient(int index) {
  if (clients[index].connected()) {
    if (clients[index].available() > 0) {
      Serial.write(clients[index].read());
    }
  } else {
    Serial.println("Disconnected");
    clients[index].stop();
    clients[index] = WiFiClient();
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
