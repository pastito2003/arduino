/*
 * Programa para el cubre colchón eléctrico por el oso loco el 16/4/2022, version 0.1
 * Como tiene dos plazas, hay que hacer todo doble. No quise escribir una subrutina porque había
 * que pasarle tantas variables que no tenía sentido
 * 
 * La idea es definir un ciclo de trabajo y con los potenciometros manejar cuanto tiempo encendido
 * y cuanto tiempo apagado (dentro del ciclo)
 * 
 * Tiene una funcion de timeout para que si te lo olvidas encendido, se apague solo hasta un nuevo reset del switch
 * Todo eso es independiente en cada plaza
 */

#include <avr/wdt.h> // Cargamos librería del watchdog timer

#define DEBUG        //Definir esta variable para imprimir info de debug en el serial
                     //Si comentamos esta línea el programa queda minusculo y mucho mas
                     //exacto en el timing de los segundos
#define CICLO 120    //Definir el ciclo de trabajo en segundos
#define POTE1 A0     //Donde esta conectado el potenciometro 1
#define POTE2 A1     //Donde esta conectado el potenciometro 2
#define SWITCH1 6    //Donde esta conectado el interruptor 1
#define SWITCH2 7    //Donde esta conectado el interruptor 2
#define SALIDA1 11   //Donde dispara el rele 1
#define SALIDA2 12   //Donde dispara el rele 2

#ifdef DEBUG
  char buffer[80];                    //Variables para imprimir, solo la necesitamos si DEBUG esta definido
  char hora[20];
#endif

//Definimos variables globales
int pote1 = 0, pote2 = 0;
unsigned long MAXTIME = 10;           //Definir maximo tiempo de encendido en horas
unsigned long intervalo;              //Definimos el maximo tiempo de encendido permitido en milisegundos
unsigned long milisPrevio1 = 0;       //Aca guardamos los milisegundos previos de 1
unsigned long milisPrevio2 = 0;       //Aca guardamos los milisegundos previos de 2
unsigned long segs1 = 0, segs2 = 0;   //Estos son los contadores de segundos de 1 y 2, los ponemos acá para asegurarnos que se inicializan en 0


void setup() {
  Serial.begin (115200);               //Definimos la velocidad del link serial de consola
  pinMode(POTE1, INPUT);               //Definimos las entradas analógicas y digitales
  pinMode(POTE2, INPUT);
  pinMode(SWITCH1,INPUT);
  pinMode(SWITCH2,INPUT);
  pinMode(SALIDA1,OUTPUT);
  pinMode(SALIDA2,OUTPUT);
  
  delay (1000);                        //Le damos tiempo a que se estabilice el link serial
  
#ifdef DEBUG
  Serial.println();
  Serial.println();
  Serial.println("*****************************************");
  Serial.println ("Arrancando el cubre colchon electrico!!!");
  Serial.println("*****************************************");
#endif
  
  intervalo = MAXTIME * 60 * 60 * 1000;  //Calculamos el intervalo en milisegundos
  //intervalo = MAXTIME * 30 * 1000;      //Definir un ciclo corto para debuggear el rollover
  
  wdt_disable();                         //Disable para que nos de tiempo de inicializar todo antes de que se resetee
  wdt_enable(WDTO_4S);                   //Habilitar al ultimo... 4S de timeout
}

//Funcion para parsear los milisegundos e imprimirlos legibles en dias:horas:minutos:segundos
#ifdef DEBUG
void parse_millis (unsigned long milisegundos) {
   unsigned long segundos;
   long minutos;
   int horas, dias;

   segundos = milisegundos / 1000;
   minutos = segundos / 60;
   horas = minutos / 60;
   dias = horas / 24;
   segundos %= 60;
   minutos %= 60;
   horas %= 24;

   sprintf (hora, "%02d:%02d:%02ld:%02ld", dias, horas, minutos, segundos);
}
#endif

