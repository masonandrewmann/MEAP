/*
 Demonstrates filtering of white noise

 Pot 0 controls cutoff
 Pot 1 controls resonance
 */


#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/whitenoise8192_int8.h>  // loads white noise wavetable

mOscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> white_noise(WHITENOISE8192_DATA);

MultiResonantFilter filter;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  white_noise.setFreq(1);
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
  int cutoff = map(meap.pot_vals[0], 0, 4095, 0, 255);
  int resonance = map(meap.pot_vals[1], 0, 4095, 0, 255);
  filter.setCutoffFreqAndResonance(cutoff, resonance);
}


AudioOutput_t updateAudio() {
  int64_t out_sample = white_noise.next();
  filter.next(out_sample);
  out_sample = filter.low();  // lowpassed sample
  return StereoOutput::fromNBit(11, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}


/**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
void updateDip(int number, bool up) {
  if (up) {  // Any DIP toggled up

  } else {  //Any DIP toggled down
  }
  switch (number) {
    case 0:
      if (up) {  // DIP 0 up
      } else {   // DIP 0 down
      }
      break;
    case 1:
      if (up) {  // DIP 1 up
      } else {   // DIP 1 down
      }
      break;
    case 2:
      if (up) {  // DIP 2 up
      } else {   // DIP 2 down
      }
      break;
    case 3:
      if (up) {  // DIP 3 up
      } else {   // DIP 3 down
      }
      break;
    case 4:
      if (up) {  // DIP 4 up
      } else {   // DIP 4 down
      }
      break;
    case 5:
      if (up) {  // DIP 5 up
      } else {   // DIP 5 down
      }
      break;
    case 6:
      if (up) {  // DIP 6 up
      } else {   // DIP 6 down
      }
      break;
    case 7:
      if (up) {  // DIP 7 up
      } else {   // DIP 7 down
      }
      break;
  }
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
      if (pressed) {  // Pad 0 pressed
      } else {        // Pad 0 released
      }
      break;
    case 1:
      if (pressed) {  // Pad 1 pressed
      } else {        // Pad 1 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 2 pressed
      } else {        // Pad 2 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 3 pressed
      } else {        // Pad 3 released
      }
      break;
    case 4:
      if (pressed) {  // Pad 4 pressed
      } else {        // Pad 4 released
      }
      break;
    case 5:
      if (pressed) {  // Pad 5 pressed
      } else {        // Pad 5 released
      }
      break;
    case 6:
      if (pressed) {  // Pad 6 pressed
      } else {        // Pad 6 released
      }
      break;
    case 7:
      if (pressed) {  // Pad 7 pressed
      } else {        // Pad 7 released
      }
      break;
  }
}