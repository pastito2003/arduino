#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_AM2320.h>
#include <OneWire.h>                //OneWire librerias para leer el termometro
#include <DallasTemperature.h>

#define DHTPIN 17
#define DHTTYPE    DHT22
#define ONE_WIRE_BUS 4
#define LM35_1       32
#define LM35_2       34
#define ADC_VREF_mV    5000.0 // in millivolt
#define ADC_RESOLUTION 4095.0
#define LM335 35

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_AM2320 am2320 = Adafruit_AM2320();
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//uint8_t sensor_adentro[8] =  { 0x28, 0x98, 0x61, 0x9F, 0x0D, 0x00, 0x00, 0xB9 };
uint8_t sensor_adentro[8] =  { 0x28, 0xFF, 0xBD, 0xB4, 0x00, 0x16, 0x02, 0x8E }; //trucho to92

void setup() {
  Serial.begin(115200);
  analogReadResolution (12); //in bits
  //analogSetPinAttenuation(LM335_1, ADC_11db); //ADC_0db ADC_2_5db ADC_6db ADC_11db
  dht.begin();
  am2320.begin();
  digitalWrite (ONE_WIRE_BUS, LOW);
  pinMode (ONE_WIRE_BUS, INPUT);
  sensors.begin();
  pinMode(LM35_1,  INPUT);
  pinMode(LM35_2,  INPUT);
}

float leer_lm35 (int sensor) {
  int adcVal;
  float milliVolt, tempC;
  
  adcVal = analogRead(sensor);
  milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  tempC = milliVolt / 10;

  return (tempC);
}

float leer_lm335 (int sensor) {
  int adcVal;
  float milliVolt, tempC;
  
  adcVal = analogRead(sensor);
  milliVolt = adcVal * (3300 / ADC_RESOLUTION);
  tempC = (milliVolt / 10) - 273.15;

  return (tempC);
}

void loop() {
  Serial.println(F("----------------------------------------------"));
  Serial.print(F("AM2320  - Temperatura: "));
  Serial.print(am2320.readTemperature());
  Serial.print(F("°C humedad: "));
  Serial.print(am2320.readHumidity());
  Serial.println(F("%"));
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  Serial.print(F("DHT22   - "));
  Serial.print(F("Temperatura: "));
  Serial.print(event.temperature);
  
  dht.humidity().getEvent(&event);
  Serial.print(F("°C humedad: "));
  Serial.print(event.relative_humidity);
  Serial.println(F("%"));

  sensors.requestTemperatures();
  Serial.print (F("DS10B20 - Temperatura: "));
  Serial.print(sensors.getTempC(sensor_adentro));
  Serial.println(F("°C"));

  Serial.print(F("LM35_1  - Temperatura: "));
  Serial.print(leer_lm35(LM35_1));
  Serial.println(F("°C"));

  Serial.print(F("LM35_2  - Temperatura: "));
  Serial.print(leer_lm35(LM35_2));
  Serial.println(F("°C"));

  Serial.print(F("LM335   - Temperatura: "));
  Serial.print(leer_lm335(LM335));
  Serial.println(F("°C"));
  
  delay (1000);
}
