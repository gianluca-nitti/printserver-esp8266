#include "WiFiManager.h"
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
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>ESP8266 print server</h1><a href=\"/wifi\">WiFi configuration</a><br><a href=\"/printerInfo\">Printer Info</a>");
    } else if (req.parseSuccess && req.httpMethod == "GET" && req.path == "/printerInfo") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n");
      newHttpClient.print(printer->getInfo());
    } else if (req.parseSuccess && req.httpMethod == "GET" && req.path == "/wifi") {
      newHttpClient.print("HTTP/1.1 200 OK \r\n\r\n<h1>WiFi configuration</h1><p>Status: ");
      newHttpClient.print(WiFiManager::info());
      newHttpClient.print("</p><form method=\"POST\" action=\"/wifi-connect\">Available networks (choose one to connect):<ul>");
      WiFiManager::getAvailableNetworks([&newHttpClient](String ssid, int encryption, int rssi){
        newHttpClient.printf("<li><input type=\"radio\" name=\"SSID\" value=\"%s\">%s (%s, %d dBm)</li>", ssid.c_str(), ssid.c_str(), WiFiManager::getEncryptionTypeName(encryption), rssi);
      });
      newHttpClient.print("</ul>Password (leave blank for open networks): <input type=\"password\" name=\"password\"><input type=\"submit\" value=\"Connect\"></form>");
    } else if (req.parseSuccess && req.httpMethod == "POST" && req.path == "/wifi-connect") {
      std::map<String, String> reqData = parseHttpUrlencoded(&newHttpClient);
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

  c->readStringUntil('\r');
  c->read(); //consume the '\n'

  String header;
  while ((header = c->readStringUntil('\r')) != "") {
    Serial.print("Received HTTP request header: ");
    Serial.println(header);
    c->read(); //consume the '\n'
  }
  c->read(); //consume the '\n'
  Serial.println("Headers finished");

  result.parseSuccess = true;
  return result;
}


std::map<String, String> TcpPrintServer::parseHttpUrlencoded(WiFiClient* c) {
  std::map<String, String> result;
  for (int i = 0; i < 2; i++) { //TODO!!
    String key = c->readStringUntil('=');
    String value = c->readStringUntil('&');
    result[key] = value;
  }
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
