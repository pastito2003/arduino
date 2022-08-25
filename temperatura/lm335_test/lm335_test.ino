#define LM335_1   35
#define VOLTAJE   3300
#define RES_ADC   4095


void setup() {
  Serial.begin(115200);
  analogReadResolution (12); //in bits
  analogSetPinAttenuation(LM335_1, ADC_11db); //ADC_0db ADC_2_5db ADC_6db ADC_11db
  
  pinMode(LM335_1, INPUT);
}


void leer_lm335 (int sensor) {
  int   lectura;
  float milivolts;
  float Kelvins;
  float centigrados;
  
  lectura = analogRead(sensor);

  milivolts = (lectura * VOLTAJE) / RES_ADC;

  Kelvins = milivolts / 10;
  centigrados = Kelvins - 273.15;
  
  Serial.print ("Sensor = ");
  Serial.print (lectura);
  Serial.print (" milivolts = ");
  Serial.print (milivolts);
  Serial.print (" Kelvins = ");
  Serial.print (Kelvins);
  Serial.print (" Centigrados = ");
  Serial.print (centigrados);
  Serial.println();
}

void loop() {
  Serial.println("\n--------------");
  leer_lm335(LM335_1);
  delay(1000);
}
