//Pastito 30 de mayo 2022
#include <esp_task_wdt.h>           //Include para el watchdog timer
#include <EEPROM.h>                 //https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>               //Libreria para TCP asincrónico
#include <ESPAsyncWebServer.h>      //Libreria para el web server
#include <OneWire.h>                //OneWire librerias para leer el termometro
#include <DallasTemperature.h>

#define EEPROM_SIZE 12              //Definiciones de la EEPROM en este caso del ESP32
#define RELE 2
#define WDT_TIMEOUT 8               //Definicion de timeout del watchdog timer
#define DEBUG
#define ONE_WIRE_BUS 4               //Definicion del puerto del termometro
#define arr_prom 10
#define AFUERA 1
#define ADENTRO 2

// Direcciones de los DS18B20
uint8_t sensor_afuera[8]   = { 0x28, 0xED, 0xB9, 0x9F, 0x0D, 0x00, 0x00, 0xAA };   //legit
uint8_t sensor_adentro[8] =  { 0x28, 0x98, 0x61, 0x9F, 0x0D, 0x00, 0x00, 0xB9 }; //legit

//uint8_t sensor_afuera[8]  =  { 0x28, 0xFF, 0x4F, 0x47, 0xB3, 0x16, 0x05, 0x89 }; //trucho impermeable
//uint8_t sensor_adentro[8] =  { 0x28, 0xFF, 0xBD, 0xB4, 0x00, 0x16, 0x02, 0x8E }; //trucho to92



// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws

const char* ssid = "COVACHA";
const char* password = "theraininspa1n";
//const char* ssid     = "Tele-7614";
//const char* password = "tele-8443";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

unsigned long previousMillis = 0;
unsigned long interval = 60000;

int  encendido;
int  temperatura;
char hora[20];

//Variable para temperatura, las inicializamos en 25 grados para que no dispare antes de tiempo
float prom_af[arr_prom] = {25,25,25,25,25,25,25,25,25,25};
float prom_ad[arr_prom] = {25};
float tempAd = 25;
float tempAf = 25;



const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
<title>Sistema de calefacci&oacute;n de pastito</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
</head><body>
<H2>Sistema de calefaccion de pastito</h2><hr><br>
<h3>Temperatura exterior: 
)rawliteral";

const char index_html1a[] PROGMEM = R"rawliteral(
<br><br>
<form action="/get">
Temperatura de encendido: <input type="text" name="input1" 
)rawliteral";

const char index_html1b[] PROGMEM = R"rawliteral(
<input type="submit" value="Aceptar">
</form><br><br>
<form action=/get>
<input type="radio" id="Encender" name="input2" value="1" 
)rawliteral";

const char index_html1c[] PROGMEM = R"rawliteral(        
<label for="html">Encendido</label><br>
<input type="radio" id="Apagar" name="input2" value="0" 
)rawliteral";

const char index_html1d[] PROGMEM = R"rawliteral(              
<label for="html">Apagado</label><br><br>
<input type="submit" value="Aceptar">
</form>
)rawliteral";



void eeprom_read() {
  int address = 0;
  
  EEPROM.begin(EEPROM_SIZE);             //Leemos de la EEPROM si estamos encendidos o apagados y la temperatura a la que encender la calefaccion
  encendido = EEPROM.read(address);      //EEPROM.get(address,readId)
  address += sizeof(encendido);          //update address value
  temperatura = EEPROM.read(address);
  EEPROM.end();
}

void eeprom_write() {
  int address = 0;

  #ifdef DEBUG
    Serial.println(F("Guardando valores en EEPROM"));
    Serial.print(F("Temperatura = "));
    Serial.println(temperatura);
    Serial.print(F("Encendido = "));
    Serial.println(encendido);
  #endif
  
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(address, encendido);        //EEPROM.put(address, boardId);
  address += sizeof(encendido);            //update address value
  EEPROM.write(address, temperatura);      //EEPROM.put(address, param);
  EEPROM.commit();
  EEPROM.end();
}

void parse_millis (unsigned long milisegundos) {   //Funcion para parsear los milisegundos e imprimirlos legibles en dias:horas:minutos:segundos
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

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initWiFi() {
  int retries = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print(F("Connecting to WiFi .."));
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
      
    esp_task_wdt_reset();
    delay(1000);
    retries++;
    if (retries > 4) {
      break; 
    }
  }

  Serial.println(WiFi.localIP());
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println(F("WiFi Failed!"));
   }
}


