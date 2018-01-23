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
