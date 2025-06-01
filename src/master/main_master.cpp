#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/command_ids.h"
#include "shared/common/ethernet/ethernet_helpers.h"
#include "shared/common/ethernet/ethernet_ping.h"
#include "shared/common/ethernet/ethernet_receive.h"
#include "shared/common/ethernet/ethernet_send.h"
#include "shared/common/helpers_logging.h"
#include "shared/common/message_handlers_udp.h"
#include "shared/common/variables.h"
#include "shared/mqtt/mqtt_helpers.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugError            = true;
bool debugEthernetGeneral  = false;
bool debugEthernetMessages = false;
bool debugEthernetPing     = false;
bool debugEthernetTraffic  = false;
bool debugGears            = false;
bool debugGeneral          = true;
bool debugPerformance      = true;

/* ======================================================================
   VARIABLES: Ethernet and communication related
   ====================================================================== */

// Define MAC address and IP address for local Arduino
EthernetConfig ethConfigLocal = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x23, 0x6E},
    .ip  = IPAddress(192, 168, 10, 100)};

// Define remote IP address for peer Arduino native messeging
const IPAddress remoteArduinoIp(192, 168, 10, 101);

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;
int           currentLuxReading         = 0; // Variable to store the current lux reading from remote Arduino

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)

// Low frequency tasks (seconds)
ptScheduler ptReportArduinoLoopStats         = ptScheduler(PT_TIME_5S);
ptScheduler ptReportArduinoPingStats         = ptScheduler(PT_TIME_5S);
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_1S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
  connectMqttClientToBroker();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {

  // Check for, and process any incoming UDP messages
  processIncomingUdpMessages();

  // Report ping RTT stats (if needed) from buffer average
  if (ptReportArduinoPingStats.call()) {
    checkPingTimeoutsAndLoss();
  }

  // Issue ping request to remote Arduino
  if (ptSendPingRequestToRemoteArduino.call()) {
    sendArduinoPingRequest();
  }

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugPerformance) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
