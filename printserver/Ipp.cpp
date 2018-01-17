#include "Http.h"
#include "Ipp.h"

uint16_t blockingRead2Bytes(Stream* source) {
  // TODO: timeout
  while (source->available() < 2) {
    yield();
  }
  uint16_t hi = source->read();
  uint16_t lo = source->read();
  return (hi << 8) | lo;
}

uint32_t blockingRead4Bytes(Stream* source) {
  // TODO: timeout
  while (source->available() < 4) {
    yield();
  }
  uint32_t b0 = source->read();
  uint32_t b1 = source->read();
  uint32_t b2 = source->read();
  uint32_t b3 = source->read();
  return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

void write2Bytes(Stream* dest, uint16_t data) {
  dest->write((byte) ((data & 0xFF00) >> 8));
  dest->write((byte) (data & 0x00FF));
}

void write4Bytes(Stream* dest, uint32_t data) {
  dest->write((byte) ((data & 0xFF000000) >> 24));
  dest->write((byte) ((data & 0x00FF0000) >> 16));
  dest->write((byte) ((data & 0x0000FF00) >> 8));
  dest->write((byte) (data & 0x000000FF));
}

void Ipp::parseRequest(WiFiClient* c) {
  http_req_t req = Http::parseRequestHeader(c);
  if (req.parseSuccess && req.httpMethod == "POST" && req.path == "/") {
    uint16_t ippVersion = blockingRead2Bytes(c);
    uint16_t operationId = blockingRead2Bytes(c);
    uint32_t requestId = blockingRead4Bytes(c);
    Serial.printf("IPP version: 0x%04X, OperationId: 0x%04X, RequestId: 0x%08X\r\n", ippVersion, operationId, requestId);
    if (ippVersion == IPP_SUPPORTED_VERSION) {
      c->stop();
    } else {
      // version not supported
      c->print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n\r\n");
      write2Bytes(c, IPP_SUPPORTED_VERSION);
      write2Bytes(c, IPP_ERROR_VERSION_NOT_SUPPORTED);
      write4Bytes(c, requestId);
      c->write(IPP_END_OF_ATTRIBUTES_TAG);
      c->stop();
    }
  } else {
    c->print("HTTP/1.1 400 Bad Request\r\n\r\n");
    c->stop();
  }
}
