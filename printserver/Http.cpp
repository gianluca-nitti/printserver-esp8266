#include "Http.h"

#define STRLEN(s) ((sizeof(s) / sizeof(s[0])) - 1)
#define CONTENT_LENGTH_HEADER "Content-Length: "

byte Http::blockingRead(Stream* source) {
  // TODO: timeout
  while (source->available() == 0) {
    yield();
  }
  return source->read();
}

http_req_t Http::parseRequestHeader(WiFiClient* c) {
  http_req_t result;
  result.parseSuccess = false;
  c->setTimeout(5000);
  result.httpMethod = c->readStringUntil(' ');
  if (result.httpMethod == "") {
    return result;
  }
  result.path = c->readStringUntil(' ');
  if (result.path == "") {
    return result;
  }

  c->readStringUntil('\r');
  c->read(); //consume the '\n'

  String header;
  while ((header = c->readStringUntil('\r')) != "") {
    if (header.startsWith(CONTENT_LENGTH_HEADER)) {
      result.contentLength = header.substring(STRLEN(CONTENT_LENGTH_HEADER)).toInt();
    }
    c->read(); //consume the '\n'
  }
  c->read(); //consume the '\n'

  result.parseSuccess = true;
  return result;
}

std::map<String, String> Http::parseUrlencodedRequestBody(WiFiClient* c, int contentLength) {
  std::map<String, String> result;
  int parsedBytes = 0;
  while (parsedBytes < contentLength) {
    String key, value;
    key.reserve(128);
    value.reserve(128);
    char readChar;
    while (parsedBytes < contentLength && ((readChar = blockingRead(c)) != '=')){
      key += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '='
    while (parsedBytes < contentLength && ((readChar = blockingRead(c)) != '&')){
      value += readChar;
      parsedBytes++;
    }
    parsedBytes++; // '&'
    result[key] = value;
  }
  return result;
}
