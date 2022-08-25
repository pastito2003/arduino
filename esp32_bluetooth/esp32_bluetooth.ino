#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#define LED 2

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LED, OUTPUT);
}

void loop() {
  //char linea = 0;
  char linea[80];
  
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    //Serial.write(SerialBT.read());

    String linea = SerialBT.readString(); //Leer lo que nos viene por Bluetooth, convertirlo en objeto String en la variable linea

    //linea = Serial.read();

    if (linea.startsWith("1")) {
      digitalWrite (LED, HIGH);
      Serial.println (F("Led encendido"));
    }

    if (linea.startsWith("0")) {
      digitalWrite (LED, LOW);
      Serial.println (F("Led apagado"));
    }
    if (linea.startsWith("2")) {
      Serial.println ("Reiniciando...");
      ESP.restart();
    }
    
    Serial.print(linea);
  }
  delay(20);
}
