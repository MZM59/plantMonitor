
#define SOIL_SENSOR_DATA A0
#define SOIL_SENSOR_PWR D3 

#define LIGHT_SENSOR_PWR D4
#define LIGHT_SENSOR_DATA A0

#define DHT_SENSOR_DATA D8 
#define AIR_FAN_PWR D10 

#include "DHT.h"
#include <ESP8266WiFi.h>
#include "./DNSServer.h"                  // Patched lib
#include <ESP8266WebServer.h>
#include "DHT.h"


float Temperature;
float Humidity;
DHT dht(DHT_SENSOR_DATA, DHT11);

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

float soilHumiditySensorValue = 0; 
float airHumiditySensorValue = 0;
float lightIntensitySensorValue = 0;
float temperatureSensorValue = 0;
float lightSpectrumSensorValue = 0;

String getSensorsState(){
  String res =  "{soilHumidity:" + String(soilHumiditySensorValue) + "," +"airHumidity:" + String(airHumiditySensorValue) + "," +"temperature:" + String(temperatureSensorValue) + "," + "lightIntensity:" + String(lightIntensitySensorValue) + "," +"lightSpectrum:" + String(lightSpectrumSensorValue) + "}";
  return res;
}

void soilHumidity(){
    Serial.println("getSensorsState triggered, state is: " + getSensorsState());
    webServer.send(200, "text/html", "SensorsState: " + getSensorsState());
}

void setup() { 
 Serial.begin(115200);
 pinMode(SOIL_SENSOR_PWR, OUTPUT);
 pinMode(LIGHT_SENSOR_PWR, OUTPUT);
 pinMode(AIR_FAN_PWR, OUTPUT);

 pinMode(DHT_SENSOR_DATA, INPUT);
 pinMode(SOIL_SENSOR_DATA, INPUT);
 pinMode(LIGHT_SENSOR_DATA, INPUT);
 dht.begin();
 
 WiFi.mode(WIFI_AP);
 WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
 WiFi.softAP("Plant Monitor");


 Serial.begin(115200);
 Serial.setDebugOutput(true);
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);


  webServer.onNotFound([]() {
    webServer.send(200, "text/html", "HomeBox Connected!");
  });
  webServer.on("/soil", soilHumidity);

  webServer.begin();
} 
void loop() { 
  dnsServer.processNextRequest();
  webServer.handleClient();

  digitalWrite(SOIL_SENSOR_PWR, HIGH);  
  digitalWrite(LIGHT_SENSOR_PWR, LOW);  
  delay(100);
  
   for (int i = 0; i <= 100; i++) 
   { 
     soilHumiditySensorValue = soilHumiditySensorValue + analogRead(SOIL_SENSOR_DATA); 
     delay(1); 
   } 
   soilHumiditySensorValue = soilHumiditySensorValue/100.0; 
   Serial.println("Soil Humidity: " + String(soilHumiditySensorValue));
   delay(30); 

  digitalWrite(SOIL_SENSOR_PWR, LOW);  
  digitalWrite(LIGHT_SENSOR_PWR, HIGH);  
  delay(100);
  
  lightIntensitySensorValue = analogRead(LIGHT_SENSOR_DATA);
  Serial.println("Light Intensity: " + String(lightIntensitySensorValue));

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  temperatureSensorValue = Temperature;
  Humidity = dht.readHumidity(); 
  airHumiditySensorValue = Humidity;
  
  // Get temperature event and print its value.
  Serial.print(F("Temperature: "));
  Serial.print(Temperature);
  Serial.println(F("°C"));

  Serial.print(F("Humidity: "));
  Serial.print(Humidity);
  Serial.println(F("%"));

  if (Humidity > 50) {
      digitalWrite(AIR_FAN_PWR, HIGH);  
  } else {
      digitalWrite(AIR_FAN_PWR, LOW);  
  }
} 
