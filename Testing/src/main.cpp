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

  pinMode(14, OUTPUT);
}

void loop() {
  if (adcBufferIndex >= adcBufferLength) adcBufferIndex = 0;

  int adc1Value = analogRead(36);
  adc1Buffer[adcBufferIndex] = adc1Value;

  int adc2Value = analogRead(12);
  adc2Buffer[adcBufferIndex] = adc2Value;

  adcBufferIndex++;

  if (adcBufferIndex % 50 == 0) { 
    int adc1Average = getAverage(adc1Buffer, adcBufferLength);
    int adc2Average = getAverage(adc2Buffer, adcBufferLength);

    double adc1Voltage = (double)adc1Average / 4095.0 * 3.3;
    double adc2Voltage = (double)adc2Average / 4095.0 * 3.3;

    double adc1Error = getErrorInVolts(adc1Voltage);
    double adc2Error = getErrorInVolts(adc2Voltage);

    double adc1Corrected = adc1Voltage - adc1Error;
    double adc2Corrected = adc2Voltage - adc2Error;

    Serial.println("ADC1: (" + String(adc1Voltage) + ", " + String(adc1Corrected) + "). ADC2: (" + String(adc2Voltage) + ", " + String(adc2Corrected) + ").");
  }

  delay(adcReadDelay);
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