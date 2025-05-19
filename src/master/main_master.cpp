#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/ethernet_helpers.h"
#include "shared/common/general_helpers.h"
#include "shared/mqtt/mqtt_helpers.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugEthernetFunctionality = true;
bool debugEthernetTraffic       = true;
bool debugGears                 = true;
bool debugGeneral               = true;
bool debugLoopInfo              = true;

/* ======================================================================
   VARIABLES: Ethernet related
   ====================================================================== */

// Define MAC address and IP address for local Arduino
EthernetConfig ethConfigLocal = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x23, 0x6E},
    .ip  = IPAddress(192, 168, 10, 100)};

// Define remote IP address and UDP port for peer Arduino native messeging
IPAddress          remoteArduinoIp(192, 168, 10, 101);
const unsigned int remoteArduinoListenPort = 8888;

// Define local listening port for incoming Arduino native messages
const unsigned int localArduinoListenPort = 8888;

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks
ptScheduler ptCheckForEthernetMessages = ptScheduler(PT_TIME_100MS);

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats = ptScheduler(PT_TIME_5S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };

  DEBUG_GENERAL("INFO: Entering main setup phase ...\n");

  initialiseEthernetShield(ethConfigLocal);
  connectMqttClientToBroker();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugLoopInfo) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }

  // Check for incoming UDP packets
  if (ptCheckForEthernetMessages.call()) {
    checkForIncomingUdpMessage();
  }
}
