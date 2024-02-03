/*
  // DIP 1 - up enables sample looping
  // DIP 2 - 
  // DIP 3 - 
  // DIP 4 - 
  // DIP 5 - 
  // DIP 6 - 
  // DIP 7 - up enables filter cutoff randomization
  // DIP 8 - up is bandpass, down is lowpass
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <MIDI.h>
#include <SPI.h>
#include <ADSR.h>
#include <Sample.h>
#include <LinkedList.h>
#include <Meap.h>
#include "piano.h"
#include "C_maj.h"
#include "tpt.h"
#include "glock.h"
#include "gtr.h"
#include "sine.h"
#include "harp.h"
#include "clari.h"
#include "C_aug.h"
#include "C_dim.h"
#include "C_min.h"
#include <ResonantFilter.h>
#include <AudioDelayFeedback.h>

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable

#define DEBUG 1      // 1 to enable serial prints, 0 to disable
#define PROBEMIDI 1  // 1 to enable MIDI monitor prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#if PROBEMIDI == 1
#define probemidi(x) Serial.print(x)
#define probemidiln(x) Serial.println(x)
#else
#define probemidi(x)
#define probemidiln(x)
#endif

#define POLYPHONY 40
#define MAX_SAMPLE_LENGTH 300000
#define MIDI_NOTE_CHANNEL 1
#define MIDI_PAD_CHANNEL 10

enum sequencerStates {
  PAUSED,
  PLAYING
};

enum clockModes {
  CK_INTERNAL,
  CK_EXTERNAL
};


using namespace admux;


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

Meap meap;

// Sample voices
Sample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;
float sample_freq[POLYPHONY];
bool sample_active[POLYPHONY] = { false };
int curr_voice = 0;

const int8_t *samples_list[11] = { piano_DATA, clari_DATA, tpt_DATA, glock_DATA, gtr_DATA, sine_DATA, harp_DATA, C_maj_DATA, C_min_DATA, C_dim_DATA, C_aug_DATA };
int samples_lengths[11] = { piano_NUM_CELLS, clari_NUM_CELLS, tpt_NUM_CELLS, glock_NUM_CELLS, gtr_NUM_CELLS, sine_NUM_CELLS, harp_NUM_CELLS, C_maj_NUM_CELLS, C_min_NUM_CELLS, C_dim_NUM_CELLS, C_aug_NUM_CELLS };
int curr_sample = 0;

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY];
int sample_gain[POLYPHONY];
int attack_time = 1;
int release_time = 2000;
bool attack_flag = false;
bool release_flag = false;

class MidiNoteVoice {
public:
  int note_num;
  int voice_num;
  float frequency;
};

LinkedList<MidiNoteVoice *> voiceQueue = LinkedList<MidiNoteVoice *>();

MultiResonantFilter<uint8_t> filter;
int cutoff = 120;
int resonance = 200;
bool cutoff_flag = false;
bool resonance_flag = false;
int rand_cutoff_range = 10;

// retrigger pads
bool retrigger_active[4] = { false, false, false, false };  // bar, half, quarter, eighth

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 10000;
int midi_step_num = 0;
bool start_flag = false;
bool stop_flag = false;
int midi_clock_mode = CK_INTERNAL;

// pitchbend
int pitchbend_semitones = 12;  // num of semitones the pitchbend wheel bends
float bend_amount = 1;

// guitar chord voice
int chord_tonic = -1;
int chord_qualities[12] = { 7, 9, 8, 10, 8, 7, 9, 7, 9, 8, 7, 9 };  // 7=maj 8=min 9=dim 10=aug



void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  default_freq = (float)piano_SAMPLERATE / (float)MAX_SAMPLE_LENGTH;
  for (int i = 0; i < POLYPHONY; i++) {
    sample[i].setTable(piano_DATA);
    sample[i].setEnd(piano_NUM_CELLS);
    sample_freq[i] = default_freq;
    sample[i].setFreq(sample_freq[i]);
    sample_gain[i] = 0;
    sample_env[i].setAttackTime(attack_time);
    sample_env[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    sample_env[i].setReleaseTime(release_time);
    sample_env[i].setADLevels(255, 255);
  }
  filter.setCutoffFreq(cutoff);
  filter.setResonance(resonance);

  midi_micros = meap.midiPulseMicros(120);
}


void loop() {
  audioHook();
  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }

  // handle generating midi clock
  if (midi_clock_mode == CK_INTERNAL) {
    uint32_t t = micros();
    if (t > midi_timer) {
      midi_timer = t + midi_micros;
      clockStep();
    }
  }
}


void updateControl() {
  meap.readPots();
  meap.readTouch();
  meap.readDip();

  // generate amplitude envelopes
  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }

  // trigger new notes
  if (attack_flag) {
    attack_flag = false;
    attack_time = map(attack_time, 0, 127, 1, 2000);
    for (int i = 0; i < POLYPHONY; i++) {
      sample_env[i].setAttackTime(attack_time);
    }
  }

  // release notes
  if (release_flag) {
    release_flag = false;
    release_time = map(release_time, 0, 127, 1, 2000);
    for (int i = 0; i < POLYPHONY; i++) {
      sample_env[i].setReleaseTime(release_time);
    }
  }

  // update cutoff
  if (cutoff_flag) {
    cutoff_flag = false;
    cutoff = cutoff << 1;  // map to 0-255
    filter.setCutoffFreq(cutoff);
  }

  // update resonance
  if (resonance_flag) {
    resonance_flag = false;
    resonance = resonance << 1;  // map to 0-255
    filter.setResonance(resonance);
    filter.setCutoffFreq(cutoff);
  }

  // read tempo from pot 0
  midi_micros = meap.midiPulseMicros(map(meap.potVals[0], 0, 4095, 80, 160));
  // debugln(midi_micros);
}


AudioOutput_t updateAudio() {
  int out_sample = 0;
  // add all samples to envelope
  for (int i = 0; i < POLYPHONY; i++) {
    sample_gain[i] = sample_env[i].next();
    out_sample += sample[i].next() * sample_gain[i];
  }
  // send output through filter
  filter.next(out_sample);
  int filtered_out;
  if (meap.dipVals[7]) {
    filtered_out = filter.band();
  } else {
    filtered_out = filter.low();
  }
  //send output to DAC
  return StereoOutput::fromAlmostNBit(19, filtered_out, filtered_out);
}

void Meap::updateDip(int number, bool up) {
  switch (number) {
    case 0:
      {
        if (up) {  // DIP 1 up
          for (int i = 0; i < POLYPHONY; i++) {
            sample[i].setLoopingOn();
          }
        } else {  // DIP 1 down
          for (int i = 0; i < POLYPHONY; i++) {
            sample[i].setLoopingOff();
          }
        }
      }
    case 1:
      {
        if (up) {  // DIP 2 up

        } else {  // DIP 2 down
        }
      }
    case 2:
      {
        if (up) {  // DIP 3 up

        } else {  // DIP 3 down
        }
      }
    case 3:
      {
        if (up) {  // DIP 4 up

        } else {  // DIP 4 down
        }
      }
    case 4:
      {
        if (up) {  // DIP 5 up

        } else {  // DIP 5 down
        }
      }
    case 5:
      {
        if (up) {  // DIP 6 up

        } else {  // DIP 6 down
        }
      }
    case 6:
      {
        if (up) {  // DIP 7 up

        } else {  // DIP 7 down
        }
      }
    case 7:
      {
        if (up) {  // DIP 8 up

        } else {  // DIP 8 down
        }
      }
  }
}

void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // any pad pressed
    // programChangeHandler(number);
    // Serial.println(number);
  } else {  // any pad released
  }

  switch (number) {
    case 0:
      {
        if (pressed) {  // Pad 1 pressed

        } else {  // Pad 1 released
        }
      }
    case 1:
      {
        if (pressed) {  // Pad 2 pressed

        } else {  // Pad 2 released
        }
      }
    case 2:
      {
        if (pressed) {  // Pad 3 pressed

        } else {  // Pad 3 released
        }
      }
    case 3:
      {
        if (pressed) {  // Pad 4 pressed

        } else {  // Pad 4 released
        }
      }
    case 4:
      {
        if (pressed) {  // Pad 5 pressed

        } else {  // Pad 5 released
        }
      }
    case 5:
      {
        if (pressed) {  // Pad 6 pressed

        } else {  // Pad 6 released
        }
      }
    case 6:
      {
        if (pressed) {  // Pad 7 pressed

        } else {  // Pad 7 released
        }
      }
    case 7:
      {
        if (pressed) {  // Pad 8 pressed

        } else {  // Pad 8 released
        }
      }
  }
}

//executes when a clock step is received
void clockStep() {
  midi_step_num = (midi_step_num + 1) % 24;

  if (midi_step_num % 24 == 0) {  // quarter note
    if (retrigger_active[0]) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 12 == 0) {  // eighth note
    if (meap.dipVals[6]) {
      randomizeCutoff(rand_cutoff_range);
    }
    if (retrigger_active[1]) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 6 == 0) {  // sixteenth note
    if (meap.dipVals[6]) {
      if (meap.irand(0, 4) == 0) {
        randomizeCutoff(rand_cutoff_range);
      }
    }
    if (retrigger_active[2]) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 3 == 0) {  // thirtysecond note
    if (retrigger_active[3]) {
      retriggerNotes(true);
    }
  }
}

/**
* @brief Randomizes cutoff of filter.
*
* @param range is the range to randomize filter over in percentage
*/
void randomizeCutoff(int range) {
  range = range << 1;  // maps input range to 0-200
  int rand_cutoff = cutoff + meap.irand(-range, range);
  // clip to range 0-255
  if (rand_cutoff > 255) {
    rand_cutoff = 255;
  } else if (rand_cutoff < 0) {
    rand_cutoff = 0;
  }
  filter.setCutoffFreq(rand_cutoff);
}


