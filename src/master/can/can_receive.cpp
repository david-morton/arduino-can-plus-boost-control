#include "can_receive.h"
#include "../../shared/debug_logging.h"
#include "can_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

const float speedScaleFactor = 1.06;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check flags and process messages accordingly
void checkAndProcessCanMessages() {
  if (canBmwMsgRecv) {
    canBmwMsgRecv = false;
    readBmwDataFromCan(CAN_BMW);
  }
}

// Read data from the BMW CAN bus and calculate vehicle speed values
bmwCanValues readBmwDataFromCan(mcp2515_can &can) {
  bmwCanValues bmwCanData;

  float wheelSpeedFl = 0;
  float wheelSpeedFr = 0;
  float wheelSpeedRl = 0;
  float wheelSpeedRr = 0;

  unsigned char len = 0;
  unsigned char buf[8];

  while (CAN_MSGAVAIL == can.checkReceive()) {
    can.readMsgBuf(&len, buf);

    unsigned long canId = can.getCanId();

    // Get the current vehicle wheel speeds
    if (canId == 0x1F0) {
      wheelSpeedFl = ((buf[0] + (buf[1] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedFr = ((buf[2] + (buf[3] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedRl = ((buf[4] + (buf[5] & 15) * 256) / 16.0) * speedScaleFactor;
      wheelSpeedRr = ((buf[6] + (buf[7] & 15) * 256) / 16.0) * speedScaleFactor;
    }

    // Calculate the required values
    bmwCanData.vehicleSpeedFront = ((wheelSpeedFl + wheelSpeedFr) / 2);
    bmwCanData.vehicleSpeedRear  = ((wheelSpeedRl + wheelSpeedRr) / 2);
    bmwCanData.timestamp         = millis();
  }
  return bmwCanData;
}
