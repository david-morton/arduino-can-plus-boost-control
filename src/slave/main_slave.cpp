#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/ethernet_helpers.h"
#include "shared/common/general_helpers.h"
#include "shared/common/variables.h"

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
   VARIABLES: Ethernet and communication related
   ====================================================================== */

// Define MAC address and IP address for local Arduino
EthernetConfig ethConfigLocal = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x1F, 0xF4},
    .ip  = IPAddress(192, 168, 10, 101)};

// Define remote IP address for peer Arduino native messeging
IPAddress remoteArduinoIp(192, 168, 10, 100);

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats  = ptScheduler(PT_TIME_5S);
ptScheduler ptSendTestEthernetMessage = ptScheduler(PT_TIME_1S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
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

  // Send test UDP message to remote Arduino
  if (ptSendTestEthernetMessage.call()) {
    String message = "Hello from Arduino 1";
    sendUdpMessage(remoteArduinoIp, message.c_str());
  }
}
