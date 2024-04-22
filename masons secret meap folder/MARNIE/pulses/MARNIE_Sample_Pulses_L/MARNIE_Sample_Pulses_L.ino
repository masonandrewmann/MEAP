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
#define POLYPHONY 8

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



// Sample voices
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];                          //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;                                                                 //default freq of a sample, calculated using length of sample and sample rate
float sample_freq[POLYPHONY];                                                       /**< stores current frequency of each sample voice */
bool sample_active[POLYPHONY] = { true, true, true, true, true, true, true, true }; /**< keeps track of whether a sample voice is playing.  */
int curr_voice = 0;                                                                 // voice from sample[] array being used

int curr_program = 0;  // current midi program, primarily used to choose sample

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY]; /**< ADSR envelopes for all sample voices */
int sample_gain[POLYPHONY];
int attack_time = 1;
int sustain_time = 10;
int release_time = 5;
int sample_pan[POLYPHONY] = { 0, 1, -1, 0, 0, 1, -1, 0 };
int sample_loop_step[POLYPHONY] = { 24, 12, 3, 16, 20, 4, 8, 36 };
int sample_curr_step[POLYPHONY] = { 0 };
int sample_note[POLYPHONY] = { 60, 67, 52, 48, 36, 43, 40, 31 };
int sample_num[POLYPHONY] = { 5, 5, 5, 5, 5, 5, 5, 5 };

int transpose = 0;

int note_bank[14] = {24, 36, 48, 60, 72, 31, 43, 55, 67, 40, 52, 64, 47, 53};

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
  for (int i = 0; i < POLYPHONY; i++) {
    sample_loop_step[i] = sample_loop_step[i] * 2;
    sample[i].setTable(piano_DATA);
    sample[i].setEnd(piano_NUM_CELLS);
    sample[i].setStart(1000);
    sample_freq[i] = default_freq;
    sample[i].setFreq(sample_freq[i]);
    sample_gain[i] = 0;
    sample_env[i].setAttackTime(attack_time);
    sample_env[i].setSustainTime(sustain_time);
    sample_env[i].setReleaseTime(release_time);
    sample_env[i].setADLevels(255, 255);
    sample_curr_step[i] = sample_loop_step[i] - 1;
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
  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }
}


AudioOutput_t updateAudio() {
  int32_t l_sample = 0;
  int32_t r_sample = 0;

  // add all samples to envelope
  for (int i = 0; i < POLYPHONY; i++) {
    sample_gain[i] = sample_env[i].next();
    int32_t immediate_sample = sample[i].next();
    if (sample_pan[i] == 0) {
      l_sample += immediate_sample * sample_gain[i] >> 1;
      r_sample += immediate_sample * sample_gain[i] >> 1;
    } else if (sample_pan[i] == 1) {
      r_sample += sample[i].next() * sample_gain[i];
    } else {
      l_sample += sample[i].next() * sample_gain[i];
    }
  }
  return StereoOutput::fromAlmostNBit(19, l_sample, r_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {
    if (sample_active[number]) {
      sample_active[number] = false;
    } else {
      sample_active[number] = true;
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
            transpose = 7;
            clock_bpm = 120;
            clock_period_micros = meap.midiPulseMicros(clock_bpm);
          } else {  // button 0 up
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 down
            transpose = 0;
            clock_bpm = 80;
            clock_period_micros = meap.midiPulseMicros(clock_bpm);
          } else {  // button 1 up
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 down
            clock_bpm = 53.3333;
            clock_period_micros = meap.midiPulseMicros(clock_bpm);
            transpose = -5;
          } else {  // button 2 up
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 down
            transpose = -10;
            clock_bpm = 35.5555;
            clock_period_micros = meap.midiPulseMicros(clock_bpm);
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
            for (int i = 0; i < POLYPHONY; i++) {
              sample_num[i] = (sample_num[i] + 1);
              if (sample_num[i] >= NUM_SAMPLES) {
                sample_num[i] = NUM_SAMPLES - 1;
              }
            }
            Serial.println(sample_num[0]);
          } else {  // button 5 up
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 down
            for (int i = 0; i < POLYPHONY; i++) {
              sample_num[i] = (sample_num[i] - 1);
              if (sample_num[i] < 0) {
                sample_num[i] = 0;
              }
            }
            Serial.println(sample_num[0]);
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
  for (int i = 0; i < POLYPHONY; i++) {
    if (sample_active[i]) {
      if (sample_curr_step[i] == 0) {
        samplePulse(i, sample_note[i], sample_num[i]);
      } else {
        lineup = false;
      }
      sample_curr_step[i]--;
      if (sample_curr_step[i] < 0) {
        sample_curr_step[i] = sample_loop_step[i] - 1;
      }
    }
  }

  if (lineup) {
    debugln("all lined up");
    // generatePattern();
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

void samplePulse(int voice_num, int midi_note_num, int sample_num) {
  //set sample

  sample[voice_num].setTable(samples_list[sample_num]);
  sample[voice_num].setEnd(samples_lengths[sample_num]);

  //set note
  sample_freq[voice_num] = sample_freq_table[midi_note_num + transpose];
  sample[voice_num].setFreq(sample_freq[voice_num]);
  sample[voice_num].start();

  //set envelope
  int sustain_min = map(clock_bpm, 30, 120, 100, 20);
  int sustain_max = map(clock_bpm, 30, 120, 1000, 500);
  sustain_time = map(sample_loop_step[voice_num], 1, 48, sustain_min, sustain_max);
  int atk = sustain_time / 4;
  int rel = sustain_time *2;
  sample_env[voice_num].setAttackTime(atk);
  sample_env[voice_num].setSustainTime(sustain_time);
  sample_env[voice_num].setReleaseTime(rel);
  sample_env[voice_num].noteOn();
}

void generatePattern() {
  for (int i = 0; i < POLYPHONY; i++) {
    sample_pan[i] = meap.irand(-1, 1);
    sample_curr_step[i] = meap.irand(0, 24);
    int random_sample = meap.irand(0, 4095);
    if (random_sample < meap.aux_mux_vals[6]) {
      sample_num[i] = 3;
    } else {
      sample_num[i] = 4;
    }
    sample_note[i] = note_bank[meap.irand(0, 13)];
    sample_loop_step[i] = meap.irand(3, 54);
  }
  // int sample_pan[POLYPHONY] = { 0, 1, -1, 0, 0, 1, -1, 0 };
  // int sample_loop_step[POLYPHONY] = { 24, 12, 3, 16, 20, 4, 8, 36 };
  // int sample_curr_step[POLYPHONY] = { 0 };
  // int sample_note[POLYPHONY] = { 60, 67, 52, 48, 36, 43, 40, 31 };
  // int sample_num[POLYPHONY] = { 0, 5, 5, 0, 5, 5, 5, 5 };
}