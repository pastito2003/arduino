void setup() {
  // put your setup code here, to run once:

}

void loop() {
  x = (99.0 * x + analogRead(PIN)) / 100.0;
  float y = (x * 4095 / 1134.0);
  Serial.println(y,2);

  /* For ADS1115
  x=ads.readADC_SingleEnded(0);
  float y = (x * 12.2929 / 65536.0);
  Serial.println (y,4);
  */
}
