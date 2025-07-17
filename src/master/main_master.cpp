#include <Arduino.h>
#include <mcp2515_can.h>
#include <ptScheduler.h>

#include "alarm/alarms_master.h"
#include "boost/boost_helpers.h"
#include "can/can_helpers.h"
#include "can/can_receive.h"
#include "can/can_send.h"
#include "check_light/check_light.h"
#include "gear/gear.h"
#include "mqtt/mqtt_helpers.h"
#include "rtc/rtc_sensor.h"
#include "sd_card/sd_helpers.h"
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
#include "telemetry/receive_from_slave.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */

bool debugBoost            = true;
bool debugCanBmw           = false;
bool debugCanNissan        = false;
bool debugError            = true;
bool debugEthernetGeneral  = false;
bool debugEthernetMessages = false;
bool debugEthernetPing     = false;
bool debugEthernetTraffic  = false;
bool debugGears            = false;
bool debugGeneral          = true;
bool debugPerformance      = true;
bool debugSdCard           = false;
bool debugSensorReadings   = false;
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
   VARIABLES: Read from CAN buses
   ====================================================================== */

int  currentEngineTempCelcius     = 0;  // Read from Nissan CAN
float currentVehicleSpeedFrontKph = 0; // Read from BMW CAN
float currentVehicleSpeedRearKph  = 0; // Read from BMW CAN

/* ======================================================================
   VARIABLES: Physical sensor inputs local to master Arduino
   ====================================================================== */

int currentElectronicsRtcTemp = 0; // Variable to store the current electronics temperature reading from RTC

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */

// High frequency tasks (tens of milliseconds)
ptScheduler ptHandleCommonTasks               = ptScheduler(PT_TIME_10MS); // Common tasks that are run on both master and slave Arduinos
ptScheduler ptHandleTelemetryReceivedFromSlave = ptScheduler(PT_TIME_10MS); // Handle telemetry data received from remote Arduino
ptScheduler ptSendCanMessages                 = ptScheduler(PT_TIME_10MS); // Send CAN messages to BMW network
ptScheduler ptHandleSdCardTasks               = ptScheduler(PT_TIME_50MS); // Handle SD card tasks

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptUpdateAlarmStatesMaster = ptScheduler(PT_TIME_200MS);
ptScheduler ptUpdateBoostTargetKpa    = ptScheduler(PT_TIME_200MS);
ptScheduler ptUpdateCurrentGear       = ptScheduler(PT_TIME_100MS);

// Low frequency tasks (seconds)
ptScheduler ptUpdateCheckLightStatus    = ptScheduler(PT_TIME_1S);
ptScheduler ptGetElectronicsTemperature = ptScheduler(PT_TIME_1MIN);

/* ======================================================================
   SETUP
   ====================================================================== */

void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("Entering main setup phase ...");

  initialiseRtc();
  initialiseSdBreakout(); // Depends on RTC for timestamping
  initialiseEthernetShield(ethConfigLocal);
  initialiseCanModules();
  reportTyreCircumference();
  connectMqttClientToBroker();
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

  // Handle telemetry data received from remote Arduino
  if (ptHandleTelemetryReceivedFromSlave.call()) {
    handleTelemetryReceivedFromSlave();
  }

  // Read the electronics temperature from the RTC sensor
  if (ptGetElectronicsTemperature.call()) {
    currentElectronicsRtcTemp = getRtcCurrentTemperature();
  }

  // Update the status of the check light
  if (ptUpdateCheckLightStatus.call()) {
    updateCheckLightStatus();
  }

  // Send required CAN messages to the BMW network
  if (ptSendCanMessages.call()) {
    sendCanMessages();
  }

  // Update the current gear based on the vehicle speed and engine RPM
  if (ptUpdateCurrentGear.call()) {
    updateCurrentGear();
  }

  // Update the alarm states based on a range of error conditions and take necessary actions
  if (ptUpdateAlarmStatesMaster.call()) {
    handleAllAlarmStatesMaster();
  }

  // Handle scheduled tasks related to SD card operations
  if (globalHealthSdCardLogging && ptHandleSdCardTasks.call()) {
    handleSdCardScheduledTasks();
  }

  // Update the boost target Kpa based on various conditions
  if (ptUpdateBoostTargetKpa.call()) {
    updateBoostTargetGaugeKpa();
  }
}
