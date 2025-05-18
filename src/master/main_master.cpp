#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/setup_ethernet.h"
#include "shared/mqtt/functions_mqtt.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: More words
   ====================================================================== */

EthernetConfig ethConfig = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x23, 0x6E},
    .ip  = IPAddress(192, 168, 10, 100)};

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };

  initialiseEthernetShield(ethConfig);
  connectMqttClientToBroker();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {
  // put your main code here, to run repeatedly:
}