/**
* @brief Handles turning on sample notes.
*
* @param note is the MIDI note number to turn on
* @param velocity is the velocity of the MIDI note to turn on (NOT CURRENTLY USED)
* @param channel is the velocity of the MIDI note to turn on (NOT CURRENTLY USED)
* @param pgm_override overrides the current program to play note on specified program 
*/
void noteOnHandler(int note, int velocity, int channel, int pgm_override) {
  // choose voice to play
  int my_voice = curr_sample;

  if (pgm_override = !-1) {
    my_voice = pgm_override;
  }
  if (my_voice == 7) {        // guitar chord voice
    if (chord_tonic == -1) {  // tonic hasn't been chosen yet, choose it now and don't play a note
      chord_tonic = note % 12;
      return;
    } else {  // tonic has been chosen, choose correct chord
      int mod_note = note % 12;
      if (mod_note < chord_tonic) {
        mod_note += 12;
      }
      int root_diff = mod_note - chord_tonic;  // how many semitones above tonic is the note
      my_voice = chord_qualities[root_diff];
    }
  }

  if (channel == MIDI_NOTE_CHANNEL) {
    // find a free voice
    curr_voice = -1;
    for (int i = 0; i < POLYPHONY; i++) {
      if (!sample_env[i].playing()) {
        sample_active[i] = true;
        curr_voice = i;
        break;
      }
    }

    if (curr_voice != -1) {  // if any voice is free, play note
      sample[curr_voice].setTable(samples_list[my_voice]);
      sample[curr_voice].setEnd(samples_lengths[my_voice]);

      // calculate frequency offset for sample
      float diff = note - 60;
      sample_freq[curr_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample

      // start the note
      sample[curr_voice].setFreq(sample_freq[curr_voice] * bend_amount);  // set frequency of sample
      sample_env[curr_voice].noteOn();
      sample[curr_voice].start();

      // keep track of note in active notes queue
      MidiNoteVoice *myNote = new MidiNoteVoice();  // create object with pair of note num and voice num
      myNote->note_num = note;
      myNote->voice_num = curr_voice;
      myNote->frequency = sample_freq[curr_voice];
      voiceQueue.add(myNote);  // add to active notes queue
    }
  } else if (channel == MIDI_PAD_CHANNEL) {
    switch (note) {
      case 50:
        retrigger_active[0] = !retrigger_active[0];
        break;
      case 45:
        retrigger_active[1] = !retrigger_active[1];
        break;
      case 51:
        retrigger_active[2] = !retrigger_active[2];
        break;
      case 49:
        retrigger_active[3] = !retrigger_active[3];
        break;
    }
  }
}

/**
* @brief Handles turning off sample notes.
*
* @param note is the MIDI note number to turn off
* @param channel is the MIDI channel of the note to turn off
*/
void noteOffHandler(int note, int channel) {
  if (channel == MIDI_NOTE_CHANNEL) {
    int queue_size = voiceQueue.size();
    if (queue_size > 0) {
      MidiNoteVoice *myNote;
      // loop through voice queue searching for note
      for (int i = 0; i < queue_size; i++) {
        myNote = voiceQueue.get(i);
        if (note == myNote->note_num) {
          sample_env[myNote->voice_num].noteOff();   // turn off note
          sample_active[myNote->voice_num] = false;  // free up voice
          delete (myNote);
          voiceQueue.remove(i);  // remove from active note queue
          break;
        }
      }
    }
  } else if (channel == MIDI_PAD_CHANNEL) {
  }
}

/**
* @brief Handles changing MIDI programs
*
* @param program_num is the MIDI program to change to
*/
void programChangeHandler(int program_num) {
  //GENERIC SAMPLE CHANGE
  curr_sample = program_num;

  switch (program_num) {
    case 0:  // PIANO
      break;
    case 1:  // Clarinet
      break;
    case 2:  // Trumpet
      break;
    case 3:  // Glockenspiel
      break;
    case 4:  // Guitar
      break;
    case 5:  // Sine
      break;
    case 6:  // Harp
      break;
    case 7:  // Guitar chord nonsense
      chord_tonic = -1;
      break;
  }
}

/**
* @brief Retriggers one or all active notes
*
* @param all when true will retrigger all notes, when false, just the most recent note
*/
void retriggerNotes(bool all) {
  if (all == false) {
    sample[curr_voice].setFreq(sample_freq[curr_voice]);  // set frequency of sample
    sample_env[curr_voice].noteOn();
    sample[curr_voice].start();
  } else {
    int queue_size = voiceQueue.size();
    if (queue_size > 0) {
      MidiNoteVoice *myNote;
      // loop through voice queue searching for note
      for (int i = 0; i < queue_size; i++) {
        myNote = voiceQueue.get(i);
        sample_env[myNote->voice_num].noteOn(true);
        sample[myNote->voice_num].start();
      }
    }
  }
}

void midiEventHandler() {
  int channel = -1;
  int data1 = -1;
  int data2 = -1;
  switch (MIDI.getType())  // Get the type of the message we caught
  {
    case midi::NoteOn:  // ---------- MIDI NOTE ON RECEIVED ----------
      {
        channel = MIDI.getChannel();
        data1 = MIDI.getData1();  // note
        data2 = MIDI.getData2();  // velocity
        probemidi("ch: ");
        probemidi(channel);
        probemidi(" noteOn: ");
        probemidi(data1);
        probemidi(" velocity: ");
        probemidiln(data2);
        noteOnHandler(data1, data2, channel, -1);
        break;
      }
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
      {
        channel = MIDI.getChannel();
        data1 = MIDI.getData1();  // note
        probemidi("Note Off: ");
        probemidi("note: ");
        probemidiln(data1);
        noteOffHandler(data1, channel);
        break;
      }

    case midi::ProgramChange:   // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      data1 = MIDI.getData1();  // program number
      probemidi("Pgm: ");
      probemidiln(data1);
      programChangeHandler(data1);
      break;

    case midi::ControlChange:  // ---------- MIDI CONTROL CHANGE RECEIVED ----------
      data1 = MIDI.getData1();
      data2 = MIDI.getData2();
      probemidi("cc: ");
      probemidi(data1);
      probemidi(" data: ");
      probemidiln(data2);
      switch (data1) {
        case 71:  // knob #1
          cutoff = data2;
          cutoff_flag = true;
          break;
        case 74:  //knob #2
          resonance = data2;
          resonance_flag = true;
          break;
        case 84:  //knob #3
          attack_time = data2;
          attack_flag = true;
          break;
        case 7:  //knob #4
          release_time = data2;
          release_flag = true;
          break;
        case 91:  // knob 5
          rand_cutoff_range = data2;
          break;
        case 93:  //knob #6
          break;
        case 5:  //knob #7
          break;
        case 10:  //knob #8
          break;
        case 1:  // mod wheel
          break;
      }
      break;
    case midi::PitchBend:{  // ---------- MIDI PITCH BEND RECEIVED ----------
      // 1. combine two 7-bit data vals into one 14-bit val
      // 2. map to reside in range -1 to 1
      // 3. multiply by number of semitones in bend range
      // 4. use that number in equal tempered formula f * 2^(pb/12)
      // 5. apply to all active notes
      data1 = MIDI.getData1();
      data2 = MIDI.getData2();
      probemidi(data1);
      probemidi(" ");
      probemidi(data2);
      probemidi(" ");
      // combine data bits into pitch bend value
      // int pitchbend_val = (data1 << 7) + data2;  // now resides between,,, 0 and 16383
      // pitchbend_val -= 8192;                     // now ,,, -8192 to 8191
      // scale pos numbers by 8191 and negative numbers by 8192
      // float bend_amount;
      // if (pitchbend_val < 0) {
      //   bend_amount = (float)pitchbend_val / 8192;
      // } else {
      //   bend_amount = (float)pitchbend_val / 8191;
      // }
      // Only seem to be getting pitchbend data on data2 so im gonna use 7 bit pitchbend
      data2 -= 64; // -64 to +63
      if (data2 < 0) {
        bend_amount = (float)data2 / 64.f;
      } else {
        bend_amount = (float)data2 / 63.f;
      }
      bend_amount *= pitchbend_semitones;
      bend_amount = pow(2, bend_amount / 12.f);
      // apply to all active notes
      int queue_size = voiceQueue.size();
      MidiNoteVoice *my_note;
      for (int i = 0; i < queue_size; i++) {
        my_note = voiceQueue.get(i);
        sample[my_note->voice_num].setFreq(my_note->frequency * bend_amount);
      }
      probemidi("pitchbend: ");
      probemidiln(data2);
      break;
    }
    case midi::Clock:  // ---------- MIDI CLOCK PULSE RECEIVED ----------
      if (midi_clock_mode == CK_EXTERNAL) {
        clockStep();
      }
      break;
  }
}