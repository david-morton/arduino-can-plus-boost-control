#include <Arduino.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../alarm/alarms_master.h"
#include "../check_light/check_light.h"
#include "../rpm/rpm.h"
#include "can_helpers.h"
#include "can_send.h"

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// High frequency tasks for BMW network
ptScheduler ptSendEngineTempToBmwCan = ptScheduler(PT_TIME_10MS);
ptScheduler ptSendEngineRpmToBmwCan  = ptScheduler(PT_TIME_10MS);
ptScheduler ptSendMiscToBmwCan       = ptScheduler(PT_TIME_10MS);

// High frequency tasks for Nissan network
ptScheduler ptSendVehicleSpeedToNissanCan = ptScheduler(PT_TIME_20MS);

/* ======================================================================
   FUNCTION DEFINITION: Send CAN messages based on their own schedules
   ====================================================================== */

// Send CAN messages based on their own schedules
void sendCanMessages() {

  if (ptSendEngineTempToBmwCan.call()) {
    sendEngineTempToBmwCan();
  }

  if (ptSendEngineRpmToBmwCan.call()) {
    sendEngineRpmToBmwCan();
  }

  if (ptSendMiscToBmwCan.call()) {
    sendMiscToBmwCan();
  }

  if (ptSendVehicleSpeedToNissanCan.call()) {
    sendVehicleSpeedToNissanCan();
  }
}

/* ======================================================================
   FUNCTIONS DEFINITIONS: Send specific CAN messages to BMW and Nissan networks
   ====================================================================== */

// Send current engine temperature to the BMW network for cluster display
void sendEngineTempToBmwCan() {
  unsigned char canPayloadTemp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  canPayloadTemp[1]               = (currentEngineTempCelcius + 48.373) / 0.75;
  CAN_BMW.sendMsgBuf(0x329, 0, 8, canPayloadTemp);
}

// Send current engine RPM to the BMW network for cluster display
void sendEngineRpmToBmwCan() {
  unsigned char canPayloadRpm[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  static int    previousEngineSpeedRpm; // Will store the previous RPM value
  int           multipliedRpm;          // The RPM value to represent in CAN payload which the cluster is expecting

  float rpmHexConversionMultipler = calculateRpmMultiplier();

  if (currentEngineSpeedRpm != 0 && abs(currentEngineSpeedRpm - previousEngineSpeedRpm) < 750) {
    multipliedRpm    = currentEngineSpeedRpm * rpmHexConversionMultipler;
    canPayloadRpm[2] = multipliedRpm;        // LSB
    canPayloadRpm[3] = (multipliedRpm >> 8); // MSB

    CAN_BMW.sendMsgBuf(0x316, 0, 8, canPayloadRpm);
  }
  previousEngineSpeedRpm = currentEngineSpeedRpm;
}

// Send miscellaneous data to the BMW network
void sendMiscToBmwCan() {
  unsigned char canPayloadMisc[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Check light and temp alarm light. Fuel consumption not used
  canPayloadMisc[0]               = currentCheckEngineLightState;
  if (currentEngineTempCelcius >= alarmThresholdsCritical.engineTemperature) // Set the red alarm light on the temp gauge if needed
    canPayloadMisc[3] = 8;
  else
    canPayloadMisc[3] = 0;

  CAN_BMW.sendMsgBuf(0x545, 0, 8, canPayloadMisc);
}

// Send vehicle speed to the Nissan network
void sendVehicleSpeedToNissanCan() {
  unsigned char canPayloadSpeed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float         speedValue         = (0.3903 * currentVehicleSpeedRearKph) + 0.5144;
  canPayloadSpeed[4]               = speedValue;
  CAN_NISSAN.sendMsgBuf(0x280, 0, 8, canPayloadSpeed); // ID 0x280 used for 2011 USDM Nissan 370Z ECU, 0x284 used for 2009 JDM Nissan Skyline 370GT ECU
}
