
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include "MIDIUSB.h"

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();

short PC = 0;

void setup() {

  // Initialize Serial port
  Serial.begin(115200);
  
  delay(5000);  
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

void programChange(byte channel, byte value) {
  midiEventPacket_t event = {0x0C, 0xC0 | channel, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void loop() {
  int no_send = 0;
  if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        PC += 1;
        break;
      case DIR_DOWN:
        PC -= 1;
        break;
      case DIR_LEFT:
        PC += 16;
        break;
      case DIR_RIGHT:
        PC -= 16;
        break;
      case DIR_NEAR:
        PC = -1;
        break;
      case DIR_FAR:
        PC = 128;
        break;
      default:
        no_send = 1;
    }

    if ( PC < 0 ) {
      PC = 0;
    }else if ( PC > 127 ) {
      PC = 127;
    }

    if( !no_send ) {
      programChange(0, PC);
    }
  }

  delay(250);  
}

