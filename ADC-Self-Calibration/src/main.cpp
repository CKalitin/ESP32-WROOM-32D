#include <Arduino.h>

#define DAC_CHANNEL 25
#define ADC_CHANNEL 36

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World");
}

void loop() {
  dacWrite(DAC_CHANNEL, 8);

  delay(25);

  int adcValue = analogRead(ADC_CHANNEL);
  Serial.println(adcValue);

  delay(25);

  dacWrite(DAC_CHANNEL, 250);

  delay(25);

  adcValue = analogRead(ADC_CHANNEL);
  Serial.println(adcValue);

  delay(25);
}
