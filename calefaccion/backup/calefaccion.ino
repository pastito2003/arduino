
//#include "BluetoothSerial.h"
//BluetoothSerial SerialBT;

#include <esp_task_wdt.h> //Include para el watchdog timer

//Libreria de EEPROM
#include <EEPROM.h>//https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM
#define EEPROM_SIZE 12
#define RELE 2

//OneWire librerias para leer el termometro
#include <OneWire.h> 
#include <DallasTemperature.h>

#define WDT_TIMEOUT 4   //Definicion del WDT
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define ONE_WIRE_BUS 4 //Definicion del puerto del termometro
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
int  address = 0;
int  temperatura;
char mybuffer[80];
char hora[20];


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Sistema de calefacci&oacute;n de pastito</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <H2>Sistema de calefaccion de pastito</h2><hr><br>
  )rawliteral";

const char index_html1a[] PROGMEM = R"rawliteral(
  <form action="/get">
    Temperatura a la que encender la calefacci&oacute;n: <input type="text" name="input1"
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
  //Leemos de la EEPROM si estamos encendidos o apagados y la temperatura a la que encender la calefaccion
  EEPROM.begin(EEPROM_SIZE);
  encendido = EEPROM.read(address); //EEPROM.get(address,readId)
  address += sizeof(encendido); //update address value
  temperatura = EEPROM.read(address);
  //EEPROM.get(address, temperatura); //readParam=EEPROM.readFloat(address);
  EEPROM.end();
}

void eeprom_write() {
  address = 0;
  
  Serial.println("Guardando valores en EEPROM");
  Serial.print("Temperatura = ");
  Serial.println(temperatura);
  Serial.print("Encendido = ");
  Serial.println(encendido);
  
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(address, encendido);//EEPROM.put(address, boardId);
  address += sizeof(encendido); //update address value
  
  //EEPROM.writeFloat(address, param);//EEPROM.put(address, param);
  EEPROM.write(address, temperatura);//EEPROM.put(address, param);
  EEPROM.commit();
  EEPROM.end();
}

//Funcion para parsear los milisegundos e imprimirlos legibles en dias:horas:minutos:segundos
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


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void initWiFi() {
  int retries = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  
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
    Serial.println("WiFi Failed!");
   }
}

void setup() {
  Serial.begin(115200);
  //SerialBT.begin("CalefaccionPastito"); //Bluetooth device name
  //Serial.println("The device started, now you can pair it with bluetooth!");
  
  pinMode(RELE, OUTPUT);      //Aca va el relé de la bomba
  delay(500);

   // Start up the library for the thermometer
  sensors.begin();
  delay (20);
  
  //Leemos la EEPROM
  eeprom_read();
  sprintf (mybuffer, "Encendido = %d", encendido);
  Serial.println(mybuffer);
  sprintf(mybuffer, "Temperatura = %f", temperatura);
  Serial.println(mybuffer);
  
  initWiFi();
 
  sensors.requestTemperatures(); // Send the command to get temperature readings
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print(index_html);
      response->print("<h3>La temperatura exterior es de: ");
      response->print(sensors.getTempCByIndex(0));
      response->print(" y la interior es de: ");
      response->print(sensors.getTempCByIndex(1));
      response->println(" grados</h3>");
      
      parse_millis (millis());
      
      response->print ("<h4>Tiempo de encendido: ");
      response->println(hora);
      response->println("<br>");

      if (digitalRead(RELE)){
        response->println("El estado del rel&eacute; es encendido<br>");
      } else {
        response->println("El estado del rel&eacute; es apagado<br>");
      }
      
      response->println("<br>");
      response->print(index_html1a);
      response->print("value=\"");
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
      
      response->println("</body></html>");
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
      } else {
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
  
  ws.cleanupClients();                // Kill old connections
  esp_task_wdt_reset();               //Reset WDT
  sensors.requestTemperatures();
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  parse_millis(millis());
  sprintf (mybuffer, "Uptime %s", hora);
  Serial.println (mybuffer);

  Serial.print("El estado de la calefaccion es = ");
  if (encendido) {
    Serial.println("ENCENDIDA");
  } else {
    Serial.println("APAGADA");
  }
  
  Serial.print("La temperatura del sensor interior es de ");
  Serial.println(sensors.getTempCByIndex(1));
  Serial.print("La temperatura del sensor exterior es de ");
  Serial.println(sensors.getTempCByIndex(0));
  Serial.print("La temperatura minima de disparo es de ");
  Serial.println(temperatura);

  if (digitalRead(RELE)){
        Serial.println("El estado del relé es encendido");
      } else {
        Serial.println("El estado del relé es apagado");
  }

  Serial.println("==========================================");
  
  if (encendido) {
    //if (sensors.getTempCByIndex(0) < temperatura) {
    if (sensors.getTempCByIndex(0) < temperatura) {
      digitalWrite(RELE, HIGH);
    } else {
      digitalWrite(RELE, LOW);
    }
  }
  if (!encendido) {
    digitalWrite(RELE, LOW);
  }

  
  delay(1000);

  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}
