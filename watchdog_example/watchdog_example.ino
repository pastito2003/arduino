#include <avr/wdt.h>
int i;


void setup() {
  Serial.begin (115200);
  wdt_disable(); //Disable para que nos de tiempo de inicializar todo antes de que se resetee
  wdt_enable(WDTO_4S); // Habilitar al ultimo
/* Valid times
WDTO_15MS
WDTO_30MS
WDTO_60MS
WDTO_120MS
WDTO_250MS
WDTO_500MS
WDTO_1S
WDTO_2S
WDTO_4S
WDTO_8S
*/
  
}

void loop() {
  wdt_reset();

  i = i + 100; //El delay va a ir aumentando en 100 milisegundos... al llegar a mas de 4 segundos el WDT reinicia la placa

  Serial.println (i);
  delay (i);

}
