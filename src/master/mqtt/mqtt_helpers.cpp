#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "../../shared/debug_logging.h"
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
    DEBUG_GENERAL("Connecting to MQTT broker...");
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setKeepAlive(5);

    if (mqttClient.connect("arduino-client")) {
      DEBUG_GENERAL("\t\tMQTT Client connected");
      mqttBrokerConnected = true;
    } else {
      DEBUG_ERROR("\t\tMQTT Client NOT connected");
      mqttBrokerConnected = false;
    }
  }
}

// Publish metric via MQTT
void sendMqttMetric(String topic, String metricName, int metricValue) {
  if (mqttBrokerConnected) {
    String payload = "{\"" + metricName + "\":" + String(metricValue) + "}";
    mqttClient.publish(topic.c_str(), payload.c_str());
  }
}

void sendMqttMetric(String topic, String metricName, String metricValue) {
  if (mqttBrokerConnected) {
    String payload = "{\"" + metricName + "\":" + String(metricValue) + "}";
    mqttClient.publish(topic.c_str(), payload.c_str());
  }
}