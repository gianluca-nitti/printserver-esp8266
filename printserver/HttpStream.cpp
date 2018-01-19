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
      Serial.println("HTTP read timed out");
      tcpConnection->print("HTTP/1.1 408 Request Timeout\r\n\r\n");
      tcpConnection->stop();
      return;
    }
  }
}

void HttpStream::parseNextChunkLength() {
  String chunkLength = tcpConnection->readStringUntil('\r');
  waitAvailable(1);
  tcpConnection->read(); //consume '\n'
  remainingChunkBytes = (int) strtol(chunkLength.c_str(), NULL, 16);
  // TODO: check if length is zero (last chunk)
}

byte HttpStream::read() {
  if (timedOut) {
    return 0;
  }
  waitAvailable(1);
  byte result = tcpConnection->read();
  if (requestChunkedEncoded) {
    remainingChunkBytes--;
    if (remainingChunkBytes == 0) {
      // consume CRLF chunk end delimiter
      waitAvailable(2);
      tcpConnection->read();
      tcpConnection->read();
      parseNextChunkLength();
    }
  }
  return result;
}

uint16_t HttpStream::read2Bytes() {
  waitAvailable(2);
  if (timedOut) {
    return 0;
  }
  uint16_t hi = read();
  uint16_t lo = read();
  return (hi << 8) | lo;
}

uint32_t HttpStream::read4Bytes() {
  waitAvailable(4);
  if (timedOut) {
    return 0;
  }
  uint32_t b0 = read();
  uint32_t b1 = read();
  uint32_t b2 = read();
  uint32_t b3 = read();
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
    result += (char) read();
  }
  return result;
}

bool HttpStream::hasMoreData() {
  if (timedOut || !tcpConnection->connected()) {
    return false;
  }
  return remainingChunkBytes != 0; //TODO: make it work for non-chunked requests as well
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

bool HttpStream::parseRequestHeader() {
  if (timedOut) {
    return false;
  }
  requestMethod = readStringUntil(' ');
  if (timedOut || requestMethod == "") {
    return false;
  }
  requestPath = readStringUntil(' ');
  if (timedOut || requestPath == "") {
    return false;
  }

  readStringUntil('\r');
  read(); //consume the '\n'

  bool chunkedEncoded = false;
  String header;
  while ((header = readStringUntil('\r')) != "") {
    header.toLowerCase(); //does not return a new string, modifies the existing one in-place (source: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/WString.cpp#L732)
    if (header.startsWith(CONTENT_LENGTH_HEADER)) {
      requestContentLength = header.substring(STRLEN(CONTENT_LENGTH_HEADER)).toInt();
    } else if (header == CHUNKED_ENCODING_HEADER) {
      chunkedEncoded = true;
      remainingChunkBytes = 0;
    }
    read(); //consume the '\n'
  }
  read(); //consume the '\n'

  requestChunkedEncoded = chunkedEncoded;
  if (chunkedEncoded) {
    parseNextChunkLength();
  }
  return true;
}

std::map<String, String> HttpStream::parseUrlencodedRequestBody() {
  std::map<String, String> result;
  if (timedOut) {
    return result;
  }
  int parsedBytes = 0;
  while (parsedBytes < requestContentLength) {
    String key, value;
    key.reserve(32);
    value.reserve(32);
    char readChar;
    while (parsedBytes < requestContentLength && ((readChar = read()) != '=')){
      key += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '='
    while (parsedBytes < requestContentLength && ((readChar = read()) != '&')){
      value += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '&'
    result[key] = value;
  }
  return result;
}

String HttpStream::getRequestMethod() {
  return requestMethod;
}

String HttpStream::getRequestPath() {
  return requestPath;
}
