#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/setup_ethernet.h"
#include "shared/mqtt/functions_mqtt.h"

#define CAN_2515

EthernetConfig ethConfig = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x8D},
    .ip = IPAddress(192, 168, 11, 3)};

void setup() {
  initialiseEthernetShield(ethConfig);
  connectMqttClientToBroker();
}

void loop() {
  // put your main code here, to run repeatedly:
}
