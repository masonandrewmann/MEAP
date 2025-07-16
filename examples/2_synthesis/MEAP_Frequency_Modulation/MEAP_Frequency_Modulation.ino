/*
  Basic FM Patch
  
  Plays a constant sine wave at 440Hz modulates its frequency using another sine wave.
  Pot 1 controls the modulation oscillator's frequency.
  Pot 2 controls modulation depth.

  Mason Mann, CC0
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int8.h>  // loads sine wavetable

mOscil<SIN8192_NUM_CELLS, AUDIO_RATE> carrier_osc(SIN8192_DATA);
mOscil<SIN8192_NUM_CELLS, AUDIO_RATE> modulator_osc(SIN8192_DATA);

Q16n16 modDepth = 0;


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object
  // ---------- YOUR SETUP CODE BELOW ----------

  carrier_osc.setFreq(440);    //set frequency of carrier oscillator
  modulator_osc.setFreq(440);  //set frequency of modulation oscillator
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
  modulator_osc.setFreq((float)map(meap.pot_vals[0], 0, 4095, 1, 1000));  // set mod oscillator range to 1->1000Hz
  modDepth = Q7n0_to_Q7n8(map(meap.pot_vals[1], 0, 4095, 0, 127));        // convert mod depth to fixed point signed int between 0 and 127
}


AudioOutput_t updateAudio() {
  Q15n16 modulation = modDepth * modulator_osc.next() >> 8;  // calculate mod depth in 15n16 signed int
  int64_t out_sample = carrier_osc.phMod(modulation * 4);
  return StereoOutput::fromNBit(8, (out_sample * meap.volume_val)>>12, (out_sample * meap.volume_val)>>12);  // scale by factor of 4 and apply phase mod to carrier oscillator
}


/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicated pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
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

/**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
void updateDip(int number, bool up) {
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
