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
      //Serial.printf("Parsed IPP attribute: tag=0x%02X, name=\"%s\", value=\"%s\"\r\n", tag, name.c_str(), value.c_str());
    }
  }
  return result;
}

void IppStream::beginResponse(uint16_t statusCode, uint32_t requestId, String charset) {
  print("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\nContent-Type: application/ipp\r\n\r\n");
  write2Bytes(IPP_SUPPORTED_VERSION);
  write2Bytes(statusCode);
  write4Bytes(requestId);
  write(IPP_OPERATION_ATTRIBUTES_TAG);
  writeStringAttribute(IPP_VALUE_TAG_CHARSET, "attributes-charset", charset);
  writeStringAttribute(IPP_VALUE_TAG_NATURAL_LANGUAGE, "attributes-natural-language", "en-us");
}

void IppStream::writeStringAttribute(byte valueTag, String name, String value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes((uint16_t) value.length());
  print(value);
}

void IppStream::writeByteAttribute(byte valueTag, String name, byte value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes(1);
  write(value);
}

void IppStream::write2BytesAttribute(byte valueTag, String name, uint16_t value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes(2);
  write2Bytes(value);
}

void IppStream::write4BytesAttribute(byte valueTag, String name, uint32_t value) {
  write(valueTag);
  write2Bytes((uint16_t) name.length());
  print(name);
  write2Bytes(4);
  write4Bytes(value);
}

void IppStream::writePrinterAttribute(String name, Printer* printer) {
  if (name == "charset-configured") {
    writeStringAttribute(IPP_VALUE_TAG_CHARSET, name, "utf-8");
  } else if (name == "charset-supported") {
    writeStringAttribute(IPP_VALUE_TAG_CHARSET, name, "utf-8");
  } else if (name == "compression-supported") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "none");
  } else if (name == "document-format-default") {
    writeStringAttribute(IPP_VALUE_TAG_MIME_MEDIA_TYPE, name, "text/plain"); //TODO - get from printer?
  } else if (name == "document-format-supported") {
    writeStringAttribute(IPP_VALUE_TAG_MIME_MEDIA_TYPE, name, "text/plain"); //TODO - get from printer?
  } else if (name == "generated-natural-language-supported") {
    writeStringAttribute(IPP_VALUE_TAG_NATURAL_LANGUAGE, name, "en-us");
  } else if (name =="ipp-versions-supported") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "1.1");
  } else if (name == "natural-language-configured") {
    writeStringAttribute(IPP_VALUE_TAG_NATURAL_LANGUAGE, name, "en-us");
  } else if (name == "operations-supported") {
    write4BytesAttribute(IPP_VALUE_TAG_ENUM, name, IPP_PRINT_JOB);
    write4BytesAttribute(IPP_VALUE_TAG_ENUM, "", IPP_VALIDATE_JOB);
    //write4BytesAttribute(IPP_VALUE_TAG_ENUM, "", IPP_CANCEL_JOB);
    //write4BytesAttribute(IPP_VALUE_TAG_ENUM, "", IPP_GET_JOB_ATTRIBUTES);
    //write4BytesAttribute(IPP_VALUE_TAG_ENUM, "", IPP_GET_JOBS);
    write4BytesAttribute(IPP_VALUE_TAG_ENUM, "", IPP_GET_PRINTER_ATTRIBUTES);
  } else if (name == "pdl-override-supported") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "not-attempted");
  } else if (name == "printer-name") {
    writeStringAttribute(IPP_VALUE_TAG_NAME, name, printer->getName());
  } else if (name =="printer-is-accepting-jobs") {
    writeByteAttribute(IPP_VALUE_TAG_BOOLEAN, name, 1);
  } else if (name == "printer-state") {
    write4BytesAttribute(IPP_VALUE_TAG_ENUM, name, 3); //3 = idle
  } else if (name == "printer-state-reasons") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "none");
  } else if (name == "printer-up-time") {
    write4BytesAttribute(IPP_VALUE_TAG_INTEGER, name, millis() / 1000);
  } else if (name == "printer-uri-supported") {
    writeStringAttribute(IPP_VALUE_TAG_URI, name, "ipp://192.168.1.1"); //TODO
  } else if (name == "queued-job-count") {
    write4BytesAttribute(IPP_VALUE_TAG_INTEGER, name, 0);
  } else if (name == "uri-authentication-supported") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "none");
  } else if (name == "uri-security-supported") {
    writeStringAttribute(IPP_VALUE_TAG_KEYWORD, name, "none");
  }
}

void IppStream::handleGetPrinterAttributesRequest(std::map<String, std::set<String>> requestAttributes, Printer* printer) {
  std::set<String>& requestedAttributes = requestAttributes["requested-attributes"];

  if (requestedAttributes.size() == 0 || (requestedAttributes.find("all") != requestedAttributes.end()) || (requestedAttributes.find("printer-description") != requestedAttributes.end())) {
    requestedAttributes = allPrinterDescriptionAttributes;
  }

  write(IPP_PRINTER_ATTRIBUTES_TAG);
  for (String attributeName: requestedAttributes) {
    writePrinterAttribute(attributeName, printer);
  }

  write(IPP_END_OF_ATTRIBUTES_TAG);
}

bool IppStream::parseRequest(Printer* printer) {
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
      handleGetPrinterAttributesRequest(requestAttributes, printer);
      return false;

    case IPP_PRINT_JOB:
      Serial.println("Operation is Print-Job");
      beginResponse(IPP_SUCCESFUL_OK, requestId, *requestAttributes["attributes-charset"].begin());
      write(IPP_JOB_ATTRIBUTES_TAG);
      write4BytesAttribute(IPP_VALUE_TAG_INTEGER, "job-id", 123); //TODO
      writeStringAttribute(IPP_VALUE_TAG_URI, "job-uri", "ipp://hostname/printername/123"); //TODO
      write4BytesAttribute(IPP_VALUE_TAG_ENUM, "job-state", 5); //5 = processing
      writeStringAttribute(IPP_VALUE_TAG_KEYWORD, "job-state-reasons", "none");
      write(IPP_END_OF_ATTRIBUTES_TAG);
      flushSendBuffer();
      return true;

    case IPP_VALIDATE_JOB:
      Serial.println("Operation is Validate-Job");
      beginResponse(IPP_SUCCESFUL_OK, requestId, *requestAttributes["attributes-charset"].begin());
      write(IPP_END_OF_ATTRIBUTES_TAG);
      return false;

    default:
      Serial.println("The requested operation is not supported!");
      beginResponse(IPP_SERVER_ERROR_OPERATION_NOT_SUPPORTED, requestId, "utf-8");
      write(IPP_END_OF_ATTRIBUTES_TAG);
      return false;
  }
}
