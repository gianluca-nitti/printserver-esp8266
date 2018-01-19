#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <map>
#include "Settings.h"

#define STRLEN(s) ((sizeof(s) / sizeof(s[0])) - 1)
#define CONTENT_LENGTH_HEADER "Content-Length: "

typedef struct {
  bool parseSuccess = false;
  String httpMethod = "";
  String path = "";
  int contentLength = 0;
} http_req_t;

class HttpStream {
  private:
    WiFiClient* tcpConnection;
    boolean timedOut = false;
    void waitAvailable(int numBytes);
  public:
    HttpStream(WiFiClient* tcpConnection);
    byte read();
    uint16_t read2Bytes();
    uint32_t read4Bytes();
    String readStringUntil(char delim);
    String readString(int length);

    void write(byte b);
    void write2Bytes(uint16_t data);
    void write4Bytes(uint32_t data);
    void print(String s);

    void stop();

    http_req_t parseRequestHeader();
    std::map<String, String> parseUrlencodedRequestBody(int contentLength);
};
