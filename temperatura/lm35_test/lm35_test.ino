/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-lm35-temperature-sensor
 */

#define ADC_VREF_mV    5000.0 // in millivolt
#define ADC_RESOLUTION 4096.0 // 1024.0 for 10 bits 4096.0 for 12 bits
#define LM35_1       32 // ESP32 pin GIOP36 (ADC0) connected to LM35
#define LM35_2       34
#define LM35_3       35


 
void setup() {
  Serial.begin(115200);
  analogReadResolution (12);
  pinMode(LM35_1,  INPUT);
  pinMode(LM35_2,  INPUT);
  pinMode(LM35_3,  INPUT);
}


void leer_lm35 (int sensor){
  int adcVal;
  float milliVolt, tempC;
  
  adcVal = analogRead(sensor);
  // convert the ADC value to voltage in millivolt
  milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  // convert the voltage to the temperature in °C
  tempC = milliVolt / 10;

  Serial.print("Temperatura ");
  Serial.print(sensor);
  Serial.print (" ");
  Serial.print(tempC);   // print the temperature in °C
  Serial.print("°C ");
  Serial.print("milivolts = ");
  Serial.print (milliVolt);
  Serial.print(" Sensor value = ");
  Serial.println (adcVal);
}




void loop() {
  Serial.println ("\n---------------------");
  leer_lm35 (LM35_1);
  leer_lm35 (LM35_2);
  leer_lm35 (LM35_3); 
  delay(1000);
}
