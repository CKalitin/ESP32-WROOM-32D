#include <Arduino.h>

const int adcBufferLength = 50;
int adcBufferIndex = 0;
int adcReadDelay = 10; // ms

int adc1Buffer[adcBufferLength];
int adc2Buffer[adcBufferLength];

int getAverage(int* buffer, int length);
double getErrorInVolts(double adcVoltage);

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World");

  pinMode(32, OUTPUT);
}

void loop() {
  digitalWrite(32, HIGH);
  Serial.println("HIGH");
  delay(1000);
  digitalWrite(32, LOW);
  Serial.println("LOW");
  delay(1000);
}

int getAverage(int* buffer, int length) {
  int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += buffer[i];
  }
  return sum / length;
}

double getErrorInVolts(double adcValue) {
  // Error Formula: -0.115 + -0.164x + 0.171x^2 + -0.0927x^3 + 0.0191x^4
  return -0.115 + (-0.164 * adcValue) + (0.171 * pow(adcValue, 2)) + (-0.0927 * pow(adcValue, 3)) + (0.0191 * pow(adcValue, 4));

  // Translate to a google sheets formula at H78:
  // =-0.115 + (-0.164 * H77) + (0.171 * H77^2) + (-0.0927 * H77^3) + (0.0191 * H77^4)
}