#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include "Settings.h"

class TcpStream {
  private:
    WiFiClient tcpConnection;
    boolean timedOut = false;

    void waitAvailable(int numBytes);
  public:
    TcpStream(WiFiClient tcpConnection);

    bool connected();
    virtual bool hasMoreData();
    virtual byte read();
    uint16_t read2Bytes();
    uint32_t read4Bytes();
    String readStringUntil(char delim);
    String readString(int length);

    void write(byte b);
    void write2Bytes(uint16_t data);
    void write4Bytes(uint32_t data);
    void print(String s);

    virtual void handleTimeout();
    ~TcpStream();
};
