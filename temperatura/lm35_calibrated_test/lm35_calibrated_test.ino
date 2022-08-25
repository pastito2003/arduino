#include <Arduino.h>
#include <driver/dac.h>
#include "esp32_2_calibration.h"

#define LM35_1       32
#define ADC_RESOLUTION 4096
#define ADC_VREF_mV 5000

void setup() {
    //dac_output_enable(DAC_CHANNEL_1);        // Enable DAC on pin 25
    //dac_output_voltage(DAC_CHANNEL_1, 0);    // Setup output voltage to 0
    analogReadResolution(12);
    Serial.begin(115200);
    //while (!Serial) {}
}

float leer_lm35 (int sensor) {
  float milliVolt, tempC;
  int calibratedReading, rawReading;

  rawReading = analogRead(sensor);
  calibratedReading = (int)ADC_LUT[rawReading];
  milliVolt = calibratedReading * ADC_VREF_mV / ADC_RESOLUTION;
  //Serial.println (milliVolt);

  //for (int i=1; i<250; i++) {
      //dac_output_voltage(DAC_CHANNEL_1, i);    // DAC output (8-bit resolution)
      //delayMicroseconds(100);
      //rawReading = analogRead(sensor);                          // read value from ADC
      //calibratedReading = (int)ADC_LUT[rawReading];    // get the calibrated value from LUT
  //}
  
  
  //milliVolt = calibratedReading * (ADC_VREF_mV / ADC_RESOLUTION);
  //tempC = (milliVolt / 10) - 273.15;
  tempC = milliVolt / 10;

  return (tempC);
}

void loop() {
    Serial.print ("LM35_1: ");
    Serial.println (leer_lm35(LM35_1));
    
    delay(1000);
}
