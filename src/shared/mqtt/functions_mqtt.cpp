#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "functions_mqtt.h"
// #include "../master/setup_ethernet.h"

// Create ethernet client from external
extern EthernetClient ethClient;

// Define toggle for connection state
bool mqttBrokerConnected = false;

// Configure MQTT client
PubSubClient mqttClient(ethClient);    // Create MQTT client on ethernet
IPAddress mqtt_server(192, 168, 11, 2); // Grafana server address on Raspberry Pi
const int mqtt_port = 1883;             // Grafana server port on Raspberry Pi

// Function for creating the MQTT client and connecting to server
void connectMqttClientToBroker() {
  if (!mqttClient.connected()) {
    Serial.println("INFO - Connecting to MQTT broker");
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setKeepAlive(5);
    if (mqttClient.connect("arduino-client")) {
      Serial.println("\tOK - MQTT Client connected");
      mqttBrokerConnected = true;
    } else {
      Serial.println("\tFATAL - MQTT Client not connected");
      mqttBrokerConnected = false;
    }
  }
}

// Publish metric via MQTT
void publishMqttMetric(String topic, String metricName, int metricValue) {
  if (mqttBrokerConnected) {
    String payload = "{\"" + metricName + "\":" + String(metricValue) + "}";
    mqttClient.publish(topic.c_str(), payload.c_str());
  }
}

void publishMqttMetric(String topic, String metricName, String metricValue) {
  if (mqttBrokerConnected) {
    String payload = "{\"" + metricName + "\":" + String(metricValue) + "}";
    mqttClient.publish(topic.c_str(), payload.c_str());
  }
}