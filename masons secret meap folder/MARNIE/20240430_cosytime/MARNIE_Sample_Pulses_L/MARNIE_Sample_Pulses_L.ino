/*
  Basic template for a MARNIE L program with clock and midi
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <Meap.h>
#include <tables/sin8192_int8.h>
#include "sample_includes.h"
#include "sample_frequencies.h"  // NEED TO RECALCULATE THESE VALUES IF DIFF SAMPLE SR OR MAX LENGTH IS USED
#include "MySample.h"
#define MAX_SAMPLE_LENGTH 300000  // Max length of a sample


#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#define PULSE_VOICES 16

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} clock_mode;

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); /**< MEAP hardware serial port*/

// MIDI clock timer
uint32_t clock_timer = 0;
uint32_t clock_period_micros = 10000;
int clock_pulse_num = 0;
float clock_bpm = 120;

Meap meap;

int majScale[] = { 0, 2, 4, 5, 7, 9, 11 };  // template for building major scale on top of 12TET
int scale_root = 60;                        // root of major scale

int triad[] = { 0, 2, 4 };  // template for triad on top of major scale
int chord_root = 2;         // root of triad
int chord_tree_level = 4;   // what level of chord tree are we on

int entropy = 0;  // amount of entropy from 0 to 100

float tempos[5] = { 23.7047, 35.555, 53.333, 80, 120 };
int tempo_level = 3;

int rhythms[8] = { 4, 8, 12, 16, 18, 24, 36, 48 };

int timbre_level = 0;

int timbre_pairs[14][2] = {
  { 24, 24 },
  { 24, 4 },
  { 16, 4 },
  { 16, 21 },
  { 3, 4 },
  { 3, 18 },
  { 16, 18 },
  { 24, 22 },
  { 21, 22 },
  { 21, 12 },
  { 15, 22 },
  { 15, 12 },
  { 16, 12 },
  { 24, 12 }
};

int density_value = 2;
int density_direction = 1;

// Sample voices
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> pulse_sample[PULSE_VOICES];                                                                               //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;                                                                                                                               //default freq of a sample, calculated using length of sample and sample rate
float pulse_freq[PULSE_VOICES];                                                                                                                   /**< stores current frequency of each sample voice */
bool pulse_active[PULSE_VOICES] = { true, true, true, true, true, true, true, true }; /**< keeps track of whether a sample voice is playing.  */  // voice from sample[] array being used

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> pulse_env[PULSE_VOICES]; /**< ADSR envelopes for all sample voices */
int pulse_gain[PULSE_VOICES];
int pulse_attack = 1;
int pulse_sustain_time = 10;
int pulse_release = 5;
int pulse_pan[PULSE_VOICES] = { 0, 1, -1, 0, 0, 1, -1, 0 };
int pulse_loop_step[PULSE_VOICES] = { 24, 12, 3, 16, 20, 4, 8, 36 };
int pulse_curr_step[PULSE_VOICES] = { 0 };
int pulse_note[PULSE_VOICES] = { 60, 67, 52, 48, 36, 43, 40, 31 };
int pulse_sample_num[PULSE_VOICES] = { 5, 5, 5, 5, 5, 5, 5, 5 };

int pulse_transpose = 0;

int note_bank[14] = { 24, 36, 48, 60, 72, 31, 43, 55, 67, 40, 52, 64, 47, 53 };

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  clock_mode = kINTERNAL;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  // initialize samples
  default_freq = (float)piano_SAMPLERATE / (float)MAX_SAMPLE_LENGTH;
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_loop_step[i] = pulse_loop_step[i] * 2;
    pulse_sample[i].setTable(piano_DATA);
    pulse_sample[i].setEnd(piano_NUM_CELLS);
    pulse_sample[i].setStart(1000);
    pulse_freq[i] = default_freq;
    pulse_sample[i].setFreq(pulse_freq[i]);
    pulse_gain[i] = 0;
    pulse_env[i].setAttackTime(pulse_attack);
    pulse_env[i].setSustainTime(pulse_sustain_time);
    pulse_env[i].setReleaseTime(pulse_release);
    pulse_env[i].setADLevels(255, 255);
    pulse_curr_step[i] = pulse_loop_step[i] - 1;
  }
}


void loop() {
  audioHook();
  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }

  // handle generating midi clock
  if (clock_mode == kINTERNAL) {
    uint32_t t = micros();
    if (t > clock_timer) {
      clock_timer = t + clock_period_micros;
      MIDI.sendRealTime(midi::Clock);
      clockStep();
    }
  }
}


void updateControl() {
  meap.readPots();
  meap.readTouch();
  meap.readDip();
  meap.readAuxMux();
  updateButtons();


  // update amplitude envelopes
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_env[i].update();
  }
}


