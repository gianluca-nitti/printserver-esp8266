#include "WiFiManager.h"
#include "Settings.h"
#include "HttpStream.h"
#include "Ipp.h"
#include "TcpPrintServer.h"

TcpPrintServer::TcpPrintServer(Printer* p) : socketServer(SOCKET_SERVER_PORT), ippServer(IPP_SERVER_PORT), httpServer(HTTP_SERVER_PORT) {
  printer = p;
  for (int i = 0; i < MAXCLIENTS; i++) {
    clients[i] = NULL;
  }
}

void TcpPrintServer::handleClient(int index) {
  if (clients[index]->connected()) {
    if (clients[index]->hasMoreData() && printer->canPrint(index)) {
      printer->printByte(index, clients[index]->read());
    } //TODO timeout
  } else {
    Serial.println("Disconnected");
    delete clients[index];
    clients[index] = NULL;
    printer->endJob(index, false);
  }
}

void TcpPrintServer::start() {
  socketServer.begin();
  ippServer.begin();
  httpServer.begin();
}

void TcpPrintServer::processSocketClients() {
  int freeClientSlot = -1;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i] != NULL) {
      handleClient(i);
    } else {
      freeClientSlot = i;
    }
  }
  if (freeClientSlot != -1) {
    WiFiClient newClient = socketServer.available();
    if (newClient) {
      Serial.println("Connected: " + newClient.remoteIP().toString() + ":" + newClient.remotePort());
      clients[freeClientSlot] = new TcpStream(newClient);
      printer->startJob(freeClientSlot);
    }
  }
}

void TcpPrintServer::processIppClients() {
  WiFiClient _ippClient = ippServer.available();
  if (_ippClient) {
    HttpStream ippClient(&_ippClient);
    Ipp::parseRequest(ippClient);
  }
}

void TcpPrintServer::processWebClients(){
  unsigned long startTime = millis();
  WiFiClient _httpClient = httpServer.available();
  if (_httpClient) {
    HttpStream newHttpClient(&_httpClient);
    if (!newHttpClient.parseRequestHeader()) {
      return;
    }
    String method = newHttpClient.getRequestMethod();
    String path = newHttpClient.getRequestPath();
    if (method == "GET" && path == "/") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>ESP8266 print server</h1><a href=\"/wifi\">WiFi configuration</a><br><a href=\"/printerInfo\">Printer Info</a>");
    } else if (method == "GET" && path == "/printerInfo") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n");
      newHttpClient.print(printer->getInfo());
    } else if (method == "GET" && path == "/wifi") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>WiFi configuration</h1><p>Status: ");
      newHttpClient.print(WiFiManager::info());
      newHttpClient.print("</p><form method=\"POST\" action=\"/wifi-connect\">Available networks (choose one to connect):<ul>");
      WiFiManager::getAvailableNetworks([&newHttpClient](String ssid, int encryption, int rssi){
        newHttpClient.print("<li><input type=\"radio\" name=\"SSID\" value=\"" + ssid + "\">" + ssid + " (" + WiFiManager::getEncryptionTypeName(encryption) + ", " + String(rssi) + " dBm)</li>");
      });
      newHttpClient.print("</ul>Password (leave blank for open networks): <input type=\"password\" name=\"password\"><input type=\"submit\" value=\"Connect\"></form>");
    } else if (method == "POST" && path == "/wifi-connect") {
      std::map<String, String> reqData = newHttpClient.parseUrlencodedRequestBody();
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>OK</h1>");
      newHttpClient.stop();
      WiFiManager::connectTo(reqData["SSID"].c_str(), reqData["password"]);
    } else {
      newHttpClient.print("HTTP/1.1 404 Not Found \r\n\r\n<h1>Not found</h1>");
    }
    newHttpClient.stop();
    Serial.println("HTTP client handled in " + String(millis() - startTime) + "ms");
  }
}

void TcpPrintServer::process() {
  processSocketClients();
  processIppClients();
  processWebClients();
}

void TcpPrintServer::printInfo() {
  int usedSlots = 0;
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i] != NULL) {
      usedSlots++;
    }
  }
  Serial.printf("Server slots: %d/%d\n", usedSlots, MAXCLIENTS);
}
