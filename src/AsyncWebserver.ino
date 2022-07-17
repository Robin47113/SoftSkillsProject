#include <ESP8266WiFi.h>
//Asynchroner Webserver
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//DNS
#include <ESP8266mDNS.h>

//SPIFFS
#include <FS.h>

//WiFi-Credentials
#include "WiFiConnection.h";

char* dns_name = "ESP8266";   //Startseite (DNS): "http://esp8266.local/"

AsyncWebServer server(80);    //Standard-Port

//Test-Variable
int tmpValue = 0;

//Variablen für die Anzeige der Messdaten (später aus Methoden beziehen)
int maxWeight = 50;
int lastDrankAmount = 10;
int lastDrankDate = 13;
int drankDay = 10;
int currentWeight = 0;

void calculateVariables() {
  currentWeight = random(1, 45);
}

#include "async_request_methods.h";

void setup() {
  SPIFFS.begin();
  
  WiFi.begin(ssid, password);

  MDNS.begin("ESP8266");
  
  //Methoden für Webseitenaufruf und Parameterübergabe
  sendRequests();
  
  server.begin();
}

void loop() {
  calculateVariables();
  MDNS.update();
}
