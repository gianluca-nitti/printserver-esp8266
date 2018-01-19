#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <map>
#include "Settings.h"

#define STRLEN(s) ((sizeof(s) / sizeof(s[0])) - 1)

#define CONTENT_LENGTH_HEADER "content-length: "
#define CHUNKED_ENCODING_HEADER "transfer-encoding: chunked"

class HttpStream {
  private:
    WiFiClient* tcpConnection;
    boolean timedOut = false;

    String requestMethod = "";
    String requestPath = "";
    int requestContentLength = 0;
    bool requestChunkedEncoded = false;
    int remainingChunkBytes = 0;

    void waitAvailable(int numBytes);
    void parseNextChunkLength();
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

    bool parseRequestHeader();
    std::map<String, String> parseUrlencodedRequestBody();

    String getRequestMethod();
    String getRequestPath();
};
