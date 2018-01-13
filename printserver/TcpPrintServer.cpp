#include "Settings.h"
#include "TcpPrintServer.h"

TcpPrintServer::TcpPrintServer(Printer* p) : socketServer(SOCKET_SERVER_PORT), httpServer(HTTP_SERVER_PORT) {
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
  socketServer.begin();
  httpServer.begin();
}

void TcpPrintServer::process() {
  // socket
  int freeClientSlot = -1;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i].connection) {
      handleClient(i);
    } else {
      freeClientSlot = i;
    }
  }
  if (freeClientSlot != -1) {
    WiFiClient newClient = socketServer.available();
    if (newClient) {
      Serial.println("Connected: " + newClient.remoteIP().toString() + ":" + newClient.remotePort());
      clients[freeClientSlot] = {newClient, millis()};
      printer->startJob(freeClientSlot);
    }
  }

  //http
  unsigned long startTime = millis();
  WiFiClient newHttpClient = httpServer.available();
  if (newHttpClient) {
    http_req_t req = parseHttpRequest(&newHttpClient);
    if (req.parseSuccess && req.httpMethod == "GET" && req.path == "/") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>ESP8266 print server</h1>");
    } else if (req.parseSuccess && req.httpMethod == "GET" && req.path == "/printerInfo") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n");
      newHttpClient.print(printer->getInfo());
    } else {
      newHttpClient.print("HTTP/1.1 404 Not Found \r\n\r\n<h1>Not found</h1>");
    }
    newHttpClient.stop();
    Serial.println("HTTP client handled in " + String(millis() - startTime) + "ms");
  }
}

http_req_t TcpPrintServer::parseHttpRequest(WiFiClient* c) {
  http_req_t result;
  result.parseSuccess = false;
  c->setTimeout(5000);
  result.httpMethod = c->readStringUntil(' ');
  if (result.httpMethod == "") {
    return result;
  }
  result.path = c->readStringUntil(' ');
  if (result.path == "") {
    return result;
  }
  result.parseSuccess = true;
  return result;
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