//Loop principal
void loop() {
  bool TMOUT1 = false;
  bool TMOUT2 = false;
  int segon1, segon2, segoff1, segoff2, switch1, switch2;
  unsigned long milisAhora = millis();
  
  wdt_reset(); // Resetear el watchdog timer o se reinicia el microcontrolador!
  
  //Revisar si estuvimos encendidos durante el tiempo especificado en TMOUT. Si es verdadero, apagar
  //Notese la belleza implicita de esta simple resta y comparación que maneja el overflow del millis()
  //con elegancia cuando hace el rollover y vuelve a empezar de 0
  if ((unsigned long)(milisAhora - milisPrevio1) >= intervalo) TMOUT1 = true; else TMOUT1 = false;
  if ((unsigned long)(milisAhora - milisPrevio2) >= intervalo) TMOUT2 = true; else TMOUT2 = false;
  
#ifdef DEBUG
  parse_millis (millis());
  sprintf (buffer, "Uptime %s", hora);
  Serial.println (buffer);
  Serial.println ();

  parse_millis (intervalo);
  sprintf (buffer, "Tiempo maximo de encendido = %s", hora);
  Serial.println (buffer);
#endif

  // Leemos los switches
  switch1 = digitalRead (SWITCH1);
  switch2 = digitalRead (SWITCH2);

  if (!switch1) {                           //Aqui, si se puso el switch en off, reseteamos milsPrevio1 así si se enciende vuelve a haber un ciclo de TMOUT1 completo!
    milisPrevio1 = milisAhora;
    digitalWrite(SALIDA1,LOW);
    TMOUT1 = false;
    segs1 = 0;
  }

  if (!switch2){                            //Aqui, si se puso el switch en off, reseteamos milsPrevio2 así si se enciende vuelve a haber un ciclo de TMOUT2 completo!
    milisPrevio2 = milisAhora;
    digitalWrite(SALIDA2,LOW);
    TMOUT2 = false;
    segs2 = 0;
  }
  
  if (switch1||switch2) {                                 //Solo si alguno de los dos switches está encendido, procesamos
    if (switch1) {                                        //Si el switch 1 está encendido
      if (!TMOUT1) {                                      //Si no se ha cumplido el timeout para la plaza 1
        pote1 = map (analogRead(POTE1), 0, 1023, 0, 100); //Calculamos el porcentaje para el pote1
        segon1 = (pote1 * CICLO) / 100;                   //Determinar cuantos segundos tiene que estar encendido dentro del ciclo para pote1
        segoff1 = CICLO - segon1;                         //Determinar cuantos segundos tiene que estar apagado para pote 1  

#ifdef DEBUG
	sprintf (buffer, "Valor pote 1 = %d encender por %d segundos, apagar por %d segundos", pote1, segon1, segoff1);
	Serial.println (buffer);
	sprintf (buffer, "segs1 = %ld", segs1);
	Serial.println (buffer);
#endif
	
        if (segs1 < segon1) {            //Utilizamos la variable segs1 para llevar la cuenta de cuánto tiempo estuvo encendido dentro del ciclo
          digitalWrite(SALIDA1,HIGH);    //Si es menor que el segmento de tiempo de encendido calculado arriba, encender. De lo contrario apagar
        } else {
          digitalWrite(SALIDA1,LOW);
        }
        ++segs1;                        //Incrementamos encender en aproximadamente 1 segundo (un poco más dado a todo lo que hay que procesar)
	
        if (segs1 > CICLO) {            //Si segs1 es mayor al ciclo completo, hacer el rollover a 0
          segs1 = 0;
        }   
      } else {
        digitalWrite(SALIDA1,LOW);     //Si segs1 es mayor que segon1 pero menor al ciclo total, entonces es tiempo del ciclo de apagado!
      }
    }
    
    if (switch2) {
      if (!TMOUT2) {
        pote2 = map (analogRead(POTE2), 0, 1023, 0, 100);
        segon2 = (pote2 * CICLO) / 100;  //Aqui la lógica es idéntica al 1...
        segoff2 = CICLO - segon2;        

#ifdef DEBUG
	sprintf (buffer, "Valor pote 2 = %d encender por %d segundos, apagar por %d segundos", pote2, segon2, segoff2);
	Serial.println (buffer);
	sprintf (buffer, "segs2 = %ld", segs2);
	Serial.println (buffer);
#endif
	
	if (segs2 < segon2) {
          digitalWrite(SALIDA2,HIGH);
        } else {
	  digitalWrite(SALIDA2,LOW);
	}
	++segs2;
	
        if (segs2 > CICLO) {
	  segs2 = 0;
	}
      } else {
        digitalWrite(SALIDA2,LOW); 
      }
    }
    
#ifdef DEBUG
  parse_millis (milisAhora - milisPrevio1);
  sprintf (buffer, "%s tiempo de circuito 1 encendido", hora);
  Serial.println (buffer);

  parse_millis (milisAhora - milisPrevio2);
  sprintf (buffer, "%s tiempo de circuito 2 encendido", hora);
  Serial.println (buffer);

  sprintf (buffer, "Switch1 = %d", switch1);
  Serial.println (buffer);
    
  sprintf (buffer, "Switch2 = %d", switch2);
  Serial.println (buffer);

  sprintf (buffer, "TMOUT1 = %d", TMOUT1);
  Serial.println (buffer);

  sprintf (buffer, "TMOUT2 = %d", TMOUT2);
  Serial.println (buffer);
#endif
  } else {
    segs1 = 0;
    segs2 = 0;
  }
  if (!switch1) digitalWrite(SALIDA1,LOW);
  if (!switch2) digitalWrite(SALIDA2,LOW);
#ifdef DEBUG
  Serial.println("==============================================");
#endif DEBUG
  delay (1000); //Este es el delay del ciclo
}
//Eso es tototoooodo amigos!
