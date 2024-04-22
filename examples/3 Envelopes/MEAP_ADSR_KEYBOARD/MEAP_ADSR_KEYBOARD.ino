/*
  Basic oscillator pitch control with touch keyboard

  Press touch pads to control pitch of oscillator.

  Mason Mann, CC0
 */
#include <Meap.h>                 // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/cos2048_int8.h>  // loads sine wavetable

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable


Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


Oscil<COS2048_NUM_CELLS, AUDIO_RATE> sines[8];
ADSR<AUDIO_RATE, AUDIO_RATE> envelopes[8];
int gains[8] = { 0 };

int midi_notes[8] = { 60, 62, 64, 65, 67, 69, 71, 72 };

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  for (int i = 0; i < 8; i++) {
    sines[i].setTable(COS2048_DATA);
    sines[i].setFreq(mtof(midi_notes[i]));
    envelopes[i].setAttackTime(10);
    envelopes[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    envelopes[i].setReleaseTime(500);
    envelopes[i].setADLevels(255, 255);
  }
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  int attack_time = map(meap.pot_vals[1], 0, 4095, 10, 1000);
  int release_time = map(meap.pot_vals[0], 0, 4095, 10, 1000);

  for (int i = 0; i < 8; i++) {
    envelopes[i].setAttackTime(attack_time);
    envelopes[i].setReleaseTime(release_time);
  }
}


AudioOutput_t updateAudio() {
  int sample = 0;

  for (int i = 0; i < 8; i++) {
    envelopes[i].update();
    gains[i] = envelopes[i].next();
    sample += sines[i].next() * gains[i];
  }
  return StereoOutput::fromAlmostNBit(19, sample, sample);
}

/** User defined function called whenever a DIP switch is toggled up or down
  @param number is the number of the switch that was toggled up or down: 0-7
  @param up is true if the switch was toggled up, and false if the switch was toggled down
	*/
void Meap::updateDip(int number, bool up) {
  if (up) {  // Any DIP up

  } else {  //Any DIP down
  }
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
      } else {   // DIP 1 down
      }
      break;
    case 1:
      if (up) {  // DIP 2 up,
      } else {   // DIP 2 down
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
      } else {   // DIP 3 down
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
      } else {   // DIP 4 down
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
      } else {   // DIP 5 down
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
      } else {   // DIP 6 down
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
      } else {   // DIP 7 down
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
      } else {   // DIP 8 down
      }
      break;
  }
}

/** User defined function called whenever a touch pad is pressed or released
  @param number is the number of the pad that was pressed or released: 0-7
  @param pressed is true if the pad was pressed and false if the pad was released
	*/
void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed
    envelopes[number].noteOn();
  } else {  // Any pad released
    envelopes[number].noteOff();
  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
      } else {        // Pad 1 released
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
      } else {        // Pad 2 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
      } else {        // Pad 3 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
      } else {        // Pad 4 released
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
      } else {        // Pad 5 released
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
      } else {        // Pad 6 released
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
      } else {        // Pad 7 released
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
      } else {        // Pad 8 released
      }
      break;
  }
}
