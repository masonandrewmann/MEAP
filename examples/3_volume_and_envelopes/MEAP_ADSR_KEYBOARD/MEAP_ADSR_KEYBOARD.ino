/*
  Basic oscillator pitch control with touch keyboard

  Press touch pads to play notes in C major scale.

  Potentiometers control attack and release time of envelopes

  Mason Mann, CC0
 */
#include <Meap.h>                 // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/cos2048_int8.h>  // loads sine wavetable

#define CONTROL_RATE 128   // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


mOscil<COS2048_NUM_CELLS, AUDIO_RATE> sines[8];
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
    envelopes[i].setTimes(1, 100, 4294967295, 500);  // attack=1ms, decay=100ms, sustain=maximum value of unsigned 32bit int (notes can
                                                   //sustain for an arbitrary amount of time), release=500ms
    envelopes[i].setADLevels(255, 127); // attack will rise to maximum volume, note will sustain at half volume
  }
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------

  int attack_time = map(meap.pot_vals[0], 0, 4095, 10, 1000); // attack time between 10ms and 1000ms
  int release_time = map(meap.pot_vals[1], 0, 4095, 10, 1000); // release time between 10ms and 1000ms

  for (int i = 0; i < 8; i++) {
    envelopes[i].setAttackTime(attack_time);
    envelopes[i].setReleaseTime(release_time);
  }
}


AudioOutput_t updateAudio() {
  int64_t out_sample = 0;

  for (int i = 0; i < 8; i++) {
    envelopes[i].update();
    gains[i] = envelopes[i].next();
    out_sample += sines[i].next() * gains[i];
  }
  return StereoOutput::fromNBit(19, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

/**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
void updateDip(int number, bool up) {
  if (up) {  // Any DIP toggld up

  } else {  //Any DIP toggled down
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

/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicated pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
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
