#include <Arduino.h>
#include <HX711.h>
#include <Adafruit_NeoPixel.h> 
#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>

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

#define LED_PIN D5
#define LED_COUNT 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//-------loop timing
int time; //timestamp (in millis since chip was turned on) of the last weight measurement.
#define WEIGHT_TAKING_DELAY 10000//after this time (in milliseconds) the weight is measured again.
#define WEIGHT_TAKING_DELAY_FILLING 500//after this time (in milliseconds) the weight is measured again during filling.
int fillingMode = 0;//0=normal functionality, 1=checking for Time Threshold, 2=waiting for filling to stop
//-------weight
#define BOTTLE_WEIGHT_EMPTY 100
#define PEDESTAL_WEIGHT_EMPTY 100
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
int lastSessionTimestamp; //TODO: good way to store the timestamp

//mqtt settings
IPAddress server(192, 168, 178, 75);//mqtt broker ip
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt";
const char* mqtt_pw = "test123";

WiFiClient espClient;
PubSubClient client(espClient);//mqtt client

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

int getLiquidWeight() { //like getWeight, but only the weight of the actual liquid is returned.
  int temp = getWeight() - BOTTLE_WEIGHT_EMPTY - PEDESTAL_WEIGHT_EMPTY;
  if (temp > 0) {
    return temp;
  } else {
    return 0;
  }
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
  //mqtt Initialization
  client.setServer(server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Initializing LED Strip");
  strip.begin(); 
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
  Serial.println("LED strip Initialized");
  time = millis();
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //weight checking
  if (fillingMode == 1) {
    if (millis() > time + FILLING_THRESHOLD_TIME) {
      time = millis();
      weight = getLiquidWeight();
      if (weight - weightPrev > FILLING_THRESHOLD_WEIGHT) {
        fillingMode = 2;
      } else {
        fillingMode = 0;
      }
    }
  } else {
    if (fillingMode == 2) {
      if (millis() > time + WEIGHT_TAKING_DELAY_FILLING) {
        time = millis();
        weight = getLiquidWeight();
        if (weight - weightPrev < LOADCELL_ERROR_MARGIN) {
          fillingMode = 0;
        }
      }
    } else {
      time = millis();
      if (millis() > time + WEIGHT_TAKING_DELAY){ //checks whether the weight has to be measured again.
        weight = getLiquidWeight();
        if (getLiquidWeight() > PEDESTAL_WEIGHT_EMPTY + LOADCELL_ERROR_MARGIN) {
          weightPrev = weight;
          int deltaWeight = weight - weightPrev;
          if (abs(deltaWeight) > LOADCELL_ERROR_MARGIN) {
            if (deltaWeight > FILLING_THRESHOLD_WEIGHT) {
              fillingMode = 1;
            } else {
              lastSessionWeight = 0 - deltaWeight;
              //TODO change timestamp; Also needs to check whether it's a new day and then shift the data array left
              consumptionWeek[7] = consumptionWeek[7] + lastSessionWeight;
            }
          }
        }
      }
    }
  }
  

  //getWeight();
  //delay(1000);
 /*   for(int i=0;i<strip.numPixels();i++){
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
  }*/
  
}




