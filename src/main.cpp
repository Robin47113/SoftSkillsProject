#include <Arduino.h>
#include <HX711.h>//load cells
#include <Adafruit_NeoPixel.h> //leds
#include <ESP8266WiFi.h>
//#include <PubSubClient.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
#include <FS.h>//spiffs
#include <NTPClient.h>//ntp (time)
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

/*// HX711-1 (1 load cell)
#define LOADCELL1_DOUT_PIN D2
#define LOADCELL1_SCK_PIN  D1
int Loadcell1Tare;
HX711 scale1;*/

// HX711-2 2 load cells 590g   -13434
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

//mqtt settings
//IPAddress server(192, 168, 178, 75);//mqtt broker ip
//const int mqtt_port = 1883;
//const char* mqtt_user = "mqtt";
//const char* mqtt_pw = "test123";

//WiFiClient espClient;
//PubSubClient client(espClient);//mqtt client

//ntp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
//discord webhook
const char* discord = "https://discord.com/api/webhooks/994272805384364072/VKC_5xUUDNtgnyNbaPNp74NiCHGJCPrqfRUdv49O01gCe2J578KxZkMJc95LLNXHJg6W";
const boolean discord_tts = false;
WiFiClientSecure client;
HTTPClient https;

/*//mqtt callback
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
}*/

//returns max weight of water
int maxWeight(){
  return 1;
}
//returns amount of last drink
int lastDrankAmount(){
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

  return 1;
}
//returns date of last drink
int lastDrankDate(){
  return 1;
}

//returns amount drank for last week
int drankDay(int day){
  return 1;
}

//returns Water left in gramms
int getWeight(){
return (scale2.get_units(10)-Loadcell2Tare)/Loadcell2cal;
}
//saves amount drank
void drank(int amount){
  
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


void setup() {
  Serial.begin(115200);
  delay(500);
  /*Serial.println("Initializing the scale1");
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  Serial.println("Scale1 Initialized");*/
  	
  Serial.println("Initializing the scale2");
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  Serial.println("Scale2 Initialized");

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
  }*/

}

void loop() {
  //mqtt loop
  /*//mqtt reconnecting 
   if (!client.connected()) {
    reconnect();
  }
  client.loop();*/


      
     
  //discord_send("test");

  Serial.print("Result: ");
  Serial.println(getWeight());
  delay(1000);


  timeClient.update();
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(+timeClient.getSeconds());
  Serial.println(timeClient.getEpochTime()/86400);//current day from 1.Jan 1970
  //Led test
  /*for(int i=0;i<strip.numPixels();i++){
   strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
    delay(100);
  }*/
  
}




