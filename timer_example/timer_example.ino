
char buffer[40];
#define LED LED_BUILTIN

void setup() {
  Serial.begin (115200);

  pinMode(LED,OUTPUT);
  Serial.println ("Starting up...");
}

void loop() {
  static uint8_t tog=0;
  static uint32_t oldtime=millis();

    if ( (millis()-oldtime) > 500) {
       oldtime = millis();

       tog = ~tog; // Invert
       if (tog) digitalWrite(LED,HIGH); else digitalWrite(LED,LOW);
       
       sprintf (buffer, "Board up for %lu seconds", millis()/1000);
       Serial.println (buffer);
    }
  
}
