#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/setup_ethernet.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: More words
   ====================================================================== */

EthernetConfig ethConfig = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x1F, 0xF4},
    .ip  = IPAddress(192, 168, 10, 101)};

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };

  Serial.println("INFO: Entering main setup phase ...\n");

  initialiseEthernetShield(ethConfig);
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {
  Serial.println("BLA");
  delay(1000);
}
