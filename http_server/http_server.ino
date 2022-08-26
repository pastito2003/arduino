/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */
#include <string.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

//OneWire librerias para leer el termometro
#include <OneWire.h> 
#include <DallasTemperature.h>

#define WDT_TIMEOUT 4   //Definicion del WDT

#define ONE_WIRE_BUS 22 //Definicion del puerto del termometro
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

const char* ssid     = "Tele-7614";
const char* password = "";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);      // set the LED pin mode

    delay(10);
     // Start up the library for the thermometer
     sensors.begin();

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
    esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
    esp_task_wdt_add(NULL);               //add current thread to WDT watch
}

int value = 0;

void loop(){
 esp_task_wdt_reset(); //Reset WDT
 WiFiClient client = server.available();    // listen for incoming clients
 sensors.requestTemperatures(); // Send the command to get temperature readings
 
  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
     
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("La temperatura es: ");
            client.print(sensors.getTempCByIndex(0));
            client.print("<br>\n");
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 2 on.<br>\n");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 2 off.<br>\n");

            client.print("<form action=\"/get\">\n");
            client.print("Temperatura a la que encender la calefaccion: <input type=\"text\" name=\"temp\">");
            client.print("<input type=\"submit\" value=\"Enviar\"><br>\n");
            client.print("</form><br>\n");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(2, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(2, LOW);                // GET /L turns the LED off
        }

  
        if (currentLine.endsWith("temp=")) {
          Serial.print("\n>>");
          //Serial.print (resultado); 
          //Serial.print (currentLine.substring(14,16));
          //Serial.println("<<");
          //Serial.println();
          //Serial.print("Temp = >>>");
          Serial.print (currentLine);
          Serial.println("<<<");
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
