#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "lux_sensor/lux_sensor.h"
#include "mux/mux_helpers.h"
#include "mux/mux_readers.h"
#include "pin_assignments_slave.h"
#include "shared/command_ids.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_helpers.h"
#include "shared/ethernet/ethernet_ping_monitor.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"
#include "shared/telemetry_send_staging.h"
#include "shared/udp_command_dispatcher.h"
#include "shared/variables_programmatic.h"
#include "shared/variables_vehicle_parameters.h"

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
bool debugMux              = true;
bool debugPerformance      = false;
bool debugSensorReadings   = false;
bool debugTelemetry        = true;

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

/* ======================================================================
   VARIABLES: Physical sensor inputs from mux
   ====================================================================== */

bool currentSwitchStateClutch         = false;
bool currentSwitchStateNeutral        = false;
int  currentBrakePressureFrontKpa     = 0;
int  currentBrakePressureRearKpa      = 0;
int  currentCoolantTempCelsius        = 0;
int  currentCrankCaseVacuumKpa        = 0;
int  currentFuelPressureKpa           = 0;
int  currentIntakeTempBank1Celsius    = 0;
int  currentIntakeTempBank2Celsius    = 0;
int  currentIntakeTempManifoldCelsius = 0;
int  currentOilPressureKpa            = 0;
int  currentOilTempCelsius            = 0;

/* ======================================================================
   VARIABLES: Physical sensor inputs direct to Arduino
   ====================================================================== */

int currentAmbientLux                = 0;
int currentGear                      = 0;
int currentIntakePressureBank1Kpa    = 0;
int currentIntakePressureBank2Kpa    = 0;
int currentIntakePressureManifoldKpa = 0;
int currentRpm                       = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */

// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)
// ptScheduler ptReadSwitchStateClutch  = ptScheduler(PT_TIME_100MS);
// ptScheduler ptReadSwitchStateNeutral = ptScheduler(PT_TIME_100MS);
ptScheduler ptReadSwitchStateClutch  = ptScheduler(PT_TIME_1S);
ptScheduler ptReadSwitchStateNeutral = ptScheduler(PT_TIME_1S);

// Low frequency tasks (seconds)
ptScheduler ptReadAmbientLightReading       = ptScheduler(PT_TIME_2S);
ptScheduler ptReportArduinoPerformanceStats = ptScheduler(PT_TIME_5S);

// Send different message classes to remote Arduino
ptScheduler ptSendLowFrequencyMessages    = ptScheduler(PT_TIME_1S);
ptScheduler ptSendMediumFrequencyMessages = ptScheduler(PT_TIME_100MS);
ptScheduler ptSendHighFrequencyMessages   = ptScheduler(PT_TIME_20MS);

/* ======================================================================
   SETUP
   ====================================================================== */

void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("Entering main setup phase ...");

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
    buildTelemetryItem(SENSOR_LUX, currentAmbientLux = calculateAverageLux());
  }

  // Read the state of clutch and neutral switches, and update current variables
  if (ptReadSwitchStateClutch.call()) {
    buildTelemetryItem(SENSOR_CLUTCH, currentSwitchStateClutch = readStableMuxDigitalChannelReading(MUX_CHANNEL_CLUTCH_SWITCH, 3, 0));
  }

  if (ptReadSwitchStateNeutral.call()) {
    buildTelemetryItem(SENSOR_NEUTRAL, currentSwitchStateNeutral = readStableMuxDigitalChannelReading(MUX_CHANNEL_NEUTRAL_SWITCH, 3, 0));
  }

  // Send low frequency messages
  if (ptSendLowFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_SLAVE_LOW_FREQUENCY, CMD_LOW_FREQUENCY_MESSAGES);
  }

  // Send medium frequency messages
  if (ptSendMediumFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_SLAVE_MED_FREQUENCY, CMD_MED_FREQUENCY_MESSAGES);
  }

  // Send high frequency messages
  if (ptSendHighFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_SLAVE_HIGH_FREQUENCY, CMD_HIGH_FREQUENCY_MESSAGES);
  }

  // Increment loop counter and report on performance stats if needed
  if (debugPerformance && millis() > 10000) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoPerformanceStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
