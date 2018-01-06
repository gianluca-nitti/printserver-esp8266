#include <Arduino.h>

typedef enum {
  IDLE,
  PRINTING_FROM_SERVER,
  PRINTING_FROM_QUEUE
} printer_status;

class Printer {
  private:
    printer_status stat;
    int printingClientId;
  public:
    boolean canAcceptJob(int s);
    void startJob(int clientId);
    void endJob(int clientId);
    boolean canPrint(int clientId);
    void printByte(int clientId, byte b);
    void processQueue();
    virtual String getInfo() = 0;
    virtual void printByte(byte b) = 0;
};
