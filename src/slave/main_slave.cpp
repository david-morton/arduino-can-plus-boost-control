#include <Arduino.h>
#include <ptScheduler.h>

#include "alarm/alarm_buzzer.h"
#include "alarm/alarms_slave.h"
#include "engine_speed/engine_speed.h"
#include "lux_sensor/lux_sensor.h"
#include "mux/mux_helpers.h"
#include "mux/mux_readers.h"
#include "pin_assignments_slave.h"
#include "pin_configuration_slave.h"
#include "shared/alarm/alarm_helpers.h"
#include "shared/command_ids.h"
#include "shared/common_task_scheduling.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_helpers.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"
#include "shared/telemetry/telemetry_send_staging.h"
#include "shared/udp_command_dispatcher.h"
#include "shared/variables_programmatic.h"
#include "shared/variables_vehicle_parameters.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */

bool debugError            = true;
bool debugEthernetGeneral  = false;
bool debugEthernetMessages = false;
bool debugEthernetTraffic  = false;
bool debugEthernetPing     = false;
bool debugGears            = false;
bool debugGeneral          = true;
bool debugMux              = false;
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
   VARIABLES: Physical sensor inputs from mux
   ====================================================================== */

bool currentSwitchStateClutchEngaged   = false;
bool currentSwitchStateInNeutral       = false;
int  currentBrakePressureFrontGaugeKpa = 0;
int  currentBrakePressureRearGaugeKpa  = 0;
int  currentCoolantTempCelsius         = 0;
int  currentCrankCaseVacuumGaugeKpa    = 0;
int  currentFuelPressureGaugeKpa       = 0;
int  currentIntakeTempBank1Celsius     = 0;
int  currentIntakeTempBank2Celsius     = 0;
int  currentIntakeTempManifoldCelsius  = 0;
int  currentOilPressureGaugeKpa        = 0;
int  currentOilTempCelsius             = 0;

/* ======================================================================
   VARIABLES: Physical sensor inputs local to Arduino
   ====================================================================== */

int currentAmbientLux                     = 0;
int currentIntakePressureBank1GaugeKpa    = 0;
int currentIntakePressureBank2GaugeKpa    = 0;
int currentIntakePressureManifoldGaugeKpa = 0;
int currentEngineSpeedRpm                 = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */

// High frequency tasks (tens of milliseconds)
ptScheduler ptHandleCommonTasks = ptScheduler(PT_TIME_10MS); // Common tasks that are run on both master and slave Arduinos

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptReadSwitchStateClutch       = ptScheduler(PT_TIME_100MS);
ptScheduler ptReadSwitchStateNeutral      = ptScheduler(PT_TIME_100MS);
ptScheduler ptUpdateAlarmStatesSlave      = ptScheduler(PT_TIME_200MS);
ptScheduler ptUpdateCurrentEngineSpeedRpm = ptScheduler(PT_TIME_200MS); // Initially set to 200ms, will be adjusted based on current RPM

// Low frequency tasks (seconds)
ptScheduler ptReadCurrentLuxReading = ptScheduler(PT_TIME_2S);

/* ======================================================================
   SETUP
   ====================================================================== */

void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("Entering main setup phase ...");

  configureAllPins();
  initialiseEthernetShield(ethConfigLocal);
  initialiseAmbientLightSensor();
  performAlarmBuzzerStartupBeep();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */

void loop() {
  // Increment loop counter so it's up to date when we report performance stats
  arduinoLoopExecutionCount++;

  // Check for, and process any incoming UDP messages as fast as possible within the main loop
  handleIncomingUdpMessage();

  // Handle shared scheduled tasks. Most of these are common tasks that are run on both master and slave Arduinos
  if (ptHandleCommonTasks.call()) {
    handleCommonScheduledTasks();
  }

  // Read ambient light sensor value at a defined interval and store for transmission
  if (ptReadCurrentLuxReading.call()) {
    buildTelemetryItem(SENSOR_LUX, currentAmbientLux = calculateAverageLux());
  }

  // Read the state of clutch and neutral switches, and update current variables
  if (ptReadSwitchStateClutch.call()) {
    buildTelemetryItem(SENSOR_CLUTCH, currentSwitchStateClutchEngaged = readStableMuxDigitalChannelReading(MUX_CHANNEL_CLUTCH_SWITCH, 3, 2));
  }

  if (ptReadSwitchStateNeutral.call()) {
    buildTelemetryItem(SENSOR_NEUTRAL, currentSwitchStateInNeutral = readStableMuxDigitalChannelReading(MUX_CHANNEL_NEUTRAL_SWITCH, 3, 2));
  }

  // Get the current RPM value, and stage for telemetry transmission. This will also update the RPM calculation frequency based on the current RPM
  if (ptUpdateCurrentEngineSpeedRpm.call()) {
    buildTelemetryItem(SENSOR_RPM, currentEngineSpeedRpm = getCurrentEngineSpeedRpm());
    updateRpmSchedulerFrequency(ptUpdateCurrentEngineSpeedRpm, currentEngineSpeedRpm);
  }

  // Update the alarm states based on a range of error conditions and take necessary actions
  if (ptUpdateAlarmStatesSlave.call()) {
    handleAllAlarmStatesSlave();
  }
}
