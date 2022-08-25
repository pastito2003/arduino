#include <Wire.h>
#include <Adafruit_ADS1X15.h>
 
// Crear objeto de la clase
Adafruit_ADS1115 ads;
 
void setup(void) 
{
  Serial.begin(115200);
  delay(200);
 
  /* Cambiar factor de escala
  GAIN_TWOTHIRDS =  6.144V  0.0001875 (default)
  GAIN_ONE = 4.096V  0.000125
  GAIN_TWO = 2.048V
  GAIN_FOUR = 1.024V
  GAIN_EIGHT = 0.512V
  GAIN_SIXTEEN = 0.256V
  */
  
  ads.setGain(GAIN_ONE);

  // Iniciar el ADS1115
  ads.begin();
}




void loop(void) 
{
  float sensorValue;
  float voltageOut;
  float temperatureC;
  float temperatureF;
  float temperatureK;

  sensorValue = ads.readADC_SingleEnded(0);
  
  voltageOut = (sensorValue * 4096) / 32768;
  // Obtener datos del A0 del ADS1115
  

  // calculate temperature for LM335
  temperatureK = voltageOut / 10;
  temperatureC = temperatureK - 273.15;
  temperatureF = (temperatureC * 1.8) + 32;

  Serial.print("Sensor = ");
  Serial.print(sensorValue);
  Serial.print("  Voltage(mV): ");
  Serial.print(voltageOut);
  Serial.print(" Temperature(ºC): ");
  Serial.print(temperatureC);
  Serial.print("  Temperature(ºF): ");
  Serial.println(temperatureF);
  
  
  delay(1000);
}
