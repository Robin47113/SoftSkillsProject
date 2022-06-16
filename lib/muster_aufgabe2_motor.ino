#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MOTOR1PIN D4
#define MOTOR2PIN D5
#define MOTOR3PIN D6

//folgende vier Parameter anpassen.
//--------------------------------
const char* ssid = "XXX";
const char* password = "XXX";

const char* mqtt_topic_subscribe = "pflanze/+/feuchtigkeit";
//--------------------------------

//folgende vier Parameter nicht ändern
//--------------------------------
const char* mqtt_server = "mqtt.iot.informatik.uni-oldenburg.de";
const int mqtt_port = 2883;
const char* mqtt_user = "sutk";
const char* mqtt_pw = "SoftSkills";
//--------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


//flag für die Motorsteuerung
bool powerMotor1 = false;
bool powerMotor2 = false;
bool powerMotor3 = false;

//Zeitstempel für Non-Blocking Delay
long motor1Stamp = 0;
long motor2Stamp = 0;
long motor3Stamp = 0;

long lastMsg = 0;
int val = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Diese Methode wird aufgerufen, sobald es neue Nachrichten gibt, die über das Topic "wetterID/2" versendet wurden.
void callback(char* topic, byte* payload, unsigned int length) {
  char receivedPayload[length];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    receivedPayload[i] = (char) payload[i];
  }
  Serial.println();

  if (strcmp(topic, "pflanze/1/feuchtigkeit") == 0) {
    Serial.println("Topic für Planze 1 erkannt");
    int feuchtigkeit1 = atoi(receivedPayload);
    if (feuchtigkeit1 <= 10) {
      Serial.println("Pflanze 1 zu trocken");
      //Motor an Flag für die Loop Methode setzen
      powerMotor1 = true;
      //Motor starten
      digitalWrite(MOTOR1PIN, LOW);
      //Systemzeit merken
      motor1Stamp = millis();
    }
  }

  if (strcmp(topic, "pflanze/2/feuchtigkeit") == 0) {
    Serial.println("Topic für Planze 2 erkannt");
    int feuchtigkeit2 = atoi(receivedPayload);
    if (feuchtigkeit2 <= 20) {
      Serial.println("Pflanze 2 zu trocken");
      //Motor an
      powerMotor2 = true;
      digitalWrite(MOTOR2PIN, LOW);
      motor2Stamp = millis();
    }
  }
  if (strcmp(topic, "pflanze/3/feuchtigkeit") == 0) {
    Serial.println("Topic für Planze 3 erkannt");
    int feuchtigkeit3 = atoi(receivedPayload);
    if (feuchtigkeit3 <= 30) {
      Serial.println("Pflanze 3 zu trocken");
      //Motor an
      powerMotor3 = true;
      digitalWrite(MOTOR3PIN, LOW);
      motor3Stamp = millis();
    }
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID: Client ID MUSS inviduell sein, da der MQTT Broker nicht mehrere Clients mit derselben ID bedienen kann
    String clientId = "Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe(mqtt_topic_subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(MOTOR1PIN, OUTPUT);
  pinMode(MOTOR2PIN, OUTPUT);
  pinMode(MOTOR3PIN, OUTPUT);
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //wenn Motor 1 Flag in der Callback auf true gesetzt wurde..
  if (powerMotor1) {
    // dann prüfe, ob seitdem 5000ms vergangen sind..
    if (millis() - motor1Stamp > 5000) {
      //falls ja, dann schalte den Motor aus, sende Nachricht und ändere Flag aus false, damit die erste if-Bedingung falsch ist
      digitalWrite(MOTOR1PIN, HIGH);
      client.publish("motor/1", "OK");
      powerMotor1 = false;
      Serial.println("Bewässerung Pflanze 1 abgeschlossen");
    }
  }

  if (powerMotor2) {
    if (millis() - motor2Stamp > 5000) {
      digitalWrite(MOTOR2PIN, HIGH);
      client.publish("motor/2", "OK");
      powerMotor2 = false;
      Serial.println("Bewässerung Pflanze 2 abgeschlossen");
    }
  }

  if (powerMotor3) {
    if (millis() - motor3Stamp > 5000) {
      digitalWrite(MOTOR3PIN, HIGH);
      client.publish("motor/3", "OK");
      powerMotor3 = false;
      Serial.println("Bewässerung Pflanze 3 abgeschlossen");
    }
  }
}
