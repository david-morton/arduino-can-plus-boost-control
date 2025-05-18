#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "mqtt_helpers.h"

/* ======================================================================
   CONFIGURE THINGS
   ====================================================================== */

// Create ethernet client from external
extern EthernetClient ethClient;

// Define toggle for connection state
bool mqttBrokerConnected = false;

// Configure MQTT client
PubSubClient mqttClient(ethClient);          // Create MQTT client on ethernet
IPAddress    mqtt_server(192, 168, 10, 102); // Grafana server address on Orange Pi
const int    mqtt_port = 1883;               // Grafana server port on Orange Pi

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Connect MQTT client to broker
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