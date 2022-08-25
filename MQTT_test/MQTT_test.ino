#include <WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 17
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid = "COVACHA";
const char* password = "theraininspa1n";

const char* mqttServer = "192.168.0.159";
const int mqttPort = 1883;
const char* mqttUser = "charly";
const char* mqttPassword = "contrase";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{ 
  Serial.begin(115200);

  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi.");

  while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".") ;
    }

  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    
    if (client.connect("ESP32Client", mqttUser, mqttPassword ))
      Serial.println("connected");
    else
      {
	Serial.print("failed with state ");
	Serial.print(client.state());
	delay(2000);
      }
  }
}

void loop()
{
  char str[16];
  
  sensors_event_t event;
  
  client.loop();

  dht.temperature().getEvent(&event);
  sprintf(str, "%.1f", event.temperature);
  client.publish("cocina/temp", str);
  Serial.println(str);

  dht.humidity().getEvent(&event);
  sprintf(str, "%.1f", event.relative_humidity);
  client.publish("cocina/humedad", str);
  Serial.println(str);
  
  delay(1000);
}
 
