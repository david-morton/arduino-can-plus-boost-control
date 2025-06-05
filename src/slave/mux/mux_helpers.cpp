#include "mux_helpers.h"
#include "../../shared/debug_logging.h"
#include <light_CD74HC4067.h>

/* ======================================================================
   VARIABLES: Pin constants
   ====================================================================== */
// Needed for multiplexer board setup
const int  muxS0Pin     = 4;
const int  muxS1Pin     = 5;
const int  muxS2Pin     = 6;
const int  muxS3Pin     = 7;
const byte muxSignalPin = A3;

/* ======================================================================
   OBJECT DECLARATIOS
   ====================================================================== */
CD74HC4067 mux(muxS0Pin, muxS1Pin, muxS2Pin, muxS3Pin);

/* ======================================================================
   FUNCTION: Setup the multiplexer
   ====================================================================== */
void setupMux() {
  DEBUG_GENERAL("Configuring mux board ...");
  pinMode(muxSignalPin, INPUT);
}

/* ======================================================================
   FUNCTION: Get average readings from multiplexed channel via CD74HC4067
   ====================================================================== */
int readAveragedMuxAnalogueChannelReading(byte channel, int samples, int delayUs) {
  mux.channel(channel);
  int averageReading = readAveragedAnaloguePinReading(muxSignalPin, samples, delayUs);
  return averageReading;
}
