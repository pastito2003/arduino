#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_AM2320.h>

#define DHTPIN 17
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_AM2320 am2320 = Adafruit_AM2320();

void setup() {
  Serial.begin(115200);
  dht.begin();
  am2320.begin();
}

void loop() {
  Serial.println(F("------------------------------------------"));
  Serial.print(F("AM2320 - Temperatura: "));
  Serial.print(am2320.readTemperature());
  Serial.print(F("°C humedad: "));
  Serial.print(am2320.readHumidity());
  Serial.println(F("%"));
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  Serial.print(F("DHT22  - "));
  Serial.print(F("Temperatura: "));
  Serial.print(event.temperature);
  
  dht.humidity().getEvent(&event);
  Serial.print(F("°C humedad: "));
  Serial.print(event.relative_humidity);
  Serial.println(F("%"));
  delay (1000);
}
