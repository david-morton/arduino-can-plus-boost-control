#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/command_ids.h"
#include "shared/common/debug_logging.h"
#include "shared/common/ethernet/ethernet_helpers.h"
#include "shared/common/ethernet/ethernet_ping_monitor.h"
#include "shared/common/ethernet/ethernet_receive_udp.h"
#include "shared/common/ethernet/ethernet_send_udp.h"
#include "shared/common/telemetry_receive_parser.h"
#include "shared/common/telemetry_send_staging.h"
#include "shared/common/udp_command_dispatcher.h"
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
bool debugPerformance      = false;
bool debugSensorReadings   = true;
bool debugTelemetry        = true;

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
int           currentElectronicsTemp    = 0; // Variable to store the current electronics temperature reading from RTC

float valueFromRemote; // Scratch variable to transport variable values from remote Arduino

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)

// Low frequency tasks (seconds)
ptScheduler ptReportArduinoPerformanceStats  = ptScheduler(PT_TIME_5S);
ptScheduler ptReportArduinoPingStats         = ptScheduler(PT_TIME_5S);
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_1S);
ptScheduler ptGetCurrentLuxReading           = ptScheduler(PT_TIME_2S);
ptScheduler ptReadElectronicsTemperature     = ptScheduler(PT_TIME_10S);

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

  // Check for, and process any incoming UDP messages as fast as possible within the main loop
  handleIncomingUdpMessage();

  // Report ping RTT stats (if needed) from buffer average
  if (ptReportArduinoPingStats.call()) {
    handlePingTimeoutsAndLoss();
  }

  // Issue ping request to remote Arduino
  if (ptSendPingRequestToRemoteArduino.call()) {
    sendArduinoPingRequest();
  }

  // Update the current lux reading from the remote Arduino
  if (ptGetCurrentLuxReading.call() && handleTelemetryFloat(SENSOR_LUX, &valueFromRemote)) {
    currentLuxReading = valueFromRemote;
  }

  // Read the electronics temperature from the RTC sensor
  if (ptReadElectronicsTemperature.call()) {
  }

  // Increment loop counter and report on performance stats if needed
  if (debugPerformance && millis() > 10000) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoPerformanceStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
