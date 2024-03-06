/*
  // DIP 1 - up enables sample looping
  // DIP 2 - up is external clock , down is internal
  // DIP 3 - 
  // DIP 4 - 
  // DIP 5 - 
  // DIP 6 - 
  // DIP 7 - 
  // DIP 8 - up is bandpass, down is lowpass
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <Mux.h>
#include <MIDI.h>
// #include <SPI.h>
#include <ADSR.h>
// #include <Sample.h>
#include "MySample.h"
#include <LinkedList.h>
#include <Meap.h>
#include "sample_includes.h"
#include <ResonantFilter.h>
#include <Portamento.h>
#include "MyPortamento.h"
#include "SamplePattern.h"

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

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
#define MIDI_OUT_CHANNEL 5 // out to cz101
enum sequencerStates {
  PAUSED,
  PLAYING
};

enum clockModes {
  CK_INTERNAL,
  CK_EXTERNAL
};

enum retrigModes {
  RT_NONE,
  RT_QUARTER,
  RT_EIGHTH,
  RT_SIXTEENTH,
  RT_THIRTYSECOND,
  RT_SIXTYFOURTH
};


using namespace admux;


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

Meap meap;

// Sample voices
// Sample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;
float sample_freq[POLYPHONY];
bool sample_active[POLYPHONY] = { false };
int curr_voice = 0;

int curr_program = 0;

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY];
int sample_gain[POLYPHONY];
int attack_time = 1;
int release_time = 500;
bool attack_flag = false;
bool release_flag = false;

MyPortamento<CONTROL_RATE> portamento[POLYPHONY];



class MidiNoteVoice {
public:
  int note_num;
  int voice_num;
  float frequency;
  bool port_done;
  uint32_t port_end_time;
};

class PatternNote {
public:
  uint8_t note_num;
  uint16_t voice_num;
  uint8_t channel_num;
  uint32_t end_time;
};

LinkedList<MidiNoteVoice *> voiceQueue = LinkedList<MidiNoteVoice *>();

LinkedList<PatternNote *> patternQueue = LinkedList<PatternNote *>();


MultiResonantFilter<uint8_t> filter;
int cutoff = 255;
int resonance = 127;
bool cutoff_flag = false;
bool resonance_flag = false;
int rand_cutoff_range = 0;

// retrigger pads
int retrigger_mode = 0;                                     // 0 = none, 1 =

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 10000;
int midi_step_num = 0;
bool start_flag = false;
bool stop_flag = false;
int midi_clock_mode = CK_INTERNAL;

// pitchbend
int pitchbend_semitones = 2;  // num of semitones the pitchbend wheel bends
float bend_amount = 1;

//portamento
uint16_t port_length = 0;  // length of portamento in milliseconds
Q16n16 last_port_note = 0;

// guitar chord voice
int chord_tonic = -1;
int chord_qualities[12] = { 7, 9, 8, 10, 8, 7, 9, 7, 9, 8, 7, 9 };  // 7=maj 8=min 9=dim 10=aug


SamplePattern my_first_pattern(4, 2, 3);  // 4 steps, 2 repeasts, glockenspiel
uint8_t my_midi_notes[4] = { 60, 62, 64, 67 };
uint16_t my_step_duration[4] = { 4, 4, 4, 4 };
uint16_t my_note_sustain[4] = { 400, 400, 400, 400 };
uint16_t my_amplitude[4] = { 127, 127, 127, 127 };

SamplePattern dylan_blues_pattern(20, -1, 5);
uint8_t dylan_blues_notes[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65}; 
uint16_t dylan_blues_step_duration[20] = {2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1};
uint16_t dylan_blues_note_sustain[20] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
uint16_t dylan_blues_amplitude[20] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 ,127, 127, 127, 127};



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

  my_first_pattern.midi_notes = my_midi_notes;
  my_first_pattern.step_duration = my_step_duration;
  my_first_pattern.note_sustain = my_note_sustain;

  dylan_blues_pattern.midi_notes = dylan_blues_notes;
  dylan_blues_pattern.step_duration = dylan_blues_step_duration;
  dylan_blues_pattern.note_sustain = dylan_blues_note_sustain;
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
      MIDI.sendRealTime(midi::Clock);
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

  // update portamento
  handlePortamento();

  // read tempo from pot 0
  midi_micros = meap.midiPulseMicros((int)map(meap.potVals[0], 0, 4095, 80, 160));
  // debugln(midi_micros);

  //handle patterns
  my_first_pattern.cycle();
  if (my_first_pattern.step_flag) {
    debugln("step");
    my_first_pattern.step_flag = false;
    step(my_first_pattern);
  }

  //handle patterns
  dylan_blues_pattern.cycle();
  if (dylan_blues_pattern.step_flag) {
    debugln("step");
    dylan_blues_pattern.step_flag = false;
    step(dylan_blues_pattern);
  }

  cleanPatternQueue();
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
          midi_clock_mode = CK_EXTERNAL;
        } else {  // DIP 2 down
          midi_clock_mode = CK_INTERNAL;
        }
      }
    case 2:
      {
        if (up) {  // DIP 3 up
        } else {   // DIP 3 down
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
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        my_first_pattern.start();
        debugln("start");
      } else {  // Pad 1 released
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        my_first_pattern.stop();
        debugln("end");
      } else {  // Pad 2 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        dylan_blues_pattern.start();
        debugln("db_start");
      } else {  // Pad 3 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        dylan_blues_pattern.stop();
        debugln("db_stop");
      } else {  // Pad 4 released
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed

      } else {  // Pad 5 released
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed

      } else {  // Pad 6 released
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed

      } else {  // Pad 7 released
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed

      } else {  // Pad 8 released
      }
      break;
  }
}

//executes when a clock step is received
void clockStep() {

  if (midi_step_num % 24 == 0) {  // quarter note
    if (retrigger_mode == RT_QUARTER) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 12 == 0) {  // eighth note
    randomizeCutoff(rand_cutoff_range);
    if (retrigger_mode == RT_EIGHTH) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 6 == 0) {  // sixteenth note
    if (meap.irand(0, 4) == 0) {
      randomizeCutoff(rand_cutoff_range);
    }
    if (retrigger_mode == RT_SIXTEENTH) {
      retriggerNotes(true);
    }

    //send triggers to patterns
    my_first_pattern.incrementClock(midi_step_num);
    dylan_blues_pattern.incrementClock(midi_step_num);
  }

  if (midi_step_num % 3 == 0) {  // thirtysecond note
    if (retrigger_mode == RT_THIRTYSECOND) {
      retriggerNotes(true);
    }
  }

  midi_step_num = (midi_step_num + 1) % 24;
}

/**
* @brief Randomizes cutoff of filter.
*
* @param range is the range to randomize filter over in percentage
*/
void randomizeCutoff(int range) {
  if (range > 0) {
    debugln("cutoff rand");
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
  int my_program = curr_program;

  if (pgm_override = !-1) {
    my_program = pgm_override;
  }
  if (my_program >= 7 && my_program <= 10) {  // guitar chord voice
    if (chord_tonic == -1) {                  // tonic hasn't been chosen yet, choose it now and don't play a note
      chord_tonic = note % 12;
      return;
    } else {  // tonic has been chosen, choose correct chord
      int mod_note = note % 12;
      if (mod_note < chord_tonic) {
        mod_note += 12;
      }
      int root_diff = mod_note - chord_tonic;  // how many semitones above tonic is the note
      my_program = chord_qualities[root_diff];
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
    Serial.println(curr_voice);

    if (curr_voice != -1) {  // if any voice is free, play note
      sample[curr_voice].setTable(samples_list[my_program]);
      sample[curr_voice].setEnd(samples_lengths[my_program]);

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
      if (port_length == 0 || voiceQueue.size() == 0) {  // check if we need to apply portamento
        myNote->port_done = true;
        myNote->port_end_time = 0;
      } else {
        myNote->port_done = false;
        myNote->port_end_time = millis() + port_length;
        portamento[curr_voice].setTime(0);
        portamento[curr_voice].start(last_port_note);
        portamento[curr_voice].next();  // jump this voice to the prev note pitch, this is really hacky
        portamento[curr_voice].next();
        portamento[curr_voice].setTime(port_length);
        portamento[curr_voice].start(float_to_Q16n16(sample_freq[curr_voice]));
      }

      last_port_note = float_to_Q16n16(sample_freq[curr_voice]);  // set this as last note played for portamento
      voiceQueue.add(myNote);                                     // add to active notes queue
    }
  } else if (channel == MIDI_PAD_CHANNEL) {
    switch (note) {
      case 50:
        if (retrigger_mode == RT_QUARTER) {
          retrigger_mode = RT_NONE;
        } else {
          retrigger_mode = RT_QUARTER;
        }
        break;
      case 45:
        if (retrigger_mode == RT_EIGHTH) {
          retrigger_mode = RT_NONE;
        } else {
          retrigger_mode = RT_EIGHTH;
        }
        break;
      case 51:
        if (retrigger_mode == RT_SIXTEENTH) {
          retrigger_mode = RT_NONE;
        } else {
          retrigger_mode = RT_SIXTEENTH;
        }
        break;
      case 49:
        if (retrigger_mode == RT_THIRTYSECOND) {
          retrigger_mode = RT_NONE;
        } else {
          retrigger_mode = RT_THIRTYSECOND;
        }
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
  curr_program = program_num;

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

/**
* @brief To be called whenever a midi event is recieved.
*/
void midiEventHandler() {
  int channel = -1;
  int data1 = -1;
  int data2 = -1;
  switch (MIDI.getType())  // Get the type of the message we caught
  {
    case 1:  // ---------- MIDI NOTE ON RECEIVED ----------
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
          port_length = map(data2, 0, 127, 0, 1000);
          debugln(port_length);
          break;
        case 5:  //knob #7
          break;
        case 10:  //knob #8
          break;
        case 1:  // mod wheel
          break;
        case 20:  // repeat button
          break;
        case 21:  // rewind button
          break;
        case 22:  // fastforward buttton
          break;
        case 23:  // stop button
          MIDI.sendRealTime(midi::Stop);
          break;
        case 24:  // play button
          midi_step_num = 0;
          MIDI.sendRealTime(midi::Start);
          break;
        case 25:  // record button
          midi_step_num = 0;
          break;
      }
      break;
    case midi::PitchBend:
      {  // ---------- MIDI PITCH BEND RECEIVED ----------
        data1 = MIDI.getData1();
        data2 = MIDI.getData2();
        probemidi(data1);
        probemidi(" ");
        probemidi(data2);
        probemidi(" ");
        data2 -= 64;  // -64 to +63
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
          if (my_note->port_done) {
            sample[my_note->voice_num].setFreq(my_note->frequency * bend_amount);
          }
        }
        probemidi("pitchbend: ");
        probemidiln(data2);
        break;
      }
    case midi::Clock:  // ---------- MIDI CLOCK PULSE RECEIVED ----------
      probemidi("clock: ");
      probemidiln(midi_step_num);
      if (midi_clock_mode == CK_EXTERNAL) {
        clockStep();
      }
      break;
    case midi::Start:  // ---------- MIDI START MESSAGE RECEIVED ----------
      midi_step_num = 0;
      break;
  }
}

/**
* @brief Calculates portamento frequencies for all active voices. To be called in updateControl()
*/
void handlePortamento() {
  int queue_size = voiceQueue.size();
  int curr_time = millis();
  if (queue_size > 0) {
    MidiNoteVoice *myNote;
    // loop through voice queue searching for note
    for (int i = 0; i < queue_size; i++) {
      myNote = voiceQueue.get(i);
      if (myNote->port_done) {
        continue;
      }
      Q16n16 port_freq = portamento[myNote->voice_num].next();
      sample[myNote->voice_num].setFreq(Q16n16_to_float(port_freq));
      if (curr_time > myNote->port_end_time) {  // if portamento time is done, set note to landing frequency
        myNote->port_done = true;
        sample[myNote->voice_num].setFreq(myNote->frequency * bend_amount);
      }
    }
  }
}

/**
* @brief Checks if any currently playing patten notes need to end then ends and deletes them
*/
void cleanPatternQueue() {
  //freeing finished midi notes
  int my_queue_size = patternQueue.size();
  if (my_queue_size > 0) {
    bool entries_to_delete[my_queue_size];  // array to keep track of which entries need to be removed

    PatternNote *my_pattern_note;
    uint32_t my_time = millis();
    // end notes and tag them to be deleted
    for (int i = 0; i < my_queue_size; i++) {
      my_pattern_note = patternQueue.get(i);
      if (my_time > my_pattern_note->end_time) {
        // END THE NOTE HERE
        sample_env[my_pattern_note->voice_num].noteOff();  // turn off note
        entries_to_delete[i] = true;
      } else {
        entries_to_delete[i] = false;
      }
    }

    // delete tagged notes starting at end of array
    for (int i = my_queue_size - 1; i >= 0; i--) {
      if (entries_to_delete[i]) {
        delete (patternQueue.get(i));
        patternQueue.remove(i);
      }
    }
  }
}

/**
   * @brief Plays next step of a pattern
   */
void step(SamplePattern &my_pattern) {
  // find note that needs to be played
  uint32_t curr_time = millis();
  uint8_t note = my_pattern.midi_notes[my_pattern.curr_step];
  uint32_t my_end_time = curr_time + my_pattern.note_sustain[my_pattern.curr_step];
  // my_pattern.next_step_time = curr_time + my_pattern.step_duration[my_pattern.curr_step];
  my_pattern.sclock = 0;
  my_pattern.next_step_time = my_pattern.step_duration[my_pattern.curr_step];


  //find a free voice
  int my_voice = -1;
  for (int i = 0; i < POLYPHONY; i++) {
    if (!sample_env[i].playing()) {
      sample_active[i] = true;
      my_voice = i;
      break;
    }
  }

  if (my_voice != -1) {  // if any voice is free, play note
    sample[my_voice].setTable(samples_list[my_pattern.sample_num]);
    sample[my_voice].setEnd(samples_lengths[my_pattern.sample_num]);

    // calculate frequency offset for sample
    float diff = note - 60;
    sample_freq[my_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample

    // start the note
    sample[my_voice].setFreq(sample_freq[my_voice]);  // set frequency of sample
    sample_env[my_voice].noteOn();
    sample[my_voice].start();


    PatternNote *my_pattern_note = new PatternNote();
    my_pattern_note->note_num = note;
    my_pattern_note->voice_num = my_voice;
    my_pattern_note->channel_num = my_pattern.channel_num;
    my_pattern_note->end_time = my_end_time;

    patternQueue.add(my_pattern_note);
  }
  my_pattern.incrementStep();
}