#include <OneWire.h>
#include <DallasTemperature.h>


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// variable to hold device addresses
DeviceAddress Thermometer;

int deviceCount = 0;



uint getDeviceCount()
{
    Serial.println("Get Dallas addresses by index..."); // Use getAddress method to ID sensors
    uint NumDallasActive = 0;
    Serial.println(" Connected temp sensor addresses:");
    // loop to count found sensors
    DeviceAddress tempaddr;
    while (sensors.getAddress(tempaddr, NumDallasActive))
    { // true if sensor found for index
        Serial.print(" IsValidAddress:");
        Serial.println(sensors.validAddress(tempaddr));
        if (!sensors.validAddress(tempaddr))
        {
            continue;
        }
        Serial.print(" IsDallasSensor:");
        Serial.println(sensors.validFamily(tempaddr));
        if (sensors.validFamily(tempaddr))
        {
            //thermos.push_back(NumDallasActive);
            NumDallasActive++;
        }
        Serial.print(" Sensor ");
        Serial.print(NumDallasActive);
        Serial.print(" address : ");
        printAddress(tempaddr);
        Serial.println();
    }
    Serial.print(" TTL Dallas Device Count:");
    Serial.println(NumDallasActive, DEC);
    // report parasite power requirements
    Serial.print(" Parasite power is: ");
    if (sensors.isParasitePowerMode())
        Serial.println("ON");
    else
        Serial.println("OFF");

    return NumDallasActive;
}


void setup(void)
{
  // start serial port
  Serial.begin(115200);
  
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.println("Locating devices...");
  Serial.print("Found ");
  //deviceCount = sensors.getDeviceCount();
  deviceCount = getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
  
  Serial.println("Printing addresses...");
  for (int i = 0;  i < deviceCount;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    sensors.getAddress(Thermometer, i);
    printAddress(Thermometer);
  }
}

void loop(void)
{}

void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}
