#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/command_ids.h"
#include "shared/common/debug_logging.h"
#include "shared/common/ethernet/ethernet_helpers.h"
#include "shared/common/ethernet/ethernet_ping_monitor.h"
#include "shared/common/ethernet/ethernet_receive_udp.h"
#include "shared/common/ethernet/ethernet_send_udp.h"
#include "shared/common/telemetry_send_staging.h"
#include "shared/common/udp_command_dispatcher.h"
#include "shared/common/variables.h"
#include "shared/lux_sensor/lux_sensor.h"

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
bool debugEthernetTraffic  = false;
bool debugEthernetPing     = false;
bool debugGears            = false;
bool debugGeneral          = false;
bool debugPerformance      = false;
bool debugSensorReadings   = false;
bool debugTelemetry        = false;

/* ======================================================================
   VARIABLES: Ethernet and communication related
   ====================================================================== */

// Define MAC address and IP address for local Arduino
EthernetConfig ethConfigLocal = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x1F, 0xF4},
    .ip  = IPAddress(192, 168, 10, 101)};

// Define remote IP address for peer Arduino native messeging
const IPAddress remoteArduinoIp(192, 168, 10, 100);

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;
int           currentLuxReading         = 0; // Variable to store the current lux reading from ambient light sensor

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)

// Low frequency tasks (seconds)
ptScheduler ptReadAmbientLightReading       = ptScheduler(PT_TIME_2S);
ptScheduler ptReportArduinoPerformanceStats = ptScheduler(PT_TIME_5S);
ptScheduler ptSendLowFrequencyMessages      = ptScheduler(PT_TIME_1S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
  initialiseAmbientLightSensor();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {

  // Check for, and process any incoming UDP messages as fast as possible within the main loop
  handleIncomingUdpMessage();

  // Read ambient light sensor value at a defined interval and store for transmission
  if (ptReadAmbientLightReading.call()) {
    buildTelemetryItem(SENSOR_LUX, currentLuxReading = calculateAverageLux());
  }

  // Send low frequency messages at a defined interval (command ID 2)
  if (ptSendLowFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_SLAVE_LOW_FREQUENCY, CMD_LOW_FREQUENCY_MESSAGES);
  }

  // Increment loop counter and report on performance stats if needed
  if (debugPerformance && millis() > 10000) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoPerformanceStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
