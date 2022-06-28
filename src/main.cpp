#include <Arduino.h>
#include <HX711.h>
#include <Adafruit_NeoPixel.h> 
#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
//for time
#include <WiFiUdp.h>
#include <NTPClient.h>
//spiffs
#include <FS.h>

// HX711-1 (1 load cell)
#define LOADCELL1_DOUT_PIN D2
#define LOADCELL1_SCK_PIN  D1
int Loadcell1Tare;
HX711 scale1;

// HX711-2 2 load cells
#define LOADCELL2_DOUT_PIN D4
#define LOADCELL2_SCK_PIN  D3
int Loadcell2Tare;
HX711 scale2;
//leds
#define LED_PIN D5
#define LED_COUNT 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


//mqtt settings
IPAddress server(192, 168, 178, 75);//mqtt broker ip
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt";
const char* mqtt_pw = "test123";

WiFiClient espClient;
PubSubClient client(espClient);//mqtt client
//for time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);




//mqtt callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//mqtt reconnecting
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient",mqtt_user,mqtt_pw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void tare(){
  Loadcell1Tare = scale1.read_average(10);
  Loadcell2Tare = scale2.read_average(10);
}
//returns weight in gramms
int getWeight(){
  if (scale1.is_ready()) {
    long reading = scale1.read_average(10)-Loadcell1Tare;
    Serial.print("HX711-1 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711-1 not found.");
  }


  if (scale2.is_ready()) {
    long reading = scale2.read_average(10)-Loadcell2Tare;
    Serial.print("HX711-2 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711-2 not found.");
  }
Serial.println();
return 1;
}
//saves amount(g) to saved.txt with timestamp
void drank (int amount){
if (SPIFFS.exists("/saved.txt")) {
 File f = SPIFFS.open("/saved.txt", "r");
      if (!f) {
        Serial.print("Unable To Open '");
        Serial.print("/saved.txt");
        Serial.println("' for Reading");
        Serial.println();
      } else {
        String s;
        Serial.print("Contents of file '");
        Serial.print("/saved.txt");
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
  } else {
      Serial.print("Unable To Find ");
      Serial.println("/saved.txt");
      Serial.println();
  }
}
//amount drank that day
int drankday(){
  return 0;
}

int drankLastDate(){
  return 0;
} 

int drankLastAmount(){
  return 0;
}





void setup() {
  Serial.begin(115200);

  Serial.println("Initializing the scale1");
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  Serial.println("Scale1 Initialized");
  	
  Serial.println("Initializing the scale2");
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  Serial.println("Scale2 Initialized");

  tare();

  //Wifimanager Initialization
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  /*//mqtt Initialization
  client.setServer(server, mqtt_port);
  client.setCallback(callback);*/

  Serial.println("Initializing LED Strip");
  strip.begin(); 
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
   Serial.println("LED strip Initialized");
   //timeclient init
   timeClient.begin();

// Startint spiffs
  if (SPIFFS.begin()) {
      Serial.println("SPIFFS Active");
      Serial.println();
  } else {
      Serial.println("Unable to activate SPIFFS");
  }
}

void loop() {

  /*//mqtt reconnecting 
   if (!client.connected()) {
    reconnect();
  }
  //mqtt loop
  client.loop();*/


timeClient.update();
Serial.println(timeClient.getFormattedTime());

getWeight();

delay(1000);

//led test
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
for(int i=0;i<strip.numPixels();i++){
  strip.setPixelColor(i, strip.Color(255, 0, 0));
  strip.show();
  delay(100);
}
  
}




