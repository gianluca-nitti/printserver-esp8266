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
#include "Settings.h"

#define SEND_BUFFER_SIZE 1024

class TcpStream {
  private:
    WiFiClient tcpConnection;
    boolean timedOut = false;
    byte sendBuffer[SEND_BUFFER_SIZE];
    int sendBufferIndex = 0;
    void waitAvailable(int numBytes);

  public:
    TcpStream(WiFiClient tcpConnection);

    virtual bool hasMoreData();
    virtual bool dataAvailable();

    virtual byte read();
    uint16_t read2Bytes();
    uint32_t read4Bytes();
    String readStringUntil(char delim);
    String readString(int length);

    void write(byte b);
    void write2Bytes(uint16_t data);
    void write4Bytes(uint32_t data);
    void print(String s);
    void flushSendBuffer();

    virtual void handleTimeout();
    ~TcpStream();
};
