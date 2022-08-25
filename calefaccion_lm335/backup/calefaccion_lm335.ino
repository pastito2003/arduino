//Pastito 30 de mayo 2022
#include <esp_task_wdt.h>           //Include para el watchdog timer
#include <EEPROM.h>                 //https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>               //Libreria para TCP asincrónico
#include <ESPAsyncWebServer.h>      //Libreria para el web server
#include <Adafruit_ADS1X15.h>       //Librería para el ADS1115


#define EEPROM_SIZE 12              //Definiciones de la EEPROM en este caso del ESP32
#define RELE 23
#define WDT_TIMEOUT 8               //Definicion de timeout del watchdog timer
#define DEBUG
#define ADENTRO 0
#define AFUERA  1

// Crear objeto de la clase
Adafruit_ADS1115 ads;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws

const char* ssid = "COVACHA";
const char* password = "";
//const char* ssid     = "Tele-7614";
//const char* password = "";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

unsigned long previousMillis = 0;
unsigned long interval = 60000;

int  encendido;
int  temperatura;
char hora[20];

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

float get_temp_ads1115_lm335 (int sensor){
  float sensorValue, voltageOut, temperatureK, temperatureC;
  
  sensorValue = ads.readADC_SingleEnded(sensor);

  voltageOut = (sensorValue * 4096) / 32768;
  // Obtener datos del A0 del ADS1115

  //Calculate temperature for LM335
  temperatureK = voltageOut / 10;
  temperatureC = temperatureK - 273.15;
  //temperatureF = (temperatureC * 1.8) + 32;
  return (temperatureC);
}

void setup() {
  Serial.begin(115200);
    
  pinMode(RELE, OUTPUT);         //Aca va el relé de la bomba
  digitalWrite(RELE, LOW);
  
  /* Cambiar factor de escala
  GAIN_TWOTHIRDS =  6.144V  0.0001875 (default)
  GAIN_ONE = 4.096V  0.000125
  GAIN_TWO = 2.048V
  GAIN_FOUR = 1.024V
  GAIN_EIGHT = 0.512V
  GAIN_SIXTEEN = 0.256V
  */
  ads.setGain(GAIN_ONE);
  // Iniciar el ADS1115
  ads.begin();
  
  //analogReadResolution (12);
    
  eeprom_read();                 //Leemos la EEPROM
  initWiFi();                    // Inicializamos Wi-Fi
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print(index_html);
      response->print(get_temp_ads1115_lm335(AFUERA));
      response->print(" y la interior es de: ");
      response->print(get_temp_ads1115_lm335(ADENTRO));
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

void loop() {
  unsigned long currentMillis = millis();
  char mybuffer[80];
  
  ws.cleanupClients();                // Kill old connections
  esp_task_wdt_reset();               //Reset WDT

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
    Serial.println(get_temp_ads1115_lm335(ADENTRO));
    Serial.print(F("Temperatura exterior: "));
    Serial.println(get_temp_ads1115_lm335(AFUERA));
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
    if (get_temp_ads1115_lm335(AFUERA) < temperatura) {
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
    Serial.println(F("Reconectando WiFi..."));
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}
