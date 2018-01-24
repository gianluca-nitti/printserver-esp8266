/*
    This file is part of printserver-esp8266.

    printserver-esp8266 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    printserver-esp8266 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with printserver-esp8266.  If not, see <http://www.gnu.org/licenses/>.
 */

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
