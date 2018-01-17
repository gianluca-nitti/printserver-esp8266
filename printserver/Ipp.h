#pragma once
#include <Arduino.h>
#include <WiFiClient.h>

#define IPP_SUPPORTED_VERSION 0x0101

#define IPP_ERROR_VERSION_NOT_SUPPORTED 0x0503

#define IPP_END_OF_ATTRIBUTES_TAG 0x03

class Ipp {
  public:
    static void parseRequest(WiFiClient* c); //TODO return bool
};
