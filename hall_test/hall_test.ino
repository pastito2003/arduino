
const int hall_sensor = 36;
int hall_valor = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  hall_valor = analogRead(hall_sensor);
  Serial.println(hall_valor);
  delay(500);
}
