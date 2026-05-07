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
const char* mqtt_topic = "esp8266/intruder";

WiFiClient espClient;
PubSubClient client(espClient);

// PIR Sensor
const int pirPin = 2;        // D4 on NodeMCU (GPIO2)
int pirState = LOW;          // current sensor state
int pirVal = 0;              // reading from sensor

bool onCampus = true;

// PIR needs ~30s warmup on power-on
const unsigned long PIR_WARMUP_MS = 30000;
unsigned long startTime = 0;
bool pirReady = false;

void setup_wifi() {
  Serial.println("Connecting to Wi-Fi...");

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
    Serial.println("\nFailed SU-IoT. Trying off-campus...");
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
      Serial.print("Failed. State: ");
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
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  Serial.println("PIR warming up (30s)...");
  startTime = millis();
  setup_wifi();
}

void loop() {
  // Wait for PIR warmup before reading
  if (!pirReady) {
    if (millis() - startTime >= PIR_WARMUP_MS) {
      pirReady = true;
      Serial.println("PIR ready.");
    } else {
      return; // still warming up
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }

  pirVal = digitalRead(pirPin);

  if (pirVal == HIGH) {
    if (pirState == LOW) {
      // Motion just detected
      Serial.println("Motion detected!");
      if (client.connected()) {
        client.publish(mqtt_topic, "BURGLAR DETECTED!");
      }
      pirState = HIGH;
      delay(1000); // debounce — avoid flooding MQTT
    }
  } else {
    if (pirState == HIGH) {
      Serial.println("Motion stopped.");
      pirState = LOW;
    }
  }
}
