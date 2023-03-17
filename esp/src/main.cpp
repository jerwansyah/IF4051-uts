#include <WiFi.h>
#include <PubSubClient.h>


const char* SSID = "TP-Link_8E6C";
const char* PASSWORD = "66101710";
const char* MQTT_SERVER = "192.168.1.110";
const int MQTT_PORT = 1883;

const char* PUBLISHER_TOPIC = "blink/frequency";
const char* SUBSCRIBER_TOPIC = "test";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int frequency = 10;                 // initial frequency is 10 Hz
int intervalMs = 1000 / frequency;  // LED on/off interval based on the frequency
bool isButtonPressed = false;       // button flag to avoid multiple increments
bool prevButtonState = false;       // button flag to avoid multiple increments

const int LED_PIN = 2;
const int BUTTON_PIN = 0;
const int MAX_FREQUENCY = 30;


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
  // handle incoming MQTT messages if needed
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);
}

void reconnect() {
  // loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // attempt to connect
    WiFi.mode(WIFI_STA);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // once connected, subscribe to any MQTT topics if needed
      mqttClient.subscribe(SUBSCRIBER_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);           // set the LED pin as an output
  pinMode(BUTTON_PIN, INPUT);         // set the button pin as an input
  Serial.begin(115200);               // start the serial port
  setup_wifi();                       // start setting up the WiFi connection
  mqttClient.setClient(wifiClient);   // set the MQTT client
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);

  digitalRead(BUTTON_PIN);
}

void loop() {
  // check if we're connected to the MQTT server
  if (!mqttClient.connected()) {
    reconnect();
  }

  // handle any incoming MQTT messages
  mqttClient.loop();

  // clear the isButtonPressed flag if the button is not pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    isButtonPressed = true;
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    isButtonPressed = false;
  }

  // check if the button is pressed
  // increment the frequency if it is
  if (isButtonPressed && isButtonPressed != prevButtonState) {
    Serial.println("Button pressed");
    frequency++;

    // reset the frequency if it exceeds the maximum
    if (frequency > MAX_FREQUENCY) {
      frequency = 10;
    }

    intervalMs = 1000 / frequency;
  }
  prevButtonState = isButtonPressed;

  // toggle the LED on/off
  digitalWrite(LED_PIN, HIGH);
  delay(intervalMs / 2);
  digitalWrite(LED_PIN, LOW);
  delay(intervalMs / 2);

  // print the frequency to the serial port
  Serial.print("13519116: ");
  Serial.print(frequency);
  Serial.print(", publishing to ");
  Serial.println(PUBLISHER_TOPIC);

  // publish the frequency to mqtt server
  String message = "13519116: " + String(frequency);
  mqttClient.publish(PUBLISHER_TOPIC, message.c_str());
}
