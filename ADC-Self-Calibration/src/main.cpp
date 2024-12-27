#include <Arduino.h>
#include <driver/dac.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DAC_CHANNEL 25 // This is equivalent to DAC_CHANNEL_1 (GPIO 25)
#define ADC_CHANNEL 36

#define DELAY_CONSTANT 2500

void get_print_test_data();
void get_calibration_data();

void setup() {
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_voltage(DAC_CHANNEL_1, 0);
  analogReadResolution(8);
  Serial.begin(115200);

  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 0, &adc_chars);

  Serial.println("ADC Characteristics");
  Serial.print("Value Type: ");
  Serial.println(val_type);
  Serial.print("Coeff A: ");
  Serial.println(adc_chars.coeff_a);
  Serial.print("Coeff B: ");
  Serial.println(adc_chars.coeff_b);
  Serial.print("Vref: ");
  Serial.println(adc_chars.vref);
  Serial.print("Bit Width: ");
  Serial.println(adc_chars.bit_width);
  Serial.print("Atten: ");
  Serial.println(adc_chars.atten);
  Serial.print("Version: ");
  Serial.println(adc_chars.version);

  adc1_config_width(ADC_WIDTH_BIT_12);

  get_calibration_data();
}

void loop() {
  //get_calibration_data();
}

void get_print_test_data(){

  dac_output_voltage(DAC_CHANNEL_1, 15);

  delay(DELAY_CONSTANT);

  int adcValue = analogRead(ADC_CHANNEL);
  Serial.println((double)adcValue/255.0 * 3.3);

  delay(DELAY_CONSTANT);

  dac_output_voltage(DAC_CHANNEL_1, 250);

  delay(DELAY_CONSTANT);

  adcValue = analogRead(ADC_CHANNEL);
  Serial.println((double)adcValue/255.0 * 3.3);

  delay(DELAY_CONSTANT);
}

void get_calibration_data() {
  for (int i=0; i<255; i++) {
    dac_output_voltage(DAC_CHANNEL_1, i); // DAC output (8-bit resolution)
    delayMicroseconds(100);
    int rawReading = adc1_get_raw(ADC1_CHANNEL_0);

    // Run Serial Plotter to see the results
    Serial.print(i);
    Serial.print(F(","));
    Serial.println(rawReading);
    delay(10);
  }
}
