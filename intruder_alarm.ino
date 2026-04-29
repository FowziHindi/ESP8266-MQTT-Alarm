#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// On-Campus Wi-Fi
const char* ssid1 = "SU-gIoT";
const char* password1 = "";

// Off-Campus Wi-Fi
const char* ssid2 = "Redmi Note 13 Pro";
const char* password2 = "244466666";

// MQTT Brokers
const char* mqtt_server_oncampus = "uskumru.sabanciuniv.edu";
const char* mqtt_server_offcampus = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp8266/button";

WiFiClient espClient;
PubSubClient client(espClient);

const int buttonPin = 0;
int lastButtonState = HIGH;
bool onCampus = true;

void setup_wifi() {
  Serial.println("Connecting to Wi-Fi...");
  
  // Try SU-IoT
  WiFi.begin(ssid1, password1);
  int retries = 50;
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(500);
    Serial.print(".");
    retries--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to SU-IoT.");
    client.setServer(mqtt_server_oncampus, mqtt_port);
    onCampus = true;
  } else {
    // Switch to off-campus Wi-Fi
    Serial.println("\nFailed to connect to SU-IoT. Trying off-campus Wi-Fi...");
    WiFi.begin(ssid2, password2);
    retries = 50;
    while (WiFi.status() != WL_CONNECTED && retries > 0) {
      delay(500);
      Serial.print(".");
      retries--;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to off-campus network.");
      client.setServer(mqtt_server_offcampus, mqtt_port);
      onCampus = false;
    } else {
      Serial.println("\nFailed to connect to any Wi-Fi network.");
    }
  }
}

void reconnect() {
  int attempts = 0;

  while (!client.connected() && attempts < 10) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP8266Client123")) {
      Serial.println("Connected to MQTT broker.");
    } else {
      Serial.print("Failed to connect to MQTT broker. State: ");
      Serial.println(client.state());
      delay(5000);
      attempts++;
    }
  }

  if (!client.connected()) {
    Serial.println("MQTT connection failed. Skipping MQTT tasks...");
  }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);

  setup_wifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }

  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button pressed!");
    if (client.connected()) {
      client.publish(mqtt_topic, "BURGLAR DETECTED!");
    }
    delay(1000);
  }
  lastButtonState = buttonState;
}
