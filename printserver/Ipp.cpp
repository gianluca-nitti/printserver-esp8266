#include <map>
#include <set>
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

String blockingReadString(Stream* source, int len) {
  // TODO: timeout
  while (source->available() < len) {
    yield();
  }
  String result = "";
  result.reserve(len);
  for (int i = 0; i < len; i++) {
    result += (char) source->read();
  }
  return result;
}

std::map<String, std::set<String>> parseRequestAttributes(Stream* source) {
  std::map<String, std::set<String>> result;
  byte tag = Http::blockingRead(source);
  if (tag == IPP_OPERATION_ATTRIBUTES_TAG) {
    String name = "";
    while ((tag = Http::blockingRead(source)) >= 0x10) { //if tag >= 0x10, then it's a value-tag
      uint16_t nameLength = blockingRead2Bytes(source);
      if (nameLength != 0) {
        name = blockingReadString(source, nameLength);
      } //otherwise, it's another value for the previous attribute
      uint16_t valueLength = blockingRead2Bytes(source);
      String value = blockingReadString(source, valueLength);
      result[name].insert(value);
      Serial.printf("Parsed IPP attribute: tag=0x%02X, name=\"%s\", value=\"%s\"\r\n", tag, name.c_str(), value.c_str());
    }
  } else if (tag != IPP_END_OF_ATTRIBUTES_TAG) {
    //bad request, TODO throw
  }
  return result;
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

void beginResponse(Stream* dest, uint16_t statusCode, uint32_t requestId) {
  dest->print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n\r\n");
  write2Bytes(dest, IPP_SUPPORTED_VERSION);
  write2Bytes(dest, statusCode);
  write4Bytes(dest, requestId);
}

void writeAttribute(Stream* dest, byte valueTag, String name, String value) {
  dest->write(valueTag);
  write2Bytes(dest, (uint16_t) name.length());
  dest->print(name);
  write2Bytes(dest, (uint16_t) value.length());
  dest->print(value);
}

inline void writeAttribute(Stream* dest, String name, attribute_value_t value) {
  writeAttribute(dest, value.valueTag, name, value.value);
}

attribute_value_t getPrinterAttribute(String name) {
  return {IPP_VALUE_TAG_UNSUPPORTED, ""};
}

void Ipp::parseRequest(WiFiClient* c) {
  http_req_t req = Http::parseRequestHeader(c);
  if (req.parseSuccess && req.httpMethod == "POST" && req.path == "/") {
    uint16_t ippVersion = blockingRead2Bytes(c);
    uint16_t operationId = blockingRead2Bytes(c);
    uint32_t requestId = blockingRead4Bytes(c);
    Serial.printf("Received IPP request; Version: 0x%04X, OperationId: 0x%04X, RequestId: 0x%08X\r\n", ippVersion, operationId, requestId);
    if (ippVersion == IPP_SUPPORTED_VERSION) {
      std::map<String, std::set<String>> requestAttributes = parseRequestAttributes(c);
      std::set<String> unsupportedAttributes;
      switch (operationId) {
        case IPP_GET_PRINTER_ATTRIBUTES:
          Serial.println("Operation is Get-printer-Attributes");
          beginResponse(c, IPP_SUCCESFUL_OK, requestId);
          c->write(IPP_OPERATION_ATTRIBUTES_TAG);
          writeAttribute(c, IPP_VALUE_TAG_CHARSET, "attributes-charset", *requestAttributes["attributes-charset"].begin());
          writeAttribute(c, IPP_VALUE_TAG_NATURAL_LANGUAGE, "attributes-natural-language", "en-us");
          c->write(IPP_PRINTER_ATTRIBUTES_TAG);
          for (String attributeName: requestAttributes["requested-attributes"]) {
            attribute_value_t attributeValue = getPrinterAttribute(attributeName);
            if (attributeValue.valueTag == IPP_VALUE_TAG_UNSUPPORTED) {
              unsupportedAttributes.insert(attributeName);
            } else {
              Serial.printf("Supported attribute: \"%s\"\r\n", attributeName.c_str());
              writeAttribute(c, attributeName, attributeValue);
            }
          }
          c->write(IPP_UNSUPPORTED_ATTRIBUTES_TAG);
          for (String attributeName: unsupportedAttributes) {
            Serial.printf("Unupported attribute: \"%s\"\r\n", attributeName.c_str());
            writeAttribute(c, IPP_VALUE_TAG_UNSUPPORTED, attributeName, "unsupported");
          }
          c->write(IPP_END_OF_ATTRIBUTES_TAG);
          c->stop();
          break;
        case IPP_PRINT_JOB:
          Serial.println("Operation is Print-Job");
          break;
        default:
          // TODO: report that operation is not supported
          Serial.println("The requested operation is not supported!");
      }
    } else {
      Serial.println("Unsupported IPP version");
      beginResponse(c, IPP_ERROR_VERSION_NOT_SUPPORTED, requestId);
      c->write(IPP_END_OF_ATTRIBUTES_TAG);
      c->stop();
    }
  } else {
    c->print("HTTP/1.1 400 Bad Request\r\n\r\n");
    c->stop();
  }
}
