#pragma once

#include <Arduino.h>

/* ======================================================================
   PIN CONSTANTS - ARDUINO DIGITAL
   ====================================================================== */

const int CAN_INTERRUPT_PIN_BMW    = 2;
const int CAN_INTERRUPT_PIN_NISSAN = 4;

/* ======================================================================
   PIN CONSTANTS - ARDUINO ANALOGUE
   ====================================================================== */

const int CAN_SPI_CS_PIN_BMW    = A3; // Slave select pin for CAN module 1 (BMW CAN bus)
const int CAN_SPI_CS_PIN_NISSAN = A5; // Slave select pin for CAN module 2 (Nissan CAN bus)
