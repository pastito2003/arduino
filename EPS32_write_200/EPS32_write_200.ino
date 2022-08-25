

void setup() {
  Serial.begin(115200);
  analogReadResolution (12); //in bits
  ledcSetup(0,5000,8);
  ledcAttachPin(34, 0);
  
}

void loop() {
  ledcWrite (0, 200);

}
