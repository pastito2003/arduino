//Libraries
#include <EEPROM.h>//https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM
//Constants
#define EEPROM_SIZE 12
void setup() {
  //Init Serial USB
  Serial.begin(115200);
  Serial.println(F("Initialize System"));
  //Init EEPROM
  EEPROM.begin(EEPROM_SIZE);
  //Write data into eeprom
  int encendido;
  int temperatura;
  int address = 0;
  /*
  EEPROM.write(address, encendido);//EEPROM.put(address, boardId);
  address += sizeof(encendido); //update address value
  
  //EEPROM.writeFloat(address, param);//EEPROM.put(address, param);
  EEPROM.write(address, temperatura);//EEPROM.put(address, param);
  EEPROM.commit();
  */
  //Read data from eeprom
  address = 0;
  
  encendido = EEPROM.read(address); //EEPROM.get(address,readId);
  Serial.print("Encendido = ");
  Serial.println(encendido);
  address += sizeof(encendido); //update address value

  temperatura = EEPROM.read(address);
  //EEPROM.get(address, temperatura); //readParam=EEPROM.readFloat(address);
  Serial.print("Temperatura = ");
  Serial.println(temperatura);
  EEPROM.end();
}
void loop() {}
