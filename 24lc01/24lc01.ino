#include <Wire.h>

#define chipAddress 80

int i = 0;

byte readFrom (int chAddress, unsigned int ceAddress) {
   Wire.beginTransmission (chAddress);
   Wire.write ((int)(ceAddress >>8));    //MSB
   Wire.write ((int)(ceAddress & 0xFF)); //LSB 
   Wire.endTransmission();
   
   Wire.requestFrom (chAddress, 1);
   byte rData = 0;
   
   if (Wire.available()) {
     rData = Wire.read(); 
   }
   return rData;
}

void writeTo (int chAddress,unsigned int ceAddress, byte wData) {
   Wire.beginTransmission (chAddress);
   Wire.write ((int)(ceAddress >>8));    //MSB
   Wire.write ((int)(ceAddress & 0xFF)); //LSB 
   Wire.write (wData);
   Wire.endTransmission();
   delay (10);
}

void setup () {
  Serial.begin (9600);
  Wire.begin();
  Serial.println ("Reading...");
  
  unsigned cellAddress = 10;
  writeTo (chipAddress, cellAddress, 128);
  delay (1000);
  //Serial.print(readFrom(chipAddress, cellAddress), DEC);  
  
  for (i=0;i <128;++i) {
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(readFrom(chipAddress, i), DEC); 
    delay (10);
  }
  
  
}

void loop () { 

  //Serial.print(i);
  //Serial.print(readFrom(chipAddress, i), DEC);  
  //i++;

}


