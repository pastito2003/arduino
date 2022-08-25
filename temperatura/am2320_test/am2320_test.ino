#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

Adafruit_AM2320 am2320 = Adafruit_AM2320();

void setup() {
  Serial.begin(115200);
  am2320.begin();
}

void loop() {
  Serial.print("Temperatura: ");
  Serial.print(am2320.readTemperature());
  Serial.print(" humedad: ");
  Serial.println(am2320.readHumidity());
  delay (1000);
}
