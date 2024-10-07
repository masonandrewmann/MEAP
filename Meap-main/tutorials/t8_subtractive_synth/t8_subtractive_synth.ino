/*
 Demonstrates basic subtractive synthesis patch

 Pot 0 controls filter attack time
 Pot 1 controls resonance

 DIP 0 toggles octave
 DIP 1 toggles waveform: down = saw, up = square
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sq8192_int16.h>  // loads square wave
#include <tables/saw8192_int16.h>           // loads square wave

mOscil<saw8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc(saw8192_int16_DATA);

MultiResonantFilter filter;

ADSR<CONTROL_RATE, AUDIO_RATE> amplitude_envelope;

ADSR<CONTROL_RATE, CONTROL_RATE> filter_envelope;

int notes[8] = { 48, 50, 52, 53, 55, 57, 59, 60 }; // choose MIDI pitches of keyboard notes
int transpose = 0; // num of semitones to transpose, used for octave switching

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  osc.setFreq(220);

  amplitude_envelope.setADLevels(255, 200); // set attack level to maximum, and sustain level slightly lower
  amplitude_envelope.setTimes(1, 100, 1000000, 1000);

  filter_envelope.setADLevels(255, 127); // set attack level to maximum, and sustain level to half
  filter_envelope.setTimes(1, 400, 1000000, 500);
}


void loop() {
  audioHook();
}


void updateControl() {
    meap.readInputs();
    // ---------- YOUR updateControl CODE BELOW ----------
  
    amplitude_envelope.update();                                                                       // update filter envelope
    filter_envelope.update();                                                                          // update filter envelopeenvelope
    filter_envelope.setTimes(map(meap.pot_vals[0], 0, 4095, 1, 2000), 400, 1000000, 500);              // use pot 0 to control filter envelope attack time
    filter.setCutoffFreqAndResonance(filter_envelope.next(), map(meap.pot_vals[1], 0, 4095, 0, 255));  // use filter envelope to control cutoff, and pot 1 to control resonance
  }


AudioOutput_t updateAudio() {
  int64_t out_sample = osc.next();                             // grab sample from oscillator
  filter.next(out_sample);                                     // send oscillator
  out_sample = filter.low();                                   // grab lowpass output from filter
  out_sample = (out_sample * amplitude_envelope.next()) >> 8;  // apply amplitude envelope
  return StereoOutput::fromNBit(19, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
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
        transpose = 12;
      } else {  // DIP 0 down
        transpose = 0;
      }
      break;
    case 1:
      if (up) {  // DIP 1 up
        osc.setTable(sq8192_int16_DATA); // set oscillator's table to a square wave
      } else {  // DIP 1 down
        osc.setTable(saw8192_int16_DATA); // set oscillator's table to a saw wave
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
    amplitude_envelope.noteOn();
    filter_envelope.noteOn();
    osc.setFreq(mtof(notes[number] + transpose));
  } else {  // Any pad released
    amplitude_envelope.noteOff();
    filter_envelope.noteOff();
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