#include <Arduino.h>
#include <HX711.h>//loadcells
#include <Adafruit_NeoPixel.h>//leds
#include <ESP8266WiFi.h>
#include <PubSubClient.h>//mqtt
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
//#include <WifiManager.h>
//#include <FS.h>//spiffs
#include <NTPClient.h>//ntp (time)
#include <WiFiUdp.h>//ntp
#include <ESP8266HTTPClient.h>//discord messaging
#include <ESP8266WiFi.h>
//Asynchroner Webserver
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//DNS
#include <ESP8266mDNS.h>

//SPIFFS
#include <FS.h>

//WiFi-Credentials
#include "WiFiConnection.h"

char* dns_name = "ESP8266";   //Homepage (DNS): "http://esp8266.local/"

AsyncWebServer server(80);    //Standard-Port

//placeholder variable for website request
int tmpValue = 0;


/*// HX711-1 (1 load cell)
#define LOADCELL1_DOUT_PIN D2
#define LOADCELL1_SCK_PIN  D1
int Loadcell1Tare;
HX711 scale1;*/

// HX711-2 2 load cells 590g
#define LOADCELL2_DOUT_PIN D4
#define LOADCELL2_SCK_PIN  D3
int Loadcell2Tare=-282175;
float Loadcell2cal=-22.76949f;
HX711 scale2;
//leds
#define LED_PIN D5
#define LED_COUNT 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//spiffs src\data\saves
#define TESTFILE "/saves.txt"
bool    spiffsActive = false;


//-------loop timing
int timeMillis; //timestamp (in millis since chip was turned on) of the last weight measurement.
#define WEIGHT_TAKING_DELAY 3000//after this time (in milliseconds) the weight is measured again.
#define WEIGHT_TAKING_DELAY_FILLING 500//after this time (in milliseconds) the weight is measured again during filling.
int fillingMode = 0;//0=normal functionality, 1=checking for Time Threshold, 2=waiting for filling to stop
int discordNotifMillis = 0; //timestamp (in millis since chip was turned on) of the last time the discord notification conditions were checked.
#define DISCORD_NOTIF_CHECKING_DELAY 30000//after this time (in milliseconds) the discord notification conditions are checked again.
#define DISCORD_ACTIVE_BEGIN 15//from this hour forward discord notifications are send.
#define DISCORD_ACTIVE_MIDDLE 19//at this hour conditions for sending notifications become less strict.
#define DISCORD_ACTIVE_END 22//at this hour the program stops sending discord notifications.
//-------weight
#define BOTTLE_WEIGHT_EMPTY 450
#define BOTTLE_WEIGHT_MAX 1150
#define FILLING_THRESHOLD_WEIGHT 100//weight that has to be added for the bottle to accept it as being filled.
#define FILLING_THRESHOLD_TIME 100//time that the additional weight has to be measured for to accept it as being filled.

//if the bottle is in filling mode and less than this is added between two weight measurements, the filling process is
//considered done.
#define LOADCELL_ERROR_MARGIN 100

int weight = 0;
int weightPrev = 0;
#define RECOMMENDED_MAX_CONSUMPTION 2000//amount of water that should be drunk every day.
//--------liquid consumption data
int consumptionWeek[7] = {0,0,0,0,0,0,0};//rightmost is the current day, left is 6 days ago
int lastSessionWeight = 0;
int lastSessionTimestamp[3] = {0,0,0}; //hour:minute:seconds
int currentDay;//for newDay() / days from 1.Jan 1970

long firstPixelHue = 0;//for rainbow wave

//mqtt settings
/*
IPAddress server(192, 168, 178, 75);//mqtt broker ip
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt";
const char* mqtt_pw = "test123";*/
//mqtt client
//WiFiClient espClient;
//PubSubClient clientmqtt(espClient);


//ntp timeclient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//discord webhook
const char* discord = "https://discord.com/api/webhooks/999006439462944808/yCw1v2TlRdGQydjUNjipzOPnZZlk75nmlQLiFZQrJftcX5NSVW92TPHvG8hNce_yLr-4";
const boolean discord_tts = false;
WiFiClientSecure client;
HTTPClient https;

//mqtt callback
/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}*/
//mqtt reconnecting
/*
void reconnect() {
  // Loop until we're reconnected
  while (!clientmqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (clientmqtt.connect("arduinoClient",mqtt_user,mqtt_pw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      clientmqtt.publish("outTopic","hello world");
      // ... and resubscribe
      clientmqtt.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(clientmqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}*/


