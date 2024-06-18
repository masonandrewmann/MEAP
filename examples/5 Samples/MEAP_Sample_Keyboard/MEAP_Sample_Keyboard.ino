/*
  Example for basic monophonic sine wave synthesizer played using
  capacitive touch pads. Plays a C major scale starting on C4.

  Mason Mann, CC0
 */

#include <Meap.h>  // MEAP library, includes all dependent libraries, including all Mozzi modules
#include "harp_c3.h"

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


// variables for sample
Sample<harp_c3_NUM_CELLS, AUDIO_RATE> my_sample(harp_c3_DATA);
float sample_freq;


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  sample_freq = (float)harp_c3_SAMPLERATE / (float)harp_c3_NUM_CELLS;
  my_sample.setFreq(sample_freq);  // play at the speed it was recorded
  my_sample.rangeWholeSample();
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches
}


AudioOutput_t updateAudio() {
  int sample = my_sample.next();
  return StereoOutput::fromAlmostNBit(8, sample, sample);
}


void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // pad 0 pressed
        my_sample.setFreq((float)sample_freq);
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
