#include "IppStream.h"

std::set<String> allPrinterDescriptionAttributes {
  "charset-configured",
  "charset-supported",
  "compression-supported",
  "document-format-default",
  "document-format-supported",
  "generated-natural-language-supported",
  "ipp-versions-supported",
  "natural-language-configured",
  "operations-supported",
  "pdl-override-supported",
  "printer-name",
  "printer-is-accepting-jobs",
  "printer-state",
  "printer-state-reasons",
  "printer-up-time",
  "printer-uri-supported",
  "queued-job-count",
  "uri-authentication-supported",
  "uri-security-supported"
};

IppStream::IppStream(WiFiClient conn): HttpStream(conn) {
}

std::map<String, std::set<String>> IppStream::parseRequestAttributes() {
  std::map<String, std::set<String>> result;
  byte tag = read();
  if (tag == IPP_OPERATION_ATTRIBUTES_TAG) {
    String name = "";
    int index = 0;
    while ((tag = read()) >= 0x10) { //if tag >= 0x10, then it's a value-tag
      uint16_t nameLength = read2Bytes();
      if (nameLength != 0) {
        name = readString(nameLength);
      } //otherwise, it's another value for the previous attribute
      uint16_t valueLength = read2Bytes();
      String value = readString(valueLength);
      if ((index == 0 && name != "attributes-charset") || (index == 1 && name != "attributes-natural-language")) {
        result.clear();
        return result;
      }
      result[name].insert(value);
      index++;
      Serial.printf("Parsed IPP attribute: tag=0x%02X, name=\"%s\", value=\"%s\"\r\n", tag, name.c_str(), value.c_str());
    }
  } else if (tag != IPP_END_OF_ATTRIBUTES_TAG) {
    //bad request, TODO throw
  }
  return result;
}

void IppStream::beginResponse(uint16_t statusCode, uint32_t requestId, String charset) {
  print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\nContent-Type: application/ipp\r\n\r\n");
  write2Bytes(IPP_SUPPORTED_VERSION);
  write2Bytes(statusCode);
  write4Bytes(requestId);
  write(IPP_OPERATION_ATTRIBUTES_TAG);
  writeAttribute(IPP_VALUE_TAG_CHARSET, "attributes-charset", charset);
  writeAttribute(IPP_VALUE_TAG_NATURAL_LANGUAGE, "attributes-natural-language", "en-us");
}

void IppStream::writeAttribute(byte valueTag, String name, String value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes((uint16_t) value.length());
  print(value);
}

void IppStream::writeAttribute(String name, attribute_value_t value) {
  write(value.valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes((uint16_t) value.valueLength);
  for (int i = 0; i < value.valueLength; i++) {
    write(value.value[i]);
  }
}

attribute_value_t getPrinterAttribute(String name) {
  if (name == "charset-configured") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_CHARSET, "utf-8");
  } else if (name == "charset-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_CHARSET, "utf-8");
  } else if (name == "compression-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "none");
  } else if (name == "document-format-default") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_MIME_MEDIA_TYPE, "text/plain");
  } else if (name == "document-format-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_MIME_MEDIA_TYPE, "text/plain");
  } else if (name == "generated-natural-language-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_NATURAL_LANGUAGE, "en-us");
  } else if (name =="ipp-versions-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "1.1");
  } else if (name == "natural-language-configured") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_NATURAL_LANGUAGE, "en-us");
  } else if (name == "operations-supported") {
    return {IPP_VALUE_TAG_ENUM, "\0\0\0\x02", 4};
  } else if (name == "pdl-override-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "en-us");
  } else if (name == "printer-name") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_NAME, "ESP8266 print server"); //TODO
  } else if (name =="printer-is-accepting-jobs") {
    return {IPP_VALUE_TAG_BOOLEAN, "\x01", 1};
  } else if (name == "printer-state") {
    return {IPP_VALUE_TAG_ENUM, "\0\0\0\x03", 4}; //3 = idle
  } else if (name == "printer-state-reasons") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "none");
  } else if (name == "printer-up-time") {
    return {IPP_VALUE_TAG_INTEGER, "\0\0\0\x01", 4};
  } else if (name == "printer-uri-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_URI, "ipp://192.168.1.1"); //TODO
  } else if (name == "queued-job-count") {
    return {IPP_VALUE_TAG_INTEGER, "\0\0\0\0", 4};
  } else if (name == "uri-authentication-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "none");
  } else if (name == "uri-security-supported") {
    return STRING_ATTRIBUTE_VALUE(IPP_VALUE_TAG_KEYWORD, "none");
  } else {
    return {IPP_VALUE_TAG_UNSUPPORTED, ""};
  }
}

