/*
  Basic AM Patch
  
  Plays a constant sine wave at 440Hz and modulates its amplitude using another sine wave.
  Pot 1 controls the modulation oscillator's frequency.
  Pot 2 controls modulation depth.

  Mason Mann, CC0
 */

#include <Meap.h>                           // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/triangle_warm8192_int8.h>  // includes triangle wavetable

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> carrier_osc(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> modulator_osc(TRIANGLE_WARM8192_DATA);

int mod_depth = 0;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object
  carrier_osc.setFreq(440);                  // set frequency of carrier oscillator
  modulator_osc.setFreq(440);                // set frequency of modulator oscillator
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.potVals[0] and meap.potVals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  carrier_osc.setFreq((float)map(meap.pot_vals[0], 0, 4095, 1, 1000));
  mod_depth = map(meap.pot_vals[1], 0, 4095, 0, 255);
}


AudioOutput_t updateAudio() {
  int carrier_sample = carrier_osc.next() * mod_depth;
  int modulator_sample = modulator_osc.next();
  int out_sample = carrier_sample * modulator_sample;
  return StereoOutput::fromAlmostNBit(24, out_sample, out_sample);
}

/** User defined function called whenever a touch pad is pressed or released
  @param number is the number of the pad that was pressed or released: 0-7
  @param pressed is true if the pad was pressed and false if the pad was released
	*/
void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released
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
      if (up) {  // DIP 2 up
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