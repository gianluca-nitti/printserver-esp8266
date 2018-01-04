#define STROBE 10
#define BUSY 9
#define STROBE_DELAY 20

const int dataPins[8] = {D0, D1, D2, D3, D4, D5, D6, D7};


void setup() {
  Serial.begin(115200);
  Serial.println("pin setup...");
  pinMode(BUSY, INPUT);
  pinMode(STROBE, OUTPUT);
  digitalWrite(STROBE, HIGH);
  for (int i = 0; i < 8; i++) {
    pinMode(dataPins[i], OUTPUT);
    digitalWrite(dataPins[i], LOW);
  }
  Serial.println("pin setup ok");
  Serial.println("ready!");
  /*printByte(27);
  printByte('E');
  printByte('A');
  printByte(27);
  printByte('E');*/
}

void waitForPrinterReady() {
  //Serial.println("waiting...");
  while (digitalRead(BUSY) == HIGH) {
    yield();
  }
  //Serial.println("ready to print byte");
}

void printByte(uint8_t data) {
  waitForPrinterReady();
  for (int i = 0; i < 8; i++) {
    digitalWrite(dataPins[i], bitRead(data, i));
  }
  digitalWrite(STROBE, LOW);
  delayMicroseconds(STROBE_DELAY);
  digitalWrite(STROBE, HIGH);
  waitForPrinterReady();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
      uint8_t data = Serial.read();

      Serial.write(data);
      printByte(data);
      Serial.write(data);
  }
  delay(500);
}
