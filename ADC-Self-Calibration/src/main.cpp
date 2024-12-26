#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World");

  pinMode(14, OUTPUT);
}

void loop() {
  digitalWrite(14, HIGH);
  delay(1000);
  digitalWrite(14, LOW);
  delay(1000);
}
