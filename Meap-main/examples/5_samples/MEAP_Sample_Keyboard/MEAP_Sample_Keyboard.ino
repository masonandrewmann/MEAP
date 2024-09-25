/*
  Example for basic monophonic sine wave synthesizer played using
  capacitive touch pads. Plays a C major scale starting on C4.

  Mason Mann, CC0
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include "tables/Glass_Piano_C.h"

// variables for sample
mSample<Glass_Piano_C_NUM_CELLS, AUDIO_RATE, int16_t> my_sample(Glass_Piano_C_DATA);
float sample_freq;


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  // ---------- YOUR SETUP CODE BELOW ----------                          

  sample_freq = (float)Glass_Piano_C_SAMPLERATE / (float)Glass_Piano_C_NUM_CELLS;
  my_sample.setFreq(sample_freq);  // play at the speed it was recorded
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
}


AudioOutput_t updateAudio() {
  int out_sample = my_sample.next();
  return StereoOutput::fromNBit(16, (out_sample * meap.volume_val)>>12, (out_sample * meap.volume_val)>>12);
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
      if (pressed) {  // pad 0 pressed
        my_sample.setFreq(sample_freq);
        my_sample.start();
      } else {  // pad 0 released
      }
      break;
    case 1:
      if (pressed) {  // pad 1 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 2.0 / 12.0)));
        my_sample.start();
      } else {  // pad 1 released
      }
      break;
    case 2:
      if (pressed) {  // pad 2 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 4.0 / 12.0)));
        my_sample.start();
      } else {  // pad 2 released
      }
      break;
    case 3:
      if (pressed) {  // pad 3 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 5.0 / 12.0)));
        my_sample.start();
      } else {  // pad 3 released
      }
      break;
    case 4:
      if (pressed) {  // pad 4 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 7.0 / 12.0)));
        my_sample.start();
      } else {  // pad 4 released
      }
      break;
    case 5:
      if (pressed) {  // pad 5 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 9.0 / 12.0)));
        my_sample.start();
      } else {  // pad 5 released
      }
      break;
    case 6:
      if (pressed) {  // pad 6 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 11.0 / 12.0)));
        my_sample.start();
      } else {  // pad 6 released
      }
      break;
    case 7:
      if (pressed) {  // pad 7 pressed
        my_sample.setFreq((float)(sample_freq * pow(2, 12.0 / 12.0)));
        my_sample.start();
      } else {  // pad 7 released
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
  if (up) {  // Any DIP toggled up

  } else {  //Any DIP toggled down
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
