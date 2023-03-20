#include <WiFi.h>
#include <PubSubClient.h>
#include <esp32-hal-ledc.h>


const char* SSID = "👻";
const char* PASSWORD = "peepeepoopoo";
// const char* MQTT_SERVER = "192.168.1.110";
const char* MQTT_SERVER = "192.168.17.104";
const int MQTT_PORT = 1883;

const char* PUBLISHER_TOPIC_1 = "lamp/status";
const char* PUBLISHER_TOPIC_2 = "ac/status";
const char* PUBLISHER_TOPIC_3 = "lamp/time";
const char* PUBLISHER_TOPIC_4 = "ac/time";
const char* SUBSCRIBER_TOPIC_1 = "lamp/command";
const char* SUBSCRIBER_TOPIC_2 = "ac/command";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const int LED_PIN = 2;

bool isLampOn;
bool isACOn;
int lampTime;
int acTime;
int lampStartTime;
int acStartTime;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle incoming mqtt messages if needed
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (strcmp(topic, SUBSCRIBER_TOPIC_1) == 0) {
    if (message == "ON") {
      isLampOn = true;
    } else if (message == "OFF") {
      isLampOn = false;
    }
  } else if (strcmp(topic, SUBSCRIBER_TOPIC_2) == 0) {
    if (message == "ON") {
      isACOn = true;
    } else if (message == "OFF") {
      isACOn = false;
    }
  }
}

void reconnect() {
  // loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // create a random client id
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // attempt to connect
    WiFi.mode(WIFI_STA);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // once connected, subscribe to any mqtt topics if needed
      mqttClient.subscribe(SUBSCRIBER_TOPIC_1);
      mqttClient.subscribe(SUBSCRIBER_TOPIC_2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);           // set the led pin as an output
  Serial.begin(115200);               // start the serial port
  setup_wifi();                       // start setting up the wifi connection
  mqttClient.setClient(wifiClient);   // set the mqtt client
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);

  ledcSetup(0, 5000, 8);      // channel 0, 5khz frequency, 8-bit resolution
  ledcAttachPin(LED_PIN, 0);  // attach the led pin to channel 0

  // set initial state
  isLampOn = true;
  isACOn = true;
  lampStartTime = millis();
  acStartTime = millis();
}

void loop() {
  // check if we're connected to the mqtt server
  if (!mqttClient.connected()) {
    reconnect();
  }

  // handle any incoming mqtt messages
  mqttClient.loop();

  // handle iots
  int startTime = millis();
  if (isLampOn) {
    ledcWrite(0, 256);
  } else {
    ledcWrite(0, 0);
    lampStartTime = millis();
  }
  // publish the status to mqtt server
  String message = isLampOn ? "ON" : "OFF";
  mqttClient.publish(PUBLISHER_TOPIC_1, message.c_str());

  while (millis() - startTime < 1000) {
    // do nothing
  }
  lampTime = isLampOn ?  millis() - lampStartTime : 0;

  if (isACOn) {
    ledcWrite(0, 16);
  } else {
    ledcWrite(0, 0);
    acStartTime = millis();
  }
  // publish the status to mqtt server
  String message2 = isACOn ? "ON" : "OFF";
  mqttClient.publish(PUBLISHER_TOPIC_2, message2.c_str());

  while (millis() - startTime < 2000) {
    // do nothing
  }
  acTime = isACOn ?  millis() - acStartTime : 0;

  // publish the time to mqtt server
  String message3 = String(lampTime);
  mqttClient.publish(PUBLISHER_TOPIC_3, message3.c_str());
  String message4 = String(acTime);
  mqttClient.publish(PUBLISHER_TOPIC_4, message4.c_str());
}
