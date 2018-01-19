#include "HttpStream.h"

HttpStream::HttpStream(WiFiClient* s): tcpConnection(s) {
  timedOut = false;
}

void HttpStream::waitAvailable(int numBytes) {
  unsigned long start = millis();
  while (tcpConnection->available() < numBytes) {
    delay(10);
    if (millis() - start > HTTP_READ_TIMEOUT_MS) {
      timedOut = true;
      // TODO: send 408 Request Timeout
      Serial.println("HTTP read timed out");
      tcpConnection->print("HTTP/1.1 408 Request Timeout\r\n\r\n");
      tcpConnection->stop();
      return;
    }
  }
}

byte HttpStream::read() {
  if (timedOut) {
    return 0;
  }
  waitAvailable(1);
  return tcpConnection->read();
}

uint16_t HttpStream::read2Bytes() {
  waitAvailable(2);
  if (timedOut) {
    return 0;
  }
  uint16_t hi = tcpConnection->read();
  uint16_t lo = tcpConnection->read();
  return (hi << 8) | lo;
}

uint32_t HttpStream::read4Bytes() {
  waitAvailable(4);
  if (timedOut) {
    return 0;
  }
  uint32_t b0 = tcpConnection->read();
  uint32_t b1 = tcpConnection->read();
  uint32_t b2 = tcpConnection->read();
  uint32_t b3 = tcpConnection->read();
  return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

String HttpStream::readStringUntil(char delim) {
  if (timedOut) {
    return "";
  }
  String result = "";
  result.reserve(16);
  char c;
  while (!timedOut && (c = read()) != delim) {
    result += c;
  }
  return result;
}

String HttpStream::readString(int len) {
  if (timedOut) {
    return "";
  }
  waitAvailable(len);
  String result = "";
  result.reserve(len);
  for (int i = 0; i < len; i++) {
    result += (char) tcpConnection->read();
  }
  return result;
}

void HttpStream::write(byte b) {
  if (!timedOut) {
    tcpConnection->write(b);
  }
}

void HttpStream::write2Bytes(uint16_t data) {
  tcpConnection->write((byte) ((data & 0xFF00) >> 8));
  tcpConnection->write((byte) (data & 0x00FF));
}

void HttpStream::write4Bytes(uint32_t data) {
  tcpConnection->write((byte) ((data & 0xFF000000) >> 24));
  tcpConnection->write((byte) ((data & 0x00FF0000) >> 16));
  tcpConnection->write((byte) ((data & 0x0000FF00) >> 8));
  tcpConnection->write((byte) (data & 0x000000FF));
}

void HttpStream::print(String s) {
  if (!timedOut) {
    tcpConnection->print(s);
  }
}

void HttpStream::stop() {
  tcpConnection->stop();
}

http_req_t HttpStream::parseRequestHeader() {
  http_req_t result;
  result.parseSuccess = false;
  if (timedOut) {
    return result;
  }
  result.httpMethod = readStringUntil(' ');
  if (result.httpMethod == "") {
    return result;
  }
  result.path = readStringUntil(' ');
  if (result.path == "") {
    return result;
  }

  readStringUntil('\r');
  read(); //consume the '\n'

  String header;
  while ((header = readStringUntil('\r')) != "") {
    if (header.startsWith(CONTENT_LENGTH_HEADER)) {
      result.contentLength = header.substring(STRLEN(CONTENT_LENGTH_HEADER)).toInt();
    }
    read(); //consume the '\n'
  }
  read(); //consume the '\n'

  result.parseSuccess = true;
  return result;
}

std::map<String, String> HttpStream::parseUrlencodedRequestBody(int contentLength) {
  std::map<String, String> result;
  if (timedOut) {
    return result;
  }
  int parsedBytes = 0;
  while (parsedBytes < contentLength) {
    String key, value;
    key.reserve(32);
    value.reserve(32);
    char readChar;
    while (parsedBytes < contentLength && ((readChar = read()) != '=')){
      key += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '='
    while (parsedBytes < contentLength && ((readChar = read()) != '&')){
      value += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '&'
    result[key] = value;
  }
  return result;
}
