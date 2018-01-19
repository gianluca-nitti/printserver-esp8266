#include <map>
#include <set>
#include "Ipp.h"

std::map<String, std::set<String>> parseRequestAttributes(HttpStream source) {
  std::map<String, std::set<String>> result;
  byte tag = source.read();
  if (tag == IPP_OPERATION_ATTRIBUTES_TAG) {
    String name = "";
    while ((tag = source.read()) >= 0x10) { //if tag >= 0x10, then it's a value-tag
      uint16_t nameLength = source.read2Bytes();
      if (nameLength != 0) {
        name = source.readString(nameLength);
      } //otherwise, it's another value for the previous attribute
      uint16_t valueLength = source.read2Bytes();
      String value = source.readString(valueLength);
      result[name].insert(value);
      Serial.printf("Parsed IPP attribute: tag=0x%02X, name=\"%s\", value=\"%s\"\r\n", tag, name.c_str(), value.c_str());
    }
  } else if (tag != IPP_END_OF_ATTRIBUTES_TAG) {
    //bad request, TODO throw
  }
  return result;
}

void beginResponse(HttpStream dest, uint16_t statusCode, uint32_t requestId) {
  dest.print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n\r\n");
  dest.write2Bytes(IPP_SUPPORTED_VERSION);
  dest.write2Bytes(statusCode);
  dest.write4Bytes(requestId);
}

void writeAttribute(HttpStream dest, byte valueTag, String name, String value) {
  dest.write(valueTag);
  dest.write2Bytes((uint16_t) name.length());
  dest.print(name);
  dest.write2Bytes((uint16_t) value.length());
  dest.print(value);
}

inline void writeAttribute(HttpStream dest, String name, attribute_value_t value) {
  writeAttribute(dest, value.valueTag, name, value.value);
}

attribute_value_t getPrinterAttribute(String name) {
  return {IPP_VALUE_TAG_UNSUPPORTED, ""};
}

void Ipp::parseRequest(HttpStream c) {
  http_req_t req = c.parseRequestHeader();
  if (req.parseSuccess && req.httpMethod == "POST" && req.path == "/") {
    uint16_t ippVersion = c.read2Bytes();
    uint16_t operationId = c.read2Bytes();
    uint32_t requestId = c.read4Bytes();
    Serial.printf("Received IPP request; Version: 0x%04X, OperationId: 0x%04X, RequestId: 0x%08X\r\n", ippVersion, operationId, requestId);
    if (ippVersion == IPP_SUPPORTED_VERSION) {
      std::map<String, std::set<String>> requestAttributes = parseRequestAttributes(c);
      std::set<String> unsupportedAttributes;
      switch (operationId) {
        case IPP_GET_PRINTER_ATTRIBUTES:
          Serial.println("Operation is Get-printer-Attributes");
          beginResponse(c, IPP_SUCCESFUL_OK, requestId);
          c.write(IPP_OPERATION_ATTRIBUTES_TAG);
          writeAttribute(c, IPP_VALUE_TAG_CHARSET, "attributes-charset", *requestAttributes["attributes-charset"].begin());
          writeAttribute(c, IPP_VALUE_TAG_NATURAL_LANGUAGE, "attributes-natural-language", "en-us");
          c.write(IPP_PRINTER_ATTRIBUTES_TAG);
          for (String attributeName: requestAttributes["requested-attributes"]) {
            attribute_value_t attributeValue = getPrinterAttribute(attributeName);
            if (attributeValue.valueTag == IPP_VALUE_TAG_UNSUPPORTED) {
              unsupportedAttributes.insert(attributeName);
            } else {
              Serial.printf("Supported attribute: \"%s\"\r\n", attributeName.c_str());
              writeAttribute(c, attributeName, attributeValue);
            }
          }
          c.write(IPP_UNSUPPORTED_ATTRIBUTES_TAG);
          for (String attributeName: unsupportedAttributes) {
            Serial.printf("Unupported attribute: \"%s\"\r\n", attributeName.c_str());
            writeAttribute(c, IPP_VALUE_TAG_UNSUPPORTED, attributeName, "unsupported");
          }
          c.write(IPP_END_OF_ATTRIBUTES_TAG);
          c.stop();
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
      c.write(IPP_END_OF_ATTRIBUTES_TAG);
      c.stop();
    }
  } else {
    c.print("HTTP/1.1 400 Bad Request\r\n\r\n");
    c.stop();
  }
}
