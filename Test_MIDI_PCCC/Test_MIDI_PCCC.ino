//This project is to demo Arduino MIDI capability.
//Use Serial-MIDI to send data.

#define MIDI_CHANNEL 1
#define MIDI_CC 0x0E

void setup() {
  Serial.begin(31250);
}

byte i = 0;

void loop() {
  
  i = i+1;
  if ( i> 127 ) {
    i = 0;
  }
  
  controlChange(MIDI_CHANNEL, MIDI_CC, i);
  programChange(MIDI_CHANNEL, i);
  delay(1000);  
}

void controlChange(byte channel, byte control, byte value)
{
  // 0xB0 is the first of 16 control change channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0xB0 - 1;
  
  if (channel >= 0xB0 && channel <= 0xBF) {
      Serial.write(channel);
      Serial.write(control);
      Serial.write(value);
  }
}

void programChange(byte channel, byte value)
{
  // 0xC0 is the first of 16 program change channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0xC0 - 1;
  
  // Ensure we're between channels 1 and 16 for a CC message
  if (channel >= 0xC0 && channel <= 0xCF) {
      Serial.write(channel);
      Serial.write(value);
  }
}