void setup() {
  Serial.begin(115200);
    
  pinMode(RELE, OUTPUT);         //Aca va el relé de la bomba
  delay(500);

  digitalWrite (ONE_WIRE_BUS, LOW);
  pinMode (ONE_WIRE_BUS, INPUT);
  sensors.begin();
  delay (20);
    
  eeprom_read();                 //Leemos la EEPROM
  initWiFi();                    // Inicializamos Wi-Fi
 
  sensors.requestTemperatures(); //Comando para leer temperaturas
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print(index_html);
      response->print(tempAf);
      response->print(" y la interior es de: ");
      response->print(tempAd);
      response->println(" grados</h3>");
      
      parse_millis (millis());
      
      response->print ("<h4>Tiempo de encendido: ");
      response->println(hora);
      response->println("<br>");
      response->println("El estado del rel&eacute; es ");

      if (digitalRead(RELE)){
        response->println("encendido");
      } else {
        response->println("apagado");
      }
      
      response->print(index_html1a);
      response->print(" value=\"");
      response->print(temperatura);
      response->println("\">");
      response->print(index_html1b);
      
      if (encendido) {
	      response->println("checked>");
      }
      response->print(index_html1c);
      
      if (!encendido) {
	      response->println("checked>");
      }
      response->print(index_html1d);
      response->println("</h4></body></html>");
      
      request->send(response);
    });
  
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String inputMessage;
      String inputParam;
      // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
      if (request->hasParam(PARAM_INPUT_1)) {
	       inputMessage = request->getParam(PARAM_INPUT_1)->value();
	       inputParam = PARAM_INPUT_1;
	       temperatura = inputMessage.toInt();
	       eeprom_write();
      }
      // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
      else if (request->hasParam(PARAM_INPUT_2)) {
	       inputMessage = request->getParam(PARAM_INPUT_2)->value();
	       inputParam = PARAM_INPUT_2;
	       if (inputMessage == "1") {
	         encendido = true;
	       }
	       if (inputMessage == "0") {
	         encendido = false;
 	       }
         eeprom_write();
      }else {
	      inputMessage = "No message sent";
	      inputParam = "none";
      }
      //Serial.println(inputMessage);
      
      request->send(200, "text/html", "HTTP GET request envio a tu ESP el campo (" 
		    + inputParam + ") con el valor: " + inputMessage +
		    "<br><a href=\"/\">Volver a la pagina principal</a>");
    });
  server.onNotFound(notFound);
  server.begin();
  
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               //add current thread to WDT watch
}

float get_temp_DS18B20(int sensor) {
  float resultado;

  if (sensor == AFUERA) {
    resultado = sensors.getTempC(sensor_afuera);
  }
  if (sensor == ADENTRO) {
    resultado = sensors.getTempC(sensor_adentro);
  }
  return resultado;
}

void mover_arreglo (int sensor, float lectura) {
  int i;

  if (sensor == AFUERA){
    for (i=1;i<arr_prom;i++){
      prom_af[i-1]=prom_af[i];
    }
    prom_af[arr_prom-1] = lectura;
  }
  if (sensor == ADENTRO){
    for (i=1;i<arr_prom;i++){
      prom_ad[i-1]=prom_ad[i];
    }
    prom_ad[arr_prom-1] = lectura;
  }
}

void leer_temp_prom (int sensor) {
  float suma = 0;
  float lectura;
  int i;

  lectura = get_temp_DS18B20(sensor);
  delay (20);
  mover_arreglo (sensor, lectura);

  if (sensor == AFUERA){
     for (i=0;i<arr_prom;i++){
        suma += prom_af[i];  
     }
     tempAf = suma / arr_prom;
  }
  if (sensor == ADENTRO){
     for (i=0;i<arr_prom;i++){
        suma += prom_ad[i];  
     }
     tempAd = suma / arr_prom;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  char mybuffer[80];
  
  ws.cleanupClients();                // Kill old connections
  esp_task_wdt_reset();               //Reset WDT
  sensors.requestTemperatures();
  
  leer_temp_prom(AFUERA);
  leer_temp_prom(ADENTRO);

  #ifdef DEBUG
    Serial.print(F("IP Address: "));
    Serial.println(WiFi.localIP());
  #endif
  
  parse_millis(millis());
  
  #ifdef DEBUG
    sprintf (mybuffer, "Uptime %s", hora);
    Serial.println (mybuffer);

    Serial.print(F("El estado de la calefaccion es = "));
  #endif
  
  if (encendido) {
    #ifdef DEBUG
      Serial.println(F("ENCENDIDA"));
    #endif
  } else {
    #ifdef DEBUG
      Serial.println(F("APAGADA"));
    #endif
  }

  #ifdef DEBUG
    Serial.print(F("Temperatura interior: "));
    Serial.println(tempAd);
    Serial.print(F("Temperatura exterior: "));
    Serial.println(tempAf);
    Serial.print(F("Temperatura de disparo: "));
    Serial.println(temperatura);
  #endif

  if (digitalRead(RELE)){
    #ifdef DEBUG
      Serial.println(F("Relé encendido"));
    #endif
    } else {
      #ifdef DEBUG
        Serial.println(F("Relé apagado"));
      #endif
  }

  #ifdef DEBUG
    Serial.println(F("=========================================="));
  #endif
  
  if (encendido) {
    if (tempAf < temperatura) {
      digitalWrite(RELE, HIGH);
    } else {
      digitalWrite(RELE, LOW);
    }
  }
  if (!encendido) {
    digitalWrite(RELE, LOW);
  }
  
  delay(1000);

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {    // Si la WiFi esta caida, reconectar cada CHECK_WIFI_TIME segundos
    Serial.print(millis());
    Serial.println(F("\nReconectando WiFi..."));
    WiFi.disconnect();
    esp_task_wdt_reset();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}
