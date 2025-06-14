#pragma once

#include <mcp2515_can.h>

/* ======================================================================
   STRUCTS
   ====================================================================== */

// Struct to hold the values read from the BMW CAN bus
typedef struct {
  float         vehicleSpeedFront;
  float         vehicleSpeedRear;
  unsigned long timestamp;
} bmwCanValues;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void         checkAndProcessCanMessages();
bmwCanValues readBmwDataFromCan(mcp2515_can &can);
