
#define MIDI_CHANNEL 1

//Pins from 1st and 2nd Sensor
#define TRIGPIN1 11
#define ECHOPIN1 12
#define TRIGPIN2 9
#define ECHOPIN2 10

void setup() {
  Serial.begin(115200);
  pinMode(TRIGPIN1, OUTPUT);
  pinMode(ECHOPIN1, INPUT);
  pinMode(TRIGPIN2, OUTPUT);
  pinMode(ECHOPIN2, INPUT);
}

byte i = 0;
int range_pre1 = 0;
int range_pre2 = 0;
int PC = 0;

void loop() {
  
  float duration1, duration2;
  int range1 = 0, range_diff1 = 0;
  int range2 = 0, range_diff2 = 0;
  int doSend = 0;

  //Use 1st Sensor to get human action
  digitalWrite(TRIGPIN1, HIGH);
  delayMicroseconds(1000);
  digitalWrite(TRIGPIN1, LOW);
  duration1 = pulseIn(ECHOPIN1, HIGH);
  range1 = (duration1/2) / 10;
  range_diff1 = range1 - range_pre1; 
  
  //Check if range change greater then 10 and the newest value low then 30
  if( abs(range_diff1) > 10 && range1 < 30) {
    //Check the change in near
    if(range_diff1 < -10) {
      PC ++;
      doSend = 1;
    }
    if ( PC > 127 ) {
      PC = 127;
    }    
  }
  range_pre1 = range1;

  if(doSend == 1) {
    programChange(1, PC);
    return;
  }

  //Use 2nd Sensor to get human action
  digitalWrite(TRIGPIN2, HIGH);
  delayMicroseconds(1000);
  digitalWrite(TRIGPIN2, LOW);
  duration2 = pulseIn(ECHOPIN2, HIGH);
  range2 = (duration2/2) / 10;

  range_diff2 = range2 - range_pre2; 
  if( abs(range_diff2) > 10 && range2 < 30) {
    if(range_diff2 < -10) {
      PC --;
      doSend = 1;
    }
    if(PC < 0) {
      PC = 0;
    }    
  }
  range_pre2 = range2;

  if(doSend == 1) 
    programChange(MIDI_CHANNEL, PC);
  
  //controlChange(MIDI_CHANNEL, MIDI_CC, i);
  //programChange(1, i);
  delay(100);  

}

// Send a MIDI control change message
void controlChange(byte channel, byte control, byte value)
{
  // 0xB0 is the first of 16 control change channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0xB0 - 1;
  
  // Ensure we're between channels 1 and 16 for a CC message
  if (channel >= 0xB0 && channel <= 0xBF) {
      Serial.write(channel);
      Serial.write(control);
      Serial.write(value);
  }
}

void programChange(byte channel, byte value)
{
  // 0xB0 is the first of 16 control change channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0xC0 - 1;
  
  // Ensure we're between channels 1 and 16 for a CC message
  if (channel >= 0xC0 && channel <= 0xCF) {
      Serial.write(channel);
      Serial.write(value);
  }
}
