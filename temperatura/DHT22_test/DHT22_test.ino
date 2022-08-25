#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 17
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  Serial.print(F("DHT22 - "));
  Serial.print(F("Temperatura: "));
  Serial.print(event.temperature);
  Serial.print(F("°C "));

  dht.humidity().getEvent(&event);
  Serial.print(F("umidade: "));
  Serial.print(event.relative_humidity);
  Serial.println(F("%"));
  delay (1000);
}
