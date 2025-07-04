#include <Arduino.h>
#include <mcp2515_can.h>
#include <ptScheduler.h>

#include "alarm/alarms_master.h"
#include "can/can_helpers.h"
#include "can/can_receive.h"
#include "can/can_send.h"
#include "check_light/check_light.h"
#include "mqtt/mqtt_helpers.h"
#include "rtc/rtc_sensor.h"
#include "shared/alarm/alarm_helpers.h"
#include "shared/command_ids.h"
#include "shared/common_task_scheduling.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_helpers.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"
#include "shared/telemetry/telemetry_receive_parser.h"
#include "shared/telemetry/telemetry_send_staging.h"
#include "shared/udp_command_dispatcher.h"
#include "shared/variables_programmatic.h"
#include "shared/variables_vehicle_parameters.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */

bool debugCanBmw           = false;
bool debugCanNissan        = true;
bool debugError            = true;
bool debugEthernetGeneral  = false;
bool debugEthernetMessages = false;
bool debugEthernetPing     = false;
bool debugEthernetTraffic  = false;
bool debugGears            = false;
bool debugGeneral          = true;
bool debugPerformance      = true;
bool debugSensorReadings   = true;
bool debugTelemetry        = false;

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
   VARIABLES: Engine parameters from CAN or remote Arduino
   ====================================================================== */

int currentEngineTempCelcius = 0; // Read from Nissan CAN

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

float valueFromRemote; // Scratch variable to transport variable values from remote Arduino

/* ======================================================================
   VARIABLES: Physical sensor inputs local to Arduino
   ====================================================================== */

int currentLuxReading      = 0; // Variable to store the current lux reading from remote Arduino
int currentElectronicsTemp = 0; // Variable to store the current electronics temperature reading from RTC

/* ======================================================================
   VARIABLES: Read from slave Arduino
   ====================================================================== */

bool currentSwitchStateClutch;
bool currentSwitchStateNeutral;
int  currentEngineSpeedRpm = 0;

/* ======================================================================
   VARIABLES: Read from CAN buses
   ====================================================================== */

float currentVehicleSpeedFrontKph = 0; // Vehicle speed from front wheels
float currentVehicleSpeedRearKph  = 0; // Vehicle speed from rear wheels

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */

// High frequency tasks (tens of milliseconds)
ptScheduler ptReadCurrentEngineSpeedRpm = ptScheduler(PT_TIME_50MS);
ptScheduler ptHandleCommonTasks         = ptScheduler(PT_TIME_10MS); // Common tasks that are run on both master and slave Arduinos
ptScheduler ptSendCanMessages           = ptScheduler(PT_TIME_10MS); // Send CAN messages to BMW network

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptReadSwitchStateClutch   = ptScheduler(PT_TIME_100MS);
ptScheduler ptReadSwitchStateNeutral  = ptScheduler(PT_TIME_100MS);
ptScheduler ptUpdateAlarmStatesMaster = ptScheduler(PT_TIME_200MS);

// Low frequency tasks (seconds)
ptScheduler ptUpdateCheckLightStatus    = ptScheduler(PT_TIME_1S);
ptScheduler ptGetCurrentLuxReading      = ptScheduler(PT_TIME_2S);
ptScheduler ptGetElectronicsTemperature = ptScheduler(PT_TIME_1MIN);

/* ======================================================================
   SETUP
   ====================================================================== */

void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("Entering main setup phase ...");

  initialiseCanModules();
  initialiseEthernetShield(ethConfigLocal);
  connectMqttClientToBroker();
  initialiseRtc();
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */

void loop() {
  // Increment loop counter so it's up to date when we report performance stats
  arduinoLoopExecutionCount++;

  // Check for, and process any incoming CAN messages as fast as possible within the main loop
  checkAndProcessCanMessages();

  // Check for, and process any incoming UDP messages as fast as possible within the main loop
  handleIncomingUdpMessage();

  // Handle shared scheduled tasks. Most of these are common tasks that are run on both master and slave Arduinos
  if (ptHandleCommonTasks.call()) {
    handleCommonScheduledTasks();
  }

  // Update the current lux reading from the remote Arduino
  if (ptGetCurrentLuxReading.call() && handleTelemetryFloat(SENSOR_LUX, &valueFromRemote)) {
    currentLuxReading = valueFromRemote;
  }

  // Update the current clutch and neutral statuses from the remote Arduino
  if (ptReadSwitchStateClutch.call() && handleTelemetryFloat(SENSOR_CLUTCH, &valueFromRemote)) {
    currentSwitchStateClutch = valueFromRemote;
  }

  if (ptReadSwitchStateNeutral.call() && handleTelemetryFloat(SENSOR_NEUTRAL, &valueFromRemote)) {
    currentSwitchStateNeutral = valueFromRemote;
  }

  // Update the current RPM reading from the remote Arduino
  // If the remote Arduino is not sending new RPM data, this will not update
  if (ptReadCurrentEngineSpeedRpm.call() && handleTelemetryFloat(SENSOR_RPM, &valueFromRemote)) {
    currentEngineSpeedRpm = valueFromRemote;
  }

  // Read the electronics temperature from the RTC sensor
  if (ptGetElectronicsTemperature.call()) {
    currentElectronicsTemp = getRtcCurrentTemperature();
  }

  // Update the status of the check light
  if (ptUpdateCheckLightStatus.call()) {
    updateCheckLightStatus();
  }

  // Send required CAN messages to the BMW network
  if (ptSendCanMessages.call()) {
    sendCanMessages();
  }

  // Update the alarm states based on a range of error conditions and take necessary actions
  if (ptUpdateAlarmStatesMaster.call()) {
    handleAllAlarmStatesMaster();
  }
}