AudioOutput_t updateAudio() {
  int32_t l_sample = 0;
  int32_t r_sample = 0;

  // add all samples to envelope
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_gain[i] = pulse_env[i].next();
    int32_t immediate_sample = pulse_sample[i].next();
    if (pulse_pan[i] == 0) {
      l_sample += immediate_sample * pulse_gain[i] >> 1;
      r_sample += immediate_sample * pulse_gain[i] >> 1;
    } else if (pulse_pan[i] == 1) {
      r_sample += immediate_sample * pulse_gain[i];
    } else {
      l_sample += immediate_sample * pulse_gain[i];
    }
  }
  return StereoOutput::fromAlmostNBit(19, l_sample, r_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {
    if (pulse_active[number]) {
      pulse_active[number] = false;
    } else {
      pulse_active[number] = true;
    }
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

void Meap::updateDip(int number, bool up) {
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

void updateButtons() {
  for (int i = 0; i < 7; i++) {
    // Read the buttons
    if (i == 0) {
      if (meap.aux_mux_vals[7] < 1000) {
        button_vals[0] = 0;
      } else {
        button_vals[0] = 1;
      }
    } else {
      button_vals[i] = digitalRead(button_pins[i - 1]);
    }
    // Check if state has changed
    if (button_vals[i] != last_button_vals[i]) {
      switch (i) {
        case 0:
          if (button_vals[i]) {  // button 0 down
            chordTreeStep();
          } else {  // button 0 up
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 down
            tempoTreeStep();
          } else {  // button 1 up
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 down
            densityTreeStep();
          } else {  // button 2 up
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 down
            timbreTreeStep();
          } else {  // button 3 up
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 down
            generatePattern();
          } else {  // button 4 up
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 down
            for (int i = 0; i < PULSE_VOICES; i++) {
              int random_sample = meap.irand(0, 4095);
              if (random_sample < meap.aux_mux_vals[6]) {
                pulse_sample_num[i] = timbre_pairs[timbre_level][0];
              } else {
                pulse_sample_num[i] = timbre_pairs[timbre_level][1];
              }
            }
          } else {  // button 5 up
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 down
            for (int i = 0; i < PULSE_VOICES; i++) {
              pulse_sample_num[i] = (pulse_sample_num[i] - 1);
              if (pulse_sample_num[i] < 0) {
                pulse_sample_num[i] = 0;
              }
            }
          } else {  // button 6 up
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
  }
}


/**
* @brief To be called whenever a midi event is recieved.
*/
void midiEventHandler() {
  int channel = MIDI.getChannel();
  int data1 = MIDI.getData1();
  int data2 = MIDI.getData2();
  switch (MIDI.getType())  // Get the type of the message we caught
  {
    case midi::NoteOn:  // ---------- MIDI NOTE ON RECEIVED ----------
      break;
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
      break;
    case midi::ProgramChange:  // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      break;
    case midi::ControlChange:  // ---------- MIDI CONTROL CHANGE RECEIVED ----------
      break;
    case midi::PitchBend:  // ---------- MIDI PITCH BEND RECEIVED ----------
      break;
    case midi::Clock:  // ---------- MIDI CLOCK PULSE RECEIVED ----------
      if (clock_mode == kEXTERNAL) {
        clockStep();
      }
      break;
    case midi::Start:  // ---------- MIDI START MESSAGE RECEIVED ----------
      break;
    case midi::Stop:  // ---------- MIDI STOP MESSAGE RECEIVED ----------
      break;
    case midi::Continue:  // ---------- MIDI CONTINUE MESSAGE RECEIVED ----------
      break;
  }
}


//executes when a clock step is received
void clockStep() {
  int lineup = true;
  for (int i = 0; i < PULSE_VOICES; i++) {
    if (pulse_active[i]) {
      if (pulse_curr_step[i] == 0) {
        samplePulse(i, pulse_note[i], pulse_sample_num[i]);
      }
      pulse_curr_step[i]--;
      if (pulse_curr_step[i] < 0) {
        pulse_curr_step[i] = pulse_loop_step[i] - 1;
      }
    }
  }


  if (clock_pulse_num % 24 == 0) {  // quarter note
  }

  if (clock_pulse_num % 12 == 0) {  // eighth note
  }

  if (clock_pulse_num % 6 == 0) {  // sixteenth note
  }

  if (clock_pulse_num % 3 == 0) {  // thirtysecond notex
  }

  clock_pulse_num = (clock_pulse_num + 1) % 24;
}

void samplePulse(int voice_num, int midi_note_num, int pulse_sample_num) {
  //set sample

  pulse_sample[voice_num].setTable(samples_list[pulse_sample_num]);
  pulse_sample[voice_num].setEnd(samples_lengths[pulse_sample_num]);

  //set note
  pulse_freq[voice_num] = sample_freq_table[midi_note_num + pulse_transpose];
  pulse_sample[voice_num].setFreq(pulse_freq[voice_num]);
  pulse_sample[voice_num].start();

  //set envelope
  int sustain_min = map(clock_bpm, 30, 120, 40, 20);
  int sustain_max = map(clock_bpm, 30, 120, 1000, 500);
  pulse_sustain_time = map(pulse_loop_step[voice_num], 1, 48, sustain_min, sustain_max);
  int atk = pulse_sustain_time/4;
  int rel = pulse_sustain_time/2;
  pulse_env[voice_num].setAttackTime(atk);
  pulse_env[voice_num].setSustainTime(pulse_sustain_time);
  pulse_env[voice_num].setReleaseTime(rel);
  pulse_env[voice_num].noteOn();
}

void generatePattern() {
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_pan[i] = meap.irand(-1, 1);
    // choose timbre

    pulse_curr_step[i] = meap.irand(0, 24);

    pulse_loop_step[i] = rhythms[meap.irand(0, 7)]*2;
  }
}

void chordTreeStep() {
  // --------------------WHAT STATE SHOULD WE MOVE TO NEXT?--------------------------------------
  if (chord_tree_level != 0) {
    chord_tree_level = chord_tree_level - 1;  // move one level down the tree
  } else {
    chord_tree_level = meap.irand(0, 4);  // if already at bottom of tree jump to a random level
  }

  // --------------------WHAT HAPPENS IN EACH STATE?-------------------------------------------
  switch (chord_tree_level) {  // choose chord based on tree level
    case 4:                    // leaves : iii
      chord_root = 2;          // iii chord (E minor)
      break;
    case 3:                         // twigs: I or vi
      if (meap.irand(0, 3) == 0) {  // 25% chance of I, 75% chance of vi
        chord_root = 0;             // I chord (C major)
      } else {
        chord_root = 5;  // vi chord (A minor)
      }
      break;
    case 2:                         // branches: IV or ii
      if (meap.irand(0, 2) == 0) {  // 33% chance of ii, 66% chance off IV
        chord_root = 1;             // ii chord (D minor)
      } else {
        chord_root = 3;  // IV chord (F major)
      }
      break;
    case 1:                         // boughs: V or vii˚
      if (meap.irand(0, 3) == 0) {  // 25% chance of vii˚, 75% chance of V
        chord_root = 6;             // vii˚ chord (B diminished)
      } else {
        chord_root = 4;  // V chord (G major)
      }
      break;
    case 0:                         // trunk: I or vi
      if (meap.irand(0, 3) == 0) {  // 25% chance of vi, 75% chance of I
        chord_root = 5;             // vi chord (A minor)
      } else {
        chord_root = 0;  // I chord (C major)
      }
      break;
  }
  note_bank[0] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[1] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[2] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[3] = scale_root + majScale[(triad[0] + chord_root) % 7] - 12;
  note_bank[4] = scale_root + majScale[(triad[1] + chord_root) % 7] - 12;
  note_bank[5] = scale_root + majScale[(triad[2] + chord_root) % 7] - 12;
  note_bank[6] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[7] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[8] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[9] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[10] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[11] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[12] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[13] = scale_root + majScale[(triad[2] + chord_root) % 7];

  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_note[i] = note_bank[meap.irand(0, 13)];
  }
}

void tempoTreeStep() {
  if (tempo_level == 0) {
    tempo_level++;
  } else if (tempo_level == 4) {
    tempo_level--;
  } else {
    tempo_level = tempo_level + (1 - 2 * meap.irand(0, 1));
  }
  updateTempo(tempos[tempo_level]);
}

void densityTreeStep() {
  density_value += meap.irand(1, 3) * density_direction;
  if (density_value > PULSE_VOICES) {
    density_value = PULSE_VOICES;
    density_direction = -1;
  } else if (density_value < 2) {
    density_value = 2;
    density_direction = 1;
  }

  for (int i = 0; i < PULSE_VOICES; i++) {
    if (i < density_value) {
      pulse_active[i] = true;
    } else {
      pulse_active[i] = false;
    }
  }
}

void updateTempo(float new_tempo) {
  clock_bpm = new_tempo;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);
}

void timbreTreeStep() {
  int rand_num = 0;
  switch (timbre_level) {
    case 0:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 1;
      } else {
        timbre_level = 7;
      }
      break;
    case 1:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 2;
      } else {
        timbre_level = 4;
      }
      break;
    case 2:
      timbre_level = 3;
      break;
    case 3:
      timbre_level = 12;
      break;
    case 4:
      timbre_level = 5;
      break;
    case 5:
      timbre_level = 6;
      break;
    case 6:
      timbre_level = 12;
      break;
    case 7:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 8;
      } else {
        timbre_level = 10;
      }
      break;
    case 8:
      timbre_level = 9;
      break;
    case 9:
      timbre_level = 12;
      break;
    case 10:
      timbre_level = 11;
      break;
    case 11:
      timbre_level = 12;
      break;
    case 12:
      timbre_level = 13;
      break;
    case 13:
      timbre_level = 0;
      break;
  }
  for (int i = 0; i < PULSE_VOICES; i++) {
    int random_sample = meap.irand(0, 4095);
    if (random_sample < meap.aux_mux_vals[6]) {
      pulse_sample_num[i] = timbre_pairs[timbre_level][0];
    } else {
      pulse_sample_num[i] = timbre_pairs[timbre_level][1];
    }
  }
}