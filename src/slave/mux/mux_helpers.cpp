#include <Arduino.h>
#include <light_CD74HC4067.h>

#include "../../shared/debug_logging.h"
#include "../../shared/read_sensors.h"
#include "../pin_assignments_slave.h"
#include "mux_helpers.h"

/* ======================================================================
   VARIABLES: Pin constants
   ====================================================================== */

const int  muxS0Pin     = ARDUINO_MUX_S0;
const int  muxS1Pin     = ARDUINO_MUX_S1;
const int  muxS2Pin     = ARDUINO_MUX_S2;
const int  muxS3Pin     = ARDUINO_MUX_S3;
const byte muxSignalPin = ARDUINO_PIN_MUX_SIGNAL;

/* ======================================================================
   OBJECT DECLARATIOS
   ====================================================================== */

CD74HC4067 mux(muxS0Pin, muxS1Pin, muxS2Pin, muxS3Pin);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Setup the multiplexer board
void setupMux() {
  DEBUG_GENERAL("Configuring mux board ...");
  pinMode(muxSignalPin, INPUT);
}

// Read an analogue pin through the multiplexer and average the readings
int readAveragedMuxAnalogueChannel(byte channel, int samples, int delayUs) {
  mux.channel(channel);
  int averageReading = readAveragedAnaloguePinReading(muxSignalPin, samples, delayUs);
  return averageReading;
}

// Read a digital pin through the multiplexer and average over multiple samples
bool readStableMuxDigitalChannelReading(byte channel, int samples, int delayUs) {
  mux.channel(channel);

  // Fast-path for low sample counts: single-shot read via analog threshold
  if (samples <= 2) {
    delayMicroseconds(delayUs);
    int  analogValue = analogRead(muxSignalPin);
    bool result      = analogValue > DIGITAL_VOLTAGE_THRESHOLD;
    DEBUG_MUX("MUX channel %d: FastRead Samples=%d Analog=%d Value=%s", channel, samples, analogValue, result ? "HIGH" : "LOW");
    return result;
  }

  // Averaged digital logic using analog threshold
  const int threshold = DIGITAL_VOLTAGE_THRESHOLD;
  int       highCount = 0;
  for (int i = 0; i < samples; i++) {
    int analogValue = analogRead(muxSignalPin);
    if (analogValue > threshold) {
      highCount++;
    }
    delayMicroseconds(delayUs);
  }

  bool result = (highCount > samples / 2);
  DEBUG_MUX("MUX channel %d: Samples=%d HIGH=%d Result=%s", channel, samples, highCount, result ? "HIGH" : "LOW");

  return result;
}
