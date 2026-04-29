# ESP8266-MQTT-Alarm
An ESP8266-based security solution featuring dual-network fallback logic and MQTT integration for real-time alerting.

### Key Features
* **Smart Network Switching:** Seamlessly transitions between campus (SU-gIoT) and mobile networks.
* **Real-Time Alerts:** Utilizes MQTT protocols to publish instant "BURGLAR DETECTED!" notifications.
* **Resilient Connectivity:** Implements automatic reconnection logic for both WiFi and MQTT.

### Tech Stack
* **Hardware:** ESP8266 (NodeMCU)
* **Language/Libraries:** C++ (Arduino Framework), PubSubClient, ESP8266WiFi
* **Protocols:** MQTT, TCP/IP
