#include "IppStream.h"

IppStream::IppStream(WiFiClient conn): HttpStream(conn) {
}

std::map<String, std::set<String>> IppStream::parseRequestAttributes() {
  std::map<String, std::set<String>> result;
  byte tag = read();
  if (tag == IPP_OPERATION_ATTRIBUTES_TAG) {
    String name = "";
    while ((tag = read()) >= 0x10) { //if tag >= 0x10, then it's a value-tag
      uint16_t nameLength = read2Bytes();
      if (nameLength != 0) {
        name = readString(nameLength);
      } //otherwise, it's another value for the previous attribute
      uint16_t valueLength = read2Bytes();
      String value = readString(valueLength);
      result[name].insert(value);
      //Serial.printf("Parsed IPP attribute: tag=0x%02X, name=\"%s\", value=\"%s\"\r\n", tag, name.c_str(), value.c_str());
    }
  } else if (tag != IPP_END_OF_ATTRIBUTES_TAG) {
    //bad request, TODO throw
  }
  return result;
}

void IppStream::beginResponse(uint16_t statusCode, uint32_t requestId) {
  print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n\r\n");
  write2Bytes(IPP_SUPPORTED_VERSION);
  write2Bytes(statusCode);
  write4Bytes(requestId);
}

void IppStream::writeAttribute(byte valueTag, String name, String value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes((uint16_t) value.length());
  print(value);
}

void IppStream::writeAttribute(String name, attribute_value_t value) {
  writeAttribute(value.valueTag, name, value.value);
}

attribute_value_t getPrinterAttribute(String name) {
  return {IPP_VALUE_TAG_UNSUPPORTED, ""};
}

bool IppStream::parseRequest() {
  if (!parseRequestHeader()) {
    return false;
  }
  if (getRequestMethod() != "POST" || getRequestPath() != "/") {
    print("HTTP/1.1 400 Bad Request\r\n");
    return false;
  }

  uint16_t ippVersion = read2Bytes();
  uint16_t operationId = read2Bytes();
  uint32_t requestId = read4Bytes();
  Serial.printf("Received IPP request; Version: 0x%04X, OperationId: 0x%04X, RequestId: 0x%08X\r\n", ippVersion, operationId, requestId);
  if (ippVersion != IPP_SUPPORTED_VERSION) {
    Serial.println("Unsupported IPP version");
    beginResponse(IPP_ERROR_VERSION_NOT_SUPPORTED, requestId);
    write(IPP_END_OF_ATTRIBUTES_TAG);
    return false;
  }

  std::map<String, std::set<String>> requestAttributes = parseRequestAttributes();
  std::set<String> unsupportedAttributes;
  switch (operationId) {
    case IPP_GET_PRINTER_ATTRIBUTES:
      Serial.println("Operation is Get-printer-Attributes");
      beginResponse(IPP_SUCCESFUL_OK, requestId);
      write(IPP_OPERATION_ATTRIBUTES_TAG);
      writeAttribute(IPP_VALUE_TAG_CHARSET, "attributes-charset", *requestAttributes["attributes-charset"].begin());
      writeAttribute(IPP_VALUE_TAG_NATURAL_LANGUAGE, "attributes-natural-language", "en-us");
      write(IPP_PRINTER_ATTRIBUTES_TAG);
      for (String attributeName : requestAttributes["requested-attributes"]) {
        attribute_value_t attributeValue = getPrinterAttribute(attributeName);
        if (attributeValue.valueTag == IPP_VALUE_TAG_UNSUPPORTED) {
          unsupportedAttributes.insert(attributeName);
        } else {
          //Serial.printf("Supported attribute: \"%s\"\r\n", attributeName.c_str());
          writeAttribute(attributeName, attributeValue);
        }
      }
      write(IPP_UNSUPPORTED_ATTRIBUTES_TAG);
      for (String attributeName : unsupportedAttributes) {
        //Serial.printf("Unupported attribute: \"%s\"\r\n", attributeName.c_str());
        writeAttribute(IPP_VALUE_TAG_UNSUPPORTED, attributeName, "unsupported");
      }
      write(IPP_END_OF_ATTRIBUTES_TAG);
      return false;
    case IPP_PRINT_JOB:
      Serial.println("Operation is Print-Job");
      return true;
    default:
      // TODO: report that operation is not supported
      Serial.println("The requested operation is not supported!");
      return false;
  }
}
