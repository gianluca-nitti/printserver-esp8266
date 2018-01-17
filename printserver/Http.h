#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h> // TODO remove
#include <map>

typedef struct {
  bool parseSuccess = false;
  String httpMethod = "";
  String path = "";
  int contentLength = 0;
} http_req_t;

class Http {
  public:
    static byte blockingRead(Stream* source);
    static http_req_t parseRequestHeader(WiFiClient* c);
    static std::map<String, String> parseUrlencodedRequestBody(WiFiClient* c, int contentLength);
};
