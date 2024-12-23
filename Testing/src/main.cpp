#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(14, OUTPUT);
}

void loop() {
  Serial.println("Hello World");
  digitalWrite(14, HIGH);
  delay(1000);
  digitalWrite(14, LOW);
  delay(1000);
}
