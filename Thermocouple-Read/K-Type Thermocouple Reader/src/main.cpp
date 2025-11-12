#include <Arduino.h>

int adc_pin = 33;

void setup() {
  // Configure UART
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Make pin an ADC input
  pinMode(adc_pin, INPUT);
}

void loop() {
  // Read and average multiple samples to reduce noise
  const int pin = adc_pin;
  const int samples = 1;
  long sum = 0;
  for (int i = 0; i < samples; ++i) {
    sum += analogRead(pin);
    delay(2);
  }
  float avg = (float)sum / (float)samples;

  // Convert ADC to voltage (assumes Vref ~= 3.3V)
  const float vref = 3.3f;
  const float adcMax = 4095.0f;
  float volts = (avg / adcMax) * vref; // in volts
  float mV = volts * 1000.0f;          // in millivolts

  // Convert mV to temperature using 5 mV/°C
  float temperatureC = (mV - 1250) / 5.0f;

  // Output over UART
  Serial.printf("ADC: %.1f  V: %.3f V  mV: %.2f mV  Temp: %.2f C\n", avg, volts, mV, temperatureC);

  delay(50);
}