String lastDrankDateString(){
  String s;
  if(lastSessionTimestamp[0]<10){
    s+="0";
    s+=(String)lastSessionTimestamp[0];
  }else{
    s+=(String)lastSessionTimestamp[0];
  }
  s+=" : ";
  if(lastSessionTimestamp[1]<10){
    s+="0";
    s+=(String)lastSessionTimestamp[1];
  }else{
    s+=(String)lastSessionTimestamp[1];
  }
  s+=" : ";
  if(lastSessionTimestamp[2]<10){
    s+="0";
    s+=(String)lastSessionTimestamp[2];
  }else{
    s+=(String)lastSessionTimestamp[2];
  }
}

//returns max weight of water
int maxWeight(){
  return BOTTLE_WEIGHT_MAX;
}
//returns amount of last drink
int lastDrankAmount(){
  return lastSessionWeight;
}
//returns date of last drink (Hour, Minute, Second)
int lastDrankDate(int i){
  return lastSessionTimestamp[i];
}
//returns amount drank for last week (Array for last week)
int drankDay(int day){
  return consumptionWeek[day];
}

//returns Water left in gramms
int getWeight(){
return (scale2.get_units(5)-Loadcell2Tare)/Loadcell2cal-BOTTLE_WEIGHT_EMPTY;
}



//sets Led
void setLed(int mode){
  //r-g-b cycling
  if(mode==0){
    for(int i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
      delay(100);
    }
    for(int i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
      delay(100);
    }
    for(int i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
      delay(100);
  }
}
//rainbow wave
  if(mode==1){
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    firstPixelHue+=256*2;
    if(firstPixelHue>=5*65536){
      firstPixelHue=0;
    }
    strip.show();
}
//fill percentage
  if(mode==2){
  float fillp = (float)getWeight()/(float)BOTTLE_WEIGHT_MAX;
  fillp  = fillp*(float)LED_COUNT;
  //Serial.print("fillp: ");
  //Serial.println(fillp-floorf(fillp));
  for(int i=0;i<strip.numPixels();i++){
    if(i>floorf(fillp)){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    }else{
      strip.setPixelColor(i, strip.Color(0,0,255));
    }
    strip.show();
  }
      //strip.setPixelColor(i+1,strip.Color(0,0,255*(fillp-floorf(fillp))));
  }
}

// Sets up POST request to discord webhook.
void discord_send(String content) {
  https.begin(client, discord);
      https.addHeader("Content-Type", "application/json");

      int httpsCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + discord_tts + "}");

      // if the code returned is -1 there has been an error, that's why it checks on -1.
      if(httpsCode > -1){
        Serial.println("Message send succesfull");
      }else{
        Serial.println("Error sending message");
      }
      https.end();
}

//updates variables and saved data for new day
void newDay(){
if (SPIFFS.exists(TESTFILE)) {
 File f = SPIFFS.open(TESTFILE, "w+");
      if (!f) {
        Serial.print("Unable To Open '");
        Serial.print(TESTFILE);
        Serial.println("' for Reading");
        Serial.println();
      } else {
  String h = (String)lastSessionTimestamp[0]+(String)lastSessionTimestamp[1]+(String)lastSessionTimestamp[2];
  f.println(h);
  f.println(lastSessionWeight);
  f.println(0);
  int tempArr[7] = {0,0,0,0,0,0,0};
  for(int i = 1;i<7;i++){
    f.println(drankDay(i-1));
    tempArr[i] = drankDay(i-1);
  }
  for(int i = 0;i<7;i++){
    consumptionWeek[i]=tempArr[i];
  }
        f.close();
    }
  } else {
    Serial.print("Unable To Find ");
    Serial.println(TESTFILE);
    Serial.println();
  }
  
}
//saves amount(g) to saved.txt with timestamp and updates variables
void drank (int amount){
    timeClient.update();
  if(timeClient.getEpochTime()/86400 != currentDay){
    newDay();
  }
if (SPIFFS.exists(TESTFILE)) {
 File f = SPIFFS.open(TESTFILE, "w+");
      if (!f) {
        Serial.print("Unable To Open '");
        Serial.print(TESTFILE);
        Serial.println("' for Reading");
        Serial.println();
      } else {
  String i;
  timeClient.update();
  if(timeClient.getHours()<10){
  i +=  "0" + (String)timeClient.getHours();
  }else{
    i += (String)timeClient.getHours();
  }
  lastSessionTimestamp[0]=timeClient.getHours();
  if(timeClient.getMinutes()<10){
  i +=  "0" + (String)timeClient.getMinutes();
  }else{
    i += (String)timeClient.getMinutes();
  }
  lastSessionTimestamp[1]=timeClient.getMinutes();
  if(timeClient.getSeconds()<10){
  i += "0" + (String)timeClient.getSeconds();
  }else{
    i += (String)timeClient.getSeconds();
  }
  lastSessionTimestamp[2]=timeClient.getSeconds();
  f.println(i);
  f.println(amount);
  f.println(drankDay(0)+amount);
  lastSessionWeight = amount;
  consumptionWeek[0]+=amount;
  for(int i = 1;i<7;i++){
    f.println(drankDay(i));
  }
        f.close();
    }
  } else {
    Serial.print("Unable To Find ");
    Serial.println(TESTFILE);
    Serial.println();
  }
}


