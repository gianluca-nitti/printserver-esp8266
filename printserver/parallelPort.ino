#define STROBE 10
#define BUSY 9
#define STROBE_DELAY 20

const int dataPins[8] = {D0, D1, D2, D3, D4, D5, D6, D7};

void parallelPort_setup() {
  pinMode(BUSY, INPUT);
  pinMode(STROBE, OUTPUT);
  digitalWrite(STROBE, HIGH);
  for (int i = 0; i < 8; i++) {
    pinMode(dataPins[i], OUTPUT);
    digitalWrite(dataPins[i], LOW);
  }
}

void parallelPort_waitForPrinterReady() {
  while (digitalRead(BUSY) == HIGH) {
    yield();
  }
}

void parallelPort_printByte(uint8_t data) {
  parallelPort_waitForPrinterReady();
  for (int i = 0; i < 8; i++) {
    digitalWrite(dataPins[i], bitRead(data, i));
  }
  digitalWrite(STROBE, LOW);
  delayMicroseconds(STROBE_DELAY);
  digitalWrite(STROBE, HIGH);
  parallelPort_waitForPrinterReady();
}
