#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <map>
#include <set>
#include "HttpStream.h"

#define IPP_SUPPORTED_VERSION 0x0101

#define IPP_SUCCESFUL_OK 0x0000
#define IPP_CLIENT_ERROR_BAD_REQUEST 0x0400
#define IPP_SERVER_ERROR_OPERATION_NOT_SUPPORTED 0x0501
#define IPP_SERVER_ERROR_VERSION_NOT_SUPPORTED 0x0503

#define IPP_OPERATION_ATTRIBUTES_TAG 0x01
#define IPP_JOB_ATTRIBUTES_TAG 0x02
#define IPP_END_OF_ATTRIBUTES_TAG 0x03
#define IPP_PRINTER_ATTRIBUTES_TAG 0x04
#define IPP_UNSUPPORTED_ATTRIBUTES_TAG 0x05

#define IPP_VALUE_TAG_UNSUPPORTED 0x10
#define IPP_VALUE_TAG_INTEGER 0x21
#define IPP_VALUE_TAG_BOOLEAN 0x22
#define IPP_VALUE_TAG_ENUM 0x23
#define IPP_VALUE_TAG_TEXT 0x41
#define IPP_VALUE_TAG_NAME 0x42
#define IPP_VALUE_TAG_KEYWORD 0x44
#define IPP_VALUE_TAG_URI 0x45
#define IPP_VALUE_TAG_CHARSET 0x47
#define IPP_VALUE_TAG_NATURAL_LANGUAGE 0x48
#define IPP_VALUE_TAG_MIME_MEDIA_TYPE 0x49

#define IPP_PRINT_JOB 0x0002
#define IPP_VALIDATE_JOB 0x0004
#define IPP_CANCEL_JOB 0x0008
#define IPP_GET_JOB_ATTRIBUTES 0x0009
#define IPP_GET_JOBS 0x000A
#define IPP_GET_PRINTER_ATTRIBUTES 0x000B

class IppStream: public HttpStream {
  private:
    std::map<String, std::set<String>> parseRequestAttributes();
    void beginResponse(uint16_t statusCode, uint32_t requestId, String charset);

    void writeStringAttribute(byte valueTag, String name, String value);
    void writeByteAttribute(byte valueTag, String name, byte value);
    void write2BytesAttribute(byte valueTag, String name, uint16_t value);
    void write4BytesAttribute(byte valueTag, String name, uint32_t value);

    void writePrinterAttribute(String name);
    void handleGetPrinterAttributesRequest(std::map<String, std::set<String>> requestAttributes);

  public:
    IppStream(WiFiClient conn);
    bool parseRequest();
};