void IppStream::handleGetPrinterAttributesRequest(std::map<String, std::set<String>> requestAttributes) {
  std::set<String>& requestedAttributes = requestAttributes["requested-attributes"];
  std::set<String> supportedAttributes;

  if (requestedAttributes.size() == 0 || (requestedAttributes.find("all") != requestedAttributes.end()) || (requestedAttributes.find("printer-description") != requestedAttributes.end())) {
    Serial.println("----sending all attributes");
    requestedAttributes = allPrinterDescriptionAttributes;
  }

  write(IPP_UNSUPPORTED_ATTRIBUTES_TAG);
  for (String attributeName: requestedAttributes) {
    if (getPrinterAttribute(attributeName).valueTag == IPP_VALUE_TAG_UNSUPPORTED) {
      writeAttribute(IPP_VALUE_TAG_UNSUPPORTED, attributeName, "unsupported");
      Serial.printf("----------------------------Unsupported attribute: \"%s\"\r\n", attributeName.c_str());
    } else {
      supportedAttributes.insert(attributeName);
    }
  }

  write(IPP_PRINTER_ATTRIBUTES_TAG);
  for (String attributeName: supportedAttributes) {
    Serial.printf("------------------------------Supported attribute: \"%s\"\r\n", attributeName.c_str());
    writeAttribute(attributeName, getPrinterAttribute(attributeName));
  }

  write(IPP_END_OF_ATTRIBUTES_TAG);
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
    beginResponse(IPP_SERVER_ERROR_VERSION_NOT_SUPPORTED, requestId, "utf-8");
    write(IPP_END_OF_ATTRIBUTES_TAG);
    return false;
  }

  if (requestId == 0) { //request-id must not be 0 (RFC8011 Section 4.1.2)
    beginResponse(IPP_CLIENT_ERROR_BAD_REQUEST, requestId, "utf-8");
    write(IPP_END_OF_ATTRIBUTES_TAG);
    return false;
  }

  std::map<String, std::set<String>> requestAttributes = parseRequestAttributes();

  if (requestAttributes.size() == 0) {
    beginResponse(IPP_CLIENT_ERROR_BAD_REQUEST, requestId, "utf-8");
    write(IPP_END_OF_ATTRIBUTES_TAG);
    return false;
  }

  switch (operationId) {
    case IPP_GET_PRINTER_ATTRIBUTES:
      Serial.println("Operation is Get-printer-Attributes");
      beginResponse(IPP_SUCCESFUL_OK, requestId, *requestAttributes["attributes-charset"].begin());
      handleGetPrinterAttributesRequest(requestAttributes);
      return false;

    case IPP_PRINT_JOB:
      Serial.println("Operation is Print-Job");
      beginResponse(IPP_SUCCESFUL_OK, requestId, *requestAttributes["attributes-charset"].begin());
      write(IPP_END_OF_ATTRIBUTES_TAG);
      return true;

    default:
      Serial.println("The requested operation is not supported!");
      beginResponse(IPP_SERVER_ERROR_OPERATION_NOT_SUPPORTED, requestId, "utf-8");
      write(IPP_END_OF_ATTRIBUTES_TAG);
      return false;
  }
}
