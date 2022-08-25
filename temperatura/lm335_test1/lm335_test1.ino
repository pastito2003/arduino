/*
 * Rui Santos
 * Complete Project Details https://RandomNerdTutorials.com
 */
 
const int sensorPin = 33; 
float sensorValue;
float voltageOut;

float temperatureC;
float temperatureF;
float temperatureK;

// uncomment if using LM335
//float temperatureK;

void setup() {
  pinMode(sensorPin, INPUT);
  analogSetPinAttenuation(sensorPin, ADC_11db); //ADC_0db ADC_2_5db ADC_6db ADC_11db
  Serial.begin(115200);
}

void loop() {
  sensorValue = analogRead(sensorPin);
  voltageOut = (sensorValue * 3300) / 4096;
  
  // calculate temperature for LM35 (LM35DZ)
  //temperatureC = voltageOut / 10;
  //temperatureF = (temperatureC * 1.8) + 32;

  // calculate temperature for LM335
  temperatureK = voltageOut / 10;
  temperatureC = temperatureK - 273;
  temperatureF = (temperatureC * 1.8) + 32;

  // calculate temperature for LM34
  //temperatureF = voltageOut / 10;
  //temperatureC = (temperatureF - 32.0)*(5.0/9.0);
  Serial.print("Sensor = ");
  Serial.print(sensorValue);
  Serial.print(" Temperature(ºC): ");
  Serial.print(temperatureC);
  Serial.print("  Temperature(ºF): ");
  Serial.print(temperatureF);
  Serial.print("  Voltage(mV): ");
  Serial.println(voltageOut);
  delay(1000);
}
