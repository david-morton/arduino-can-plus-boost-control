#pragma once

#include <Arduino.h>

/* ======================================================================
   PIN CONSTANTS - ARDUINO DIGITAL
   ====================================================================== */

constexpr uint8_t SD_CS_PIN       = 7; // Chip select pin for the SD card breakout board
constexpr uint8_t SD_CD_PIN       = 8; // Card detect pin for the SD card breakout board
constexpr uint8_t PIN_ETHERNET_CS = 10;

/* ======================================================================
   PIN CONSTANTS - ARDUINO ANALOGUE
   ====================================================================== */

constexpr uint8_t CAN_SPI_CS_PIN_BMW    = 3; // Slave select pin for CAN module 1 (BMW CAN bus)
constexpr uint8_t CAN_SPI_CS_PIN_NISSAN = 5; // Slave select pin for CAN module 2 (Nissan CAN bus)
