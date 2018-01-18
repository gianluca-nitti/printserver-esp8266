#pragma once
#include <Arduino.h>
#include <WiFiClient.h>

#define IPP_SUPPORTED_VERSION 0x0101

#define IPP_SUCCESFUL_OK 0x0000
#define IPP_ERROR_VERSION_NOT_SUPPORTED 0x0503

#define IPP_OPERATION_ATTRIBUTES_TAG 0x01
#define IPP_JOB_ATTRIBUTES_TAG 0x02
#define IPP_END_OF_ATTRIBUTES_TAG 0x03
#define IPP_PRINTER_ATTRIBUTES_TAG 0x04
#define IPP_UNSUPPORTED_ATTRIBUTES_TAG 0x05

#define IPP_VALUE_TAG_UNSUPPORTED 0x10
#define IPP_VALUE_TAG_CHARSET 0x47
#define IPP_VALUE_TAG_NATURAL_LANGUAGE 0x48

#define IPP_PRINT_JOB 0x0002
#define IPP_GET_PRINTER_ATTRIBUTES 0x000B

typedef struct {
  byte valueTag;
  String value;
} attribute_value_t;

class Ipp {
  public:
    static void parseRequest(WiFiClient* c); //TODO return bool
};