//prints data from saved.txt
void printData(){
   if (spiffsActive) {
    if (SPIFFS.exists(TESTFILE)) {
      File f = SPIFFS.open(TESTFILE, "r");
      if (!f) {
        Serial.print("Unable To Open '");
        Serial.print(TESTFILE);
        Serial.println("' for Reading");
        Serial.println();
      } else {
        String s;
        Serial.print("Contents of file '");
        Serial.print(TESTFILE);
        Serial.println("'");
        Serial.println();
        while (f.position()<f.size())
        {
          s=f.readStringUntil('\n');
          s.trim();
          Serial.println(s);
        }
         
        f.close();
      }
      Serial.println();
     } else {
      Serial.print("Unable To Find ");
      Serial.println(TESTFILE);
      Serial.println();
    }
  }
  Serial.print("Timestamp: ");
  Serial.print(lastSessionTimestamp[0]);
  Serial.print(lastSessionTimestamp[1]);
  Serial.println(lastSessionTimestamp[2]);
  Serial.print("Last: ");
  Serial.println(lastSessionWeight);
  Serial.print("Week: ");
  for(int i = 0;i<7;i++){
    Serial.print(consumptionWeek[i]);
    Serial.print(" : ");
  }
  Serial.println();
}

//loads data from saved.txt to variables
void loadData(){
    if (spiffsActive) {
    if (SPIFFS.exists(TESTFILE)) {
      File f = SPIFFS.open(TESTFILE, "r");
      if (!f) {
        Serial.print("Unable To Open '");
        Serial.print(TESTFILE);
        Serial.println("' for Reading");
        Serial.println();
      } else {
        String s;
        Serial.print("Contents of file '");
        Serial.print(TESTFILE);
        Serial.println("'");
        Serial.println();
        while (f.position()<f.size())
        {
          s=f.readStringUntil('\n');
          s.trim();
          Serial.println(s);
            lastSessionTimestamp[0] = s.substring(0,2).toInt();
            Serial.print("Hours :");
            Serial.println(lastSessionTimestamp[0]);
            lastSessionTimestamp[1]=s.substring(2,4).toInt();
            Serial.print("Minutes :");
            Serial.println(lastSessionTimestamp[1]);
            lastSessionTimestamp[2]=s.substring(4,6).toInt();
            Serial.print("Seconds :");
            Serial.println(lastSessionTimestamp[2]); 

          s=f.readStringUntil('\n');
          s.trim();
          Serial.println(s);

            Serial.print("Last drank :");
            lastSessionWeight=s.toInt();
            Serial.println(s.toInt());

          for(int i = 0;i<7;i++){
            s=f.readStringUntil('\n');
            s.trim();
            

            Serial.print("Drank day ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(s);
            consumptionWeek[i]=s.toInt();
          }
        }
         
        f.close();
      }
      Serial.println();
     } else {
      Serial.print("Unable To Find ");
      Serial.println(TESTFILE);
      Serial.println();
    }
  }
  
}

void checkWeight(){

 //weight checking
  if (fillingMode == 1) {
    if (millis() > timeMillis + FILLING_THRESHOLD_TIME) {
      timeMillis = millis();
      weight = getWeight();
      if (weight - weightPrev > FILLING_THRESHOLD_WEIGHT) {
        fillingMode = 2;
      } else {
        fillingMode = 0;
      }
    }
  } else {
    if (fillingMode == 2) {
      if (millis() > timeMillis + WEIGHT_TAKING_DELAY_FILLING) {
        timeMillis = millis();
        weight = getWeight();
        if (weight - weightPrev < LOADCELL_ERROR_MARGIN) {
          fillingMode = 0;
        }
      }
    } else {
      timeMillis = millis();
      if (millis() > timeMillis + WEIGHT_TAKING_DELAY){ //checks whether the weight has to be measured again.
        weight = getWeight();
        if (weight > LOADCELL_ERROR_MARGIN) {
          weightPrev = weight;
          int deltaWeight = weight - weightPrev;
          if (abs(deltaWeight) > LOADCELL_ERROR_MARGIN) {
            if (deltaWeight > FILLING_THRESHOLD_WEIGHT) {
              fillingMode = 1;
            } else {
              drank(0 - deltaWeight);
            }
          }
        }
      }
    }
  }
}

void sendRequests() {
  //Startseite
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.php", String(tmpValue));
  });
  
  //Info-Seite
  server.on("/infopage.php", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/infopage.php", String(tmpValue));
  });

  //Impressum
  server.on("/imprint.php", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/imprint.php", String(tmpValue));
  });

  //Nutzungsbedingungen
  server.on("/policy.php", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/policy.php", String(tmpValue));
  });

  //Datenschutz
  server.on("/privacy.php", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/privacy.php", String(tmpValue));
  });

  //CSS-Dateien
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/button.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/button.css", "text/css");
  });

  //Daten-Anzeige auf der Startseite
  server.on("/currentWeight", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(getWeight()));
  });
  
  server.on("/maxWeight", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(maxWeight()));
  });

  server.on("/lastDrankAmount", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(lastDrankAmount()));
  });

  server.on("/lastDrankDateHour", HTTP_GET, [](AsyncWebServerRequest * request) {
   request->send(200, "text/plain", String(lastSessionTimestamp[0]));
  });

  server.on("/lastDrankDateMinute", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(lastSessionTimestamp[1]));
  });

  server.on("/lastDrankDateSecond", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(lastSessionTimestamp[2]));
  });

  server.on("/DrankDay1", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[0]));
  });

  server.on("/DrankDay2", HTTP_GET, [](AsyncWebServerRequest * request) {
   request->send(200, "text/plain", String(consumptionWeek[1]));
  });

  server.on("/DrankDay3", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[2]));
  });

  server.on("/DrankDay4", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[3]));
  });

  server.on("/DrankDay5", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[4]));
  });

  server.on("/DrankDay6", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[5]));
  });

  server.on("/DrankDay7", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(consumptionWeek[6]));
  });
}


