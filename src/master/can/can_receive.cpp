#include "can_receive.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "can_helpers.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check flags and process messages accordingly
void checkAndProcessCanMessages() {
  bmwCanValues    bmwResults;
  nissanCanValues nissanResults;

  if (readBmwDataFromCan(CAN_BMW, bmwResults)) {
    currentVehicleSpeedFrontKph = bmwResults.vehicleSpeedFront;
    currentVehicleSpeedRearKph  = bmwResults.vehicleSpeedRear;
    DEBUG_CAN_BMW("Received BMW CAN message: Front Speed: %.2f kph, Rear Speed: %.2f kph", currentVehicleSpeedFrontKph, currentVehicleSpeedRearKph);
    updateReceiveCanMessageCountBmw();
  }

  if (readNissanDataFromCan(CAN_NISSAN, nissanResults)) {
    currentEngineTempCelcius = nissanResults.engineTempCelsius;
    DEBUG_CAN_BMW("Received Nissan CAN message: Engine temperature: %d°C", currentEngineTempCelcius);
    updateReceiveCanMessageCountNissan();
  }
}

// Read data from the BMW CAN bus and calculate vehicle speed values
bool readBmwDataFromCan(mcp2515_can &can, bmwCanValues &bmwCanData) {
  static const float speedScaleFactor = 1.06; // Scale factor to correct wheel speed to real world speed values

  int wheelSpeedFl = 0;
  int wheelSpeedFr = 0;
  int wheelSpeedRl = 0;
  int wheelSpeedRr = 0;

  unsigned char len = 0;
  unsigned char buf[8];

  while (CAN_MSGAVAIL == can.checkReceive()) {
    can.readMsgBuf(&len, buf);
    unsigned long canId = can.getCanId();

    // Get the current vehicle wheel speeds from data sent by ABS module on ID 0x1F0
    if (canId == 0x1F0) {
      wheelSpeedFl = ((buf[0] + (buf[1] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedFr = ((buf[2] + (buf[3] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedRl = ((buf[4] + (buf[5] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedRr = ((buf[6] + (buf[7] & 15) * 256) / 16.0) * speedScaleFactor;

      bmwCanData.vehicleSpeedFront = (wheelSpeedFl + wheelSpeedFr) / 2;
      bmwCanData.vehicleSpeedRear  = (wheelSpeedRl + wheelSpeedRr) / 2;
      bmwCanData.timestamp         = millis();

      return true;
    }
  }

  return false; // No matching 0x1F0 message found
}

// Read data from the Nissan CAN bus and calculate engine temperature
bool readNissanDataFromCan(mcp2515_can &can, nissanCanValues &nissanCanData) {
  unsigned char len = 0;
  unsigned char buf[8];

  while (CAN_MSGAVAIL == can.checkReceive()) {
    can.readMsgBuf(&len, buf);
    unsigned long canId = can.getCanId();

    // Get the current engine temperature from data sent by Nissan on ID 0x551
    if (canId == 0x551) {
      nissanCanData.engineTempCelsius = (buf[0] - 40); // Convert to Celsius
      return true;
    }
  }

  return false; // No matching 0x551 message found
}
