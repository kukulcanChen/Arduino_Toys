#include "hidjoystickrptparser.h"
#include "MIDIUSB.h"

short PC_channel = 0;
short PC = 0;


short CC_channel = 0;
short CC_element = 0;
short CC = 0;


void programChange(byte channel, byte value) {
  midiEventPacket_t event = {0x0C, 0xC0 | channel, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void controlChange(byte channel, byte number, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, number, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
joyEvents(evt),
oldHat(0xDE),
oldButtons(0) {
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
    oldPad[i] = 0xD;
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  bool match = true;

  // Checking if there are changes in report since the method was last called
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
    if (buf[i] != oldPad[i]) {
      match = false;
      break;
    }

  // Calling Game Pad event handler
  if (!match && joyEvents) {
    joyEvents->OnGamePadChanged((const GamePadEventData*)buf);

    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
  }

  uint8_t hat = (buf[5] & 0xF);

  // Calling Hat Switch event handler
  if (hat != oldHat && joyEvents) {
    joyEvents->OnHatSwitch(hat);
    oldHat = hat;
  }

  uint16_t buttons = (0x0000 | buf[6]);
  buttons <<= 4;
  buttons |= (buf[5] >> 4);
  uint16_t changes = (buttons ^ oldButtons);

  // Calling Button Event Handler for every button changed
  if (changes) {
    for (uint8_t i = 0; i < 0x0C; i++) {
      uint16_t mask = (0x0001 << i);

      if (((mask & changes) > 0) && joyEvents) {
        if ((buttons & mask) > 0)
          joyEvents->OnButtonDn(i + 1);
        else
          joyEvents->OnButtonUp(i + 1);
      }
    }
    oldButtons = buttons;
  }
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {

  if (evt->X == 0) {
    CC = CC - 1;
    if ( CC < 0 ) CC = 0;
    controlChange(CC_channel, CC_element, CC);
  }
  else if (evt->X == 0xFF) {
    CC = CC + 1;
    if ( CC > 127 ) CC = 127;
    controlChange(CC_channel, CC_element, CC);
  }
        
  Serial.print("X1: ");
  PrintHex<uint8_t > (evt->X, 0x80);
  Serial.print("\tY1: ");
  PrintHex<uint8_t > (evt->Y, 0x80);
  //Serial.print("\tX2: ");
  //PrintHex<uint8_t > (evt->Z1, 0x80);
  Serial.print("\tY2: ");
  PrintHex<uint8_t > (evt->Z2, 0x80);
  Serial.print("\tRz: ");
  PrintHex<uint8_t > (evt->Rz, 0x80);
  Serial.println("");
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
        Serial.print("Hat Switch: ");
        PrintHex<uint8_t > (hat, 0x80);
        Serial.println("");
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
  Serial.print("Up: ");
  Serial.println(but_id, DEC);
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
  Serial.print("Dn: ");
  Serial.println(but_id);
  if (but_id == 8) {
    PC = PC + 1;
    if (PC > 127) PC = 127;
    programChange(PC_channel, PC);
  }
  else if (but_id == 7) {
    PC = PC - 1;
    if (PC < 0) PC = 0;
    programChange(PC_channel, PC);
  }
  else if (but_id == 6) {
    PC_channel = PC_channel + 1;
    if (PC_channel > 127) PC_channel = 127;
  }
  else if (but_id == 5) {
    PC_channel = PC_channel - 1;
    if (PC_channel < 0) PC_channel = 0;
  }
  else if (but_id == 9) {
    PC_channel = 0;
    PC = 0;
  }
  else if (but_id == 10) {
    CC_channel = 0;
    CC_element = 0;
    CC = 0;
  }
  else if (but_id == 1) {
    CC_channel = CC_channel - 1;
    if (CC_channel < 0 ) CC_channel = 0;
  }
  else if (but_id == 2) {
    CC_channel = CC_channel + 1;
    if (CC_channel > 127 ) CC_channel = 127;
  }
  else if (but_id == 4) {
    CC_element = CC_element - 1;
    if ( CC_element < 0 ) CC_element = 0;
  }
  else if (but_id == 3) {
    CC_element = CC_element + 1;
    if ( CC_element > 127 ) CC_element = 127;
  }
}
