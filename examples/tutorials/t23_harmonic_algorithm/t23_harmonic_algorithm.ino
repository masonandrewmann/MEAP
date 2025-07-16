/*
  Implements a basic version of Laurie Spiegel's Harmonic Algorithm, changing chords every second.
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------

// Synthesis
#include <tables/triangle_warm8192_int8.h>
mOscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc1(TRIANGLE_WARM8192_DATA);
mOscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc2(TRIANGLE_WARM8192_DATA);
mOscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc3(TRIANGLE_WARM8192_DATA);
mOscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc4(TRIANGLE_WARM8192_DATA);

ADSR<AUDIO_RATE, AUDIO_RATE> env1;
ADSR<AUDIO_RATE, AUDIO_RATE> env2;
ADSR<AUDIO_RATE, AUDIO_RATE> env3;
ADSR<AUDIO_RATE, AUDIO_RATE> env4;

// templates
int major_scale[] = { 0, 2, 4, 5, 7, 9, 11 };  // template for building major scale on top of 12TET
int scale_root = 60;                           // root of major scale

int triad[] = { 0, 2, 4 };  // template for triad on top of major scale
int chord_root = 2;         // root of triad

// chord timer
EventDelay chord_timer;
int chord_length = 1000;

int tree_level = 4;  // what level of chord tree are we on

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  // ---------- YOUR SETUP CODE BELOW ----------

  //set oscillators to pitches of first chord (iii chord on c major scale)
  osc1.setFreq(mtof(scale_root + major_scale[(triad[0] + chord_root) % 7]));
  osc2.setFreq(mtof(scale_root + major_scale[(triad[1] + chord_root) % 7]));
  osc3.setFreq(mtof(scale_root + major_scale[(triad[2] + chord_root) % 7]));
  osc4.setFreq(mtof(12 + scale_root + major_scale[(triad[0] + chord_root) % 7]));

  // sets envelope attack and sustain levels
  env1.setADLevels(255, 0);
  env2.setADLevels(255, 0);
  env3.setADLevels(255, 0);
  env4.setADLevels(255, 0);

  // sets envelope times
  env1.setTimes(1, 500, 1, 1);
  env2.setTimes(1, 500, 1, 1);
  env3.setTimes(1, 500, 1, 1);
  env4.setTimes(1, 500, 1, 1);
}


void loop() {
  audioHook();  // handles Mozzi audio generation behind the scenes
}


/** Called automatically at rate specified by CONTROL_RATE macro, most of your mode should live in here
	*/
void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
  if (chord_timer.ready()) {

    // move to next tree level
    if (tree_level != 0) {  // move one level down tree
      tree_level = tree_level - 1;
    } else {  // if at bottom of tree (0), jump to a random level
      tree_level = meap.irand(0, 4);
    }

    // choose chord depending on current level
    switch (tree_level) {
      case 4:            // leaves: iii
        chord_root = 2;  // iii chord (E minor)
        break;
      case 3:                           // twigs: I or vi
        if (meap.irand(1, 100) > 25) {  // 25% chance of I, 75% chance of vi
          chord_root = 5;               // vi chord (A minor)
        } else {
          chord_root = 0;  // I chord (C major)
        }
        break;
      case 2:                           // branches: IV or ii
        if (meap.irand(1, 100) > 33) {  // 66% change of IV, 33% chance of ii
          chord_root = 3;               // IV chord (F major)
        } else {

          chord_root = 1;  // ii chord (D minor)
        }
        break;
      case 1:// boughs: V or vii˚
        if (meap.irand(1, 100) > 25) {  // 75% change of V, 25% chance of vii˚
          chord_root = 4;               // V chord (G major)
        } else {
          chord_root = 6;  // vii˚ chord (B diminished)
        }
        break;
      case 0:                 // trunk: I or vi
        if (meap.irand(1, 100) > 25) { // 75% change of I, 25% chance of vi
          chord_root = 0;  // I chord (C major)
        } else {
          chord_root = 5;  // vi chord (A minor)
        }
        break;
    }

    // set oscillator frequencies
    osc1.setFreq(mtof(scale_root + major_scale[(triad[0] + chord_root) % 7]));      // root of chord (modded to octave)
    osc2.setFreq(mtof(scale_root + major_scale[(triad[1] + chord_root) % 7]));      // third of chord (modded to octave)
    osc3.setFreq(mtof(scale_root + major_scale[(triad[2] + chord_root) % 7]));      // fifth of chord (modded to octave)
    osc4.setFreq(2 * mtof(scale_root + major_scale[(triad[0] + chord_root) % 7]));  // octave above root

    // trigger all notes
    env1.noteOn();
    env2.noteOn();
    env3.noteOn();
    env4.noteOn();

    // start timer
    chord_timer.start(chord_length);
  }
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
	*/
AudioOutput_t updateAudio() {
  env1.update();
  env2.update();
  env3.update();
  env4.update();

  int64_t out_sample = osc1.next() * env1.next() + osc2.next() * env2.next() + osc3.next() * env3.next() + osc4.next() * env4.next();

  return StereoOutput::fromNBit(18, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicates pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released
  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 0 pressed
        Serial.println("t0 pressed ");
      } else {  // Pad 0 released
        Serial.println("t0 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed");
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
      } else {  // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
      } else {  // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
      } else {  // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
      } else {  // Pad 7 released
        Serial.println("t7 released");
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
      if (up) {  // DIP 0 up
        Serial.println("d0 up");
      } else {  // DIP 0 down
        Serial.println("d0 down");
      }
      break;
    case 1:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 2:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 3:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 4:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 5:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 6:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 7:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
  }
}