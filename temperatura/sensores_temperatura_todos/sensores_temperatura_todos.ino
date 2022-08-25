#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <OneWire.h>                //OneWire librerias para leer el termometro
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4

uint8_t sensor_adentro[8] =  { 0x28, 0xFF, 0xBD, 0xB4, 0x00, 0x16, 0x02, 0x8E }; //trucho to92

// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Crear objeto de la clase
Adafruit_ADS1115 ads;

float sensorValue;
float voltageOut;
float temperatureC;
float temperatureF;
float temperatureK;

#define LM35_1       32 // ESP32 pin GIOP36 (ADC0) connected to LM35
#define LM35_2       34
#define LM35_3       35
 
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
  
  analogReadResolution (12);
  pinMode(LM35_1,  INPUT);
  pinMode(LM35_2,  INPUT);
  pinMode(LM35_3,  INPUT);
  
  digitalWrite (ONE_WIRE_BUS, LOW);
  pinMode (ONE_WIRE_BUS, INPUT);
  sensors.begin();
  delay (20);
  sensors.requestTemperatures(); //Comando para leer temperaturas
}

void leer_lm35 (int sensor){
  int adcVal;
  float milliVolt, tempC;
  
  adcVal = analogRead(sensor);
  // convert the ADC value to voltage in millivolt
  milliVolt = adcVal * (5000 / 4096);
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


float get_temp_ads1115_lm335 (int sensor){
  sensorValue = ads.readADC_SingleEnded(sensor);

  voltageOut = (sensorValue * 4096) / 32768;
  // Obtener datos del A0 del ADS1115

  // calculate temperature for LM335
  temperatureK = voltageOut / 10;
  temperatureC = temperatureK - 273.15;
  temperatureF = (temperatureC * 1.8) + 32;
}

float get_temp_ads1115 (int sensor){
  
  sensorValue = ads.readADC_SingleEnded(sensor);
  
  voltageOut = (sensorValue * 4096) / 32768;

   // calculate temperature for LM35
  temperatureC = voltageOut / 10;
  temperatureK = temperatureC + 273.15;
  temperatureF = (temperatureC * 1.8) + 32;
}

void imprimir_serial (char *sensor){
  Serial.print("Sensor ");
  Serial.print(sensor);
  Serial.print(" = ");
  Serial.print(sensorValue);
  Serial.print("  Voltage(mV): ");
  Serial.print(voltageOut);
  Serial.print(" Temperature(ºC): ");
  Serial.print(temperatureC);
  Serial.print("  Temperature(ºF): ");
  Serial.println(temperatureF);
}


void loop(void) 
{
  get_temp_ads1115_lm335(0);
  imprimir_serial ("lm335_1");
  get_temp_ads1115_lm335(1);
  //get_temp_ads1115(1);
  imprimir_serial ("lm335_2");
  leer_lm35 (LM35_1);
  leer_lm35 (LM35_2);
  //leer_lm35 (LM35_3);
  
  Serial.print(F("Temperatura DS18B20: "));
  sensors.requestTemperatures(); //Comando para leer temperaturas
  Serial.println(sensors.getTempC(sensor_adentro));
  
  Serial.println("-------------------------------------------------");
  
  delay(1000);
}
