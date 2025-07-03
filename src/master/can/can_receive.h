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

// Struct to hold the values read from the Nissan CAN bus
typedef struct {
  int engineTempCelsius;
} nissanCanValues;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void checkAndProcessCanMessages();
bool readBmwDataFromCan(mcp2515_can &can, bmwCanValues &bmwCanData);
bool readNissanDataFromCan(mcp2515_can &can, nissanCanValues &nissanCanData);
