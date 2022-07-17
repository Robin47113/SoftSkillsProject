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
#include "WiFiConnection.h";

char* dns_name = "ESP8266";   //Startseite (DNS): "http://esp8266.local/"

AsyncWebServer server(80);    //Standard-Port

//Platzhalter-Variable für den Webseitenaufruf
int tmpValue = 0;

//Variablen für die Anzeige der Messdaten (später aus Methoden beziehen)
int maxWeight = 0;
int lastDrankAmount = 0;
int lastDrankDateHour = 0;
int lastDrankDateMinute = 0;
int lastDrankDateSecond = 0;

int[] lastDrankDate = { lastDrankDateHour, lastDrankDateMinute, lastDrankDateSecond };

int drankDay1 = 0;
int drankDay2 = 0;
int drankDay3 = 0;
int drankDay4 = 0;
int drankDay5 = 0;
int drankDay6 = 0;
int drankDay7 = 0;

int[] drankDay = { drankDay1, drankDay2, drankDay3, drankDay4, drankDay5, drankDay6, drankDay7 };

int currentWeight = 0;

#include "async_request_methods.h";

/*// HX711-1 (1 load cell)
#define LOADCELL1_DOUT_PIN D2
#define LOADCELL1_SCK_PIN  D1
int Loadcell1Tare;
HX711 scale1;*/

// HX711-2 2 load cells 590g
#define LOADCELL2_DOUT_PIN D4
#define LOADCELL2_SCK_PIN  D3
int Loadcell2Tare=-266617;
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
#define WEIGHT_TAKING_DELAY 10000//after this time (in milliseconds) the weight is measured again.
#define WEIGHT_TAKING_DELAY_FILLING 500//after this time (in milliseconds) the weight is measured again during filling.
int fillingMode = 0;//0=normal functionality, 1=checking for Time Threshold, 2=waiting for filling to stop
//-------weight
#define BOTTLE_WEIGHT_EMPTY 0
#define BOTTLE_WEIGHT_MAX 1000
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
const char* discord = "https://discord.com/api/webhooks/994272805384364072/VKC_5xUUDNtgnyNbaPNp74NiCHGJCPrqfRUdv49O01gCe2J578KxZkMJc95LLNXHJg6W";
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

//returns max weight of water
int maxWeight(){
  maxWeight = BOTTLE_WEIGHT_MAX;
  return BOTTLE_WEIGHT_MAX;
}
//returns amount of last drink
int lastDrankAmount(){
  lastDrankAmount = lastSessionWeight;
  return lastSessionWeight;
}
//returns date of last drink (Hour, Minute, Second)
int lastDrankDate(int i){
  lastDrankDate1 = lastSessionTimestamp[0];
  lastDrankDate2 = lastSessionTimestamp[1];
  lastDrankDate3 = lastSessionTimestamp[2];
  return lastSessionTimestamp[i];
}
//returns amount drank for last week (Array for last week)
int drankDay(int day){
  for (int i = 0; i < 7; i++) {
    drankDay[0] = consumptionWeek[0];
  }

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
  float fillp = getWeight()/BOTTLE_WEIGHT_MAX;
  fillp  = fillp*LED_COUNT;
  strip.clear();
  for(int i=0;i<floorf(fillp);i++){
    strip.setPixelColor(i, strip.Color(0,0,255));
    if(i+1==floorf(fillp)){
      strip.setPixelColor(i+1,strip.Color(0,0,255*(fillp-floorf(fillp))));
    }
    strip.show();

  }
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

  MDNS.begin("ESP8266");
  
  //Methoden für Webseitenaufruf und Parameterübergabe
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

  //discord message
  client.setInsecure();
  //current day for newDay
  timeClient.update();
  currentDay = timeClient.getEpochTime()/86400;

  //Load cell calibration
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

  //Serial.print("Result: ");
  //Serial.println(getWeight());
  //delay(1000);
  


  checkWeight();

  setLed(3);

  MDNS.update();
  
}



