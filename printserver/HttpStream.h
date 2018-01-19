#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <map>
#include "Settings.h"
#include "TcpStream.h"

#define STRLEN(s) ((sizeof(s) / sizeof(s[0])) - 1)

#define CONTENT_LENGTH_HEADER "content-length: "
#define CHUNKED_ENCODING_HEADER "transfer-encoding: chunked"

class HttpStream: public TcpStream {
  private:
    String requestMethod = "";
    String requestPath = "";
    int requestContentLength = 0;
    bool requestChunkedEncoded = false;
    int remainingChunkBytes = 0;

    void parseNextChunkLength();
  public:
    HttpStream(WiFiClient conn);

    byte read();
    bool hasMoreData();

    bool parseRequestHeader();
    std::map<String, String> parseUrlencodedRequestBody();
    String getRequestMethod();
    String getRequestPath();
};