void setup() {
  Serial.begin(115200);
  delay(500);
  /*Serial.println("Initializing the scale1");
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  Serial.println("Scale1 Initialized");*/
  	
  Serial.println("Initializing the scale2");
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  Serial.println("Scale2 Initialized");

  SPIFFS.begin();
  
  WiFi.begin(ssid, password);

  if(WiFi.isConnected()){
    Serial.println("Wifi Conneced");
  }
  MDNS.begin("ESP8266");
  
  //contains methods for website requests
  sendRequests();

 
  //Wifimanager Initialization
  //WiFiManager wifiManager;
  //wifiManager.autoConnect("AutoConnectAP");
  /*//mqtt Initialization
  clientmqtt.setServer(server, mqtt_port);
  clientmqtt.setCallback(callback);*/

  Serial.println("Initializing LED Strip");
  strip.begin(); 
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
   Serial.println("LED strip Initialized");

  timeClient.begin();
  Serial.println("NTP client Initialized");
  timeClient.setTimeOffset(7200);

 // Start filing subsystem
  if (SPIFFS.begin()) {
      Serial.println("SPIFFS Active");
      Serial.println();
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }
  //current day for newDay
  timeClient.update();
  currentDay = timeClient.getEpochTime()/86400;

  //discord message
  client.setInsecure();
  //Load cell calibration
  Serial.print("Raw Result: ");
  Serial.println(scale2.read_average(5));
  /*
  if (scale2.is_ready()) {
    scale2.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    Serial.println(scale2.read_average(5));
    delay(5000);
    //scale2.tare();
    //scale2.set_offset(scale2.read_average(5));
    Loadcell2Tare = scale2.read_average(10);
    Serial.println("Tare done...");
    Serial.println(scale2.read_average(10)-Loadcell2Tare);
    Serial.print("Place a known weight on the scale...");
    delay(4000);
    //scale2.set_scale(590);
    //Loadcell2cal = scale2.get_units(10)/590;
    Serial.print("Resultcal: ");
    Serial.println(scale2.get_units(10)/590);
    long reading = (scale2.get_units(10)-Loadcell2Tare)/Loadcell2cal;
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  */
  loadData();
  printData();


  timeMillis = millis();

  server.begin();

}

void loop() {
  //mqtt loop
  /*//mqtt reconnecting 
   if (!client.connected()) {
    reconnect();
  }
  //mqtt loop
  client.loop();*/

 // Serial.print("Result: ");
 //Serial.println(getWeight());


  if ((discordNotifMillis + DISCORD_NOTIF_CHECKING_DELAY) < (int)millis()) {
    discordNotifMillis = millis();
    timeClient.update();
    int h = timeClient.getHours();
    if (h > DISCORD_ACTIVE_BEGIN && h < DISCORD_ACTIVE_END) {
      if ((h < DISCORD_ACTIVE_MIDDLE && 2000/(double)consumptionWeek[0] < (24/((double) h))) || (h > DISCORD_ACTIVE_MIDDLE && consumptionWeek[0] < 2000)) {//2000 means 2 liters every day.
        String s  = "Hey! Du hast deinen täglichen Wasserbedarf noch nicht erfüllt. Es fehlen heute noch ";
        s += (String)(2000 - consumptionWeek[0]);
        s += " Milliliter!";
        discord_send(s);
      }
    }
  }
  
  checkWeight();

  setLed(2);

  MDNS.update();
}



