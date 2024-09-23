/*
  Implements a first-order transition table-based recreation of the classic song "happy birthday"
  Separate transition tables are used to model first-order pitch sequences and measure-to-measure rhythm sequences
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/triangle_warm8192_int8.h>
mOscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc(TRIANGLE_WARM8192_DATA);
ADSR<CONTROL_RATE, AUDIO_RATE> env;

// for triggering the envelope
EventDelay note_delay;
int sixteenth_length = 100;

#define NUM_NOTES 8
int note_pitches[] = { 60, 62, 64, 65, 67, 69, 70, 72 };  // list of pitches used in each row of transition table

float note_transition[NUM_NOTES][NUM_NOTES] =  // tally of number of occurences of each note transition
  {
    { 3, 2, 0, 1, 1, 0, 0, 1 },  //from C
    { 2, 0, 0, 0, 0, 0, 1, 0 },  //from D
    { 2, 0, 0, 0, 0, 0, 1, 0 },  //from E
    { 1, 0, 2, 0, 1, 0, 0, 0 },  //from f
    { 0, 0, 0, 2, 0, 0, 0, 0 },  //from G
    { 0, 0, 0, 2, 0, 0, 0, 0 },  //from A
    { 0, 0, 0, 0, 0, 1, 1, 0 },  //from Bb
    { 0, 0, 0, 0, 1, 0, 0, 0 },  //from C^
  };

float note_transition_cumulative[NUM_NOTES][NUM_NOTES] =  // in setup we will convert the above transition table to cumulative distribution function
  {
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from C
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from D
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from E
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from f
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from G
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from A
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from Bb
    { 0, 0, 0, 0, 0, 0, 0, 0 },  //from C^
  };

int curr_note = 0;

// Transition table for rhythms on a measure-wide basis
// 0: q q q
// 1: h e. s
// 2: q q e. s

#define NUM_RHYTHMS 3

float rhythm_transition[3][3] = {
  { 0, 3, 1 },  // from 0: q q q
  { 3, 0, 0 },  // from 1: h e. s
  { 1, 0, 0 },  // from 2: q q e. s
};

float rhythm_cumulative[3][3] = {
  { 0, 0, 0},
  { 0, 0, 0},
  { 0, 0, 0},
};

int current_rhythm;           // rhythm currently playing
int rhythm_queue[4] = { 0 };  // queue of note lengths to play (max of 4 because longest rhythm is 4 notes)
int rhythm_length;            // length of current rhythm
int rhythm_index;             // what note within current rhythm are we playing


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  // ---------- YOUR SETUP CODE BELOW ----------
  env.setADLevels(255, 200);  // set attack level to maximum, and sustain level slightly lower
  env.setTimes(1, 100, 0, 500);

  // ---------- CALCULATING NOTE TRANSITION TABLE ----------
  int note_sums[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // CALCULATING SUMS
  for (int row = 0; row < NUM_NOTES; row++) {
    for (int col = 0; col < NUM_NOTES; col++) {
      note_sums[row] += note_transition[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < NUM_NOTES; row++) {
    for (int col = 0; col < NUM_NOTES; col++) {
      note_transition[row][col] /= note_sums[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < NUM_NOTES; row++) {
    for (int col = 0; col < NUM_NOTES; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        note_transition_cumulative[row][col] += note_transition[row][i];
      }
    }
  }

  // ------CALCULATING RHYTHM TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int rhythm_sums[NUM_RHYTHMS] = { 0 };

  // CALCULATING SUMS
  for (int row = 0; row < NUM_RHYTHMS; row++) {
    for (int col = 0; col < NUM_RHYTHMS; col++) {
      rhythm_sums[row] += rhythm_transition[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < NUM_RHYTHMS; row++) {
    for (int col = 0; col < NUM_RHYTHMS; col++) {
      rhythm_transition[row][col] /= rhythm_sums[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < NUM_RHYTHMS; row++) {
    for (int col = 0; col < NUM_RHYTHMS; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        rhythm_cumulative[row][col] += rhythm_transition[row][i];
      }
    }
  }

  // initialize first rhythm
  rhythm_queue[0] = 4 * sixteenth_length;
  rhythm_queue[1] = 4 * sixteenth_length;
  rhythm_queue[2] = 4 * sixteenth_length;
  rhythm_length = 3;
  rhythm_index = 0;
  note_delay.start(rhythm_queue[0]);
  env.noteOn();
  osc.setFreq(mtof(note_pitches[curr_note]));
}


void loop() {
  audioHook();  // handles Mozzi audio generation behind the scenes
}


/** Called automatically at rate specified by CONTROL_RATE macro, most of your mode should live in here
	*/
void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
  if (note_delay.ready()) {  // silent night
    //move to next pitch
    float R = meap.frand();
    for (int i = 0; i < 8; i++) {
      if (note_transition_cumulative[curr_note][i] >= R) {
        curr_note = i;
        break;
      }
    }

    osc.setFreq(mtof(note_pitches[curr_note]));  //set frequency of sine oscillator
    env.noteOn();

    rhythm_index++;
    if (rhythm_index >= rhythm_length) {  //reached end of current rhythm
      rhythm_index = 0;
      float R = meap.frand();
      for (int i = 0; i < NUM_RHYTHMS; i++) {
        if (rhythm_cumulative[current_rhythm][i] >= R) {
          current_rhythm = i;
          break;
        }
      }
      switch (current_rhythm) {
        case 0:  // q q q
          rhythm_queue[0] = 4 * sixteenth_length;
          rhythm_queue[1] = 4 * sixteenth_length;
          rhythm_queue[2] = 4 * sixteenth_length;
          rhythm_length = 3;
          break;
        case 1:  // h e. s
          rhythm_queue[0] = 8 * sixteenth_length;
          rhythm_queue[1] = 3 * sixteenth_length;
          rhythm_queue[2] = 1 * sixteenth_length;
          rhythm_length = 3;
          break;
        case 2:  // q q e. s
          rhythm_queue[0] = 4 * sixteenth_length;
          rhythm_queue[1] = 4 * sixteenth_length;
          rhythm_queue[2] = 3 * sixteenth_length;
          rhythm_queue[3] = sixteenth_length;
          rhythm_length = 4;
          break;
      }
    }
    note_delay.start(rhythm_queue[rhythm_index]);
  }
  env.update();
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
	*/
AudioOutput_t updateAudio() {
  int64_t out_sample = osc.next() * env.next();
  return StereoOutput::fromNBit(16, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
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