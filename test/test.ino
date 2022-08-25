#define SWITCH1 6    //Donde esta conectado el interruptor 1
#define SWITCH2 7    //Donde esta conectado el interruptor 2
#define SALIDA1 11   //Donde dispara el rele 1
#define SALIDA2 12   //Donde dispara el rele 2


void setup() {
  Serial.begin (115200);

  pinMode(SWITCH1,INPUT);
  pinMode(SWITCH2,INPUT);
  pinMode(SALIDA1,OUTPUT);
  pinMode(SALIDA2,OUTPUT);

}

void loop() {
  int switch1, switch2;
 // Leemos los switches
  switch1 = digitalRead (SWITCH1);
  switch2 = digitalRead (SWITCH2);
  char buffer[80];

  if (switch1||switch2) {
    Serial.println ("A switch has been turned on!");
  }

  if (switch1) digitalWrite(SALIDA1,HIGH); else digitalWrite(SALIDA1,LOW);
  if (switch2) digitalWrite(SALIDA2,HIGH); else digitalWrite(SALIDA2,LOW);
  
  sprintf (buffer, "Switch1 = %d", switch1);
  Serial.println (buffer);

  sprintf (buffer, "Switch2 = %d", switch2);
  Serial.println (buffer);
  Serial.println ("=====");
  delay (500);
}
