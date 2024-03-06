/*
  Sampler half of MARNIE patch for inchworm race at sunrise album release show

  MEAP Pot 0:
  MEAP Pot 1:

  DIP 1: UP = external clock |||| DOWN = internal clock
  DIP 2:
  DIP 3: 
  DIP 4:
  DIP 5: 
  DIP 6:
  DIP 7:
  DIP 8:

  MARNIE Pot 0:
  MARNIE Pot 1:
  MARNIE Pot 2: Portamento Range
  MARNIE Pot 3: Cutoff Randomization Range

  MARNIE Pot 4: Filter Cutoff
  MARNIE Pot 5: Amplitude Envelope Attack
  MARNIE Pot 6: Filter Resonance
  MARNIE Pot 7: Amplitude Envelope Release
  
  MARNIE Switch 0:
  MARNIE Switch 1:
  MARNIE Switch 2: enable MIDI keyboard -> Glockenspiel
  MARNIE Switch 3: RESTING = sample looping off |||| ENGAGED = sample looping on 
  MARNIE Switch 4:
  MARNIE Switch 5:

  MIDI CONTROLLER KNOB 1 (cc71):
  MIDI CONTROLLER KNOB 2 (cc74):
  MIDI CONTROLLER KNOB 3 (cc84):
  MIDI CONTROLLER KNOB 4 (cc7):
  MIDI CONTROLLER KNOB 5 (cc91):
  MIDI CONTROLLER KNOB 6 (cc93):
  MIDI CONTROLLER KNOB 7 (cc5):  Change program (chooses sample)
  MIDI CONTROLLER KNOB 8 (cc10): Change pattern bank
  MIDI Repeat Button: Stop all sampler patterns
  MIDI Rewind Button: Flush all MIDI notes
  MIDI Start Button/Message:
  MIDI Stop Button/Message:
  MIDI Record Button:
  

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <Mux.h>
#include <MIDI.h>
#include <ADSR.h>
#include "MySample.h"
#include <LinkedList.h>
#include <Meap.h>
#include "sample_includes.h"
#include <ResonantFilter.h>
#include <Portamento.h>
#include "MyPortamento.h"
#include "SamplePattern.h"
#include "patterns.h"
#include "sample_frequencies.h"  // NEED TO RECALCULATE THESE VALUES IF DIFF SAMPLE SR OR MAX LENGTH IS USED

#define DEBUG 1
#define PROBEMIDI 0

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

#define CONTROL_RATE 64           // Hz, powers of 2 are most reliable
#define NUM_PROGRAMS 34           // Total number of programs
#define POLYPHONY 35              // How many voices
#define MAX_SAMPLE_LENGTH 300000  // Max length of a sample
#define MIDI_NOTE_CHANNEL 1       // Channel to listen for MIDI note messages
#define MIDI_PAD_CHANNEL 10       // Channel that M-audio interface sends pad messages on

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} midi_clock_mode;


enum RetrigModes {
  kNONE,
  kQUARTER,
  kEIGHTH,
  kSIXTEENTH,
  kTHIRTYSECOND,
  kSIXTYFOURTH
} retrigger_mode;


/**
* Reading the extra organ switch inputs
*/
int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[6] = { 0, 0, 0, 0, 0, 0 };
int last_button_vals[6] = { 0, 0, 0, 0, 0, 0 };

Meap meap; /**< object containing all meap functions*/

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); /**< MEAP hardware serial port*/

// Sample voices
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;                                         //default freq of a sample, calculated using length of sample and sample rate
float sample_freq[POLYPHONY];                               /**< stores current frequency of each sample voice */
bool sample_active[POLYPHONY] = { false };                  /**< keeps track of whether a sample voice is playing.  */
int curr_voice = 0;                                         // voice from sample[] array being used

int curr_program = 0;  // current midi program, primarily used to choose sample

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY]; /**< ADSR envelopes for all sample voices */
int sample_gain[POLYPHONY];
int attack_time = 1;
int release_time = 500;

MyPortamento<CONTROL_RATE> portamento[POLYPHONY];

float voice_pan[POLYPHONY] = { 0 };  // keeps track of panning per voice

// for easing timing of pattern starting
uint32_t last_sixteenth_time = 0;     /**< keeps track of the time in ms that the last 16th note happened */
uint32_t half_sixteenth_length = 100; /**< half of the period in ms of one sixteenth note */

/**
* Keeps track of a single note turned on by a midi message
*/
class MidiNoteVoice {
public:
  int note_num;           /**< MIDI note number */
  int voice_num;          /**< Num of sample voice sounding this note */
  float frequency;        /**< Frequency of note, used for portamento */
  bool port_done;         /**< True when portamento is done, False while portamento is sliding to new note */
  uint32_t port_end_time; /**< End time of portamento slide */
};

/**
* Keeps track of a single note turned on by a SamplePattern object
*/
class PatternNote {
public:
  uint8_t note_num;    /**< MIDI note number */
  uint16_t voice_num;  /**< Num of sample voice sounding this note */
  uint8_t channel_num; /**< MIDI channel number to output on */
  uint32_t end_time;   /**< Time to end the note automatically */
};

LinkedList<MidiNoteVoice *> voiceQueue = LinkedList<MidiNoteVoice *>();

LinkedList<PatternNote *> patternQueue = LinkedList<PatternNote *>();


MultiResonantFilter<uint8_t> filter;  /**< Filter for left channel */
MultiResonantFilter<uint8_t> filter2; /**< Filter for right channel */
int cutoff = 255;
int resonance = 127;
int rand_cutoff_range = 0;

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 10000;
int midi_step_num = 0;
// int midi_clock_mode = kINTERNAL;

// pitchbend
int pitchbend_semitones = 2;  // num of semitones the pitchbend wheel bends
float bend_amount = 1;

//portamento
uint16_t port_length = 0;  // length of portamento in milliseconds
Q16n16 last_port_note = 0;

// guitar chord voice
int chord_tonic = -1;
int chord_qualities[12] = { 7, 9, 8, 10, 8, 7, 9, 7, 9, 8, 7, 9 };  // 7=maj 8=min 9=dim 10=aug, chords present in a major scale (with chormatic stuff filled in for accidentals)

// pattern stuff
int curr_pattern_bank = 0;

// midi glockenspiel
bool glock_enable = false;
int glock_channel = 16;  // sends notes out on channel 16 to connect to my MIDI glockenspiel


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  // initialize samples
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

  // initialize filter
  filter.setCutoffFreq(cutoff);
  filter.setResonance(resonance);
  filter2.setCutoffFreq(cutoff);
  filter2.setResonance(resonance);

  // initialize tempo to 80 BPM
  midi_micros = meap.midiPulseMicros(80);
  half_sixteenth_length = midi_micros * 3 / 1000;

  // set up organ switch pins
  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  //digital voice setup
  // pdPortamento.setTime(500);

  patternSetup();
}


void loop() {
  audioHook();
  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }

  // handle generating midi clock
  if (midi_clock_mode == kINTERNAL) {
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
  meap.readAuxMux();
  updateButtons();

  // MARNIE Pot Controls
  //1 pole smoothing here
  float alpha = 0.82;
  int prev_cutoff = cutoff;
  cutoff = (alpha)*cutoff + (1.0 - alpha) * map(meap.auxMuxVals[4], 0, 4095, 0, 255);
  if (prev_cutoff != cutoff) {
    filter.setCutoffFreq(cutoff);
    filter2.setCutoffFreq(cutoff);
  }


  resonance = map(meap.auxMuxVals[6], 0, 4095, 0, 255);
  filter.setResonance(resonance);
  filter2.setResonance(resonance);

  attack_time = meap.auxMuxVals[5];
  release_time = meap.auxMuxVals[7];

  // update portamento
  port_length = map(meap.auxMuxVals[2], 0, 4095, 0, 1000);
  handlePortamento();

  // update rand cutoff range
  rand_cutoff_range = map(meap.auxMuxVals[3], 0, 4095, 0, 127);


  // update amplitude envelopes
  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }

  // read tempo from pot 0 -- replace this with reading tempo from pattern_tempo array
  // midi_micros = meap.midiPulseMicros((int)map(meap.potVals[0], 0, 4095, 80, 160));

  //handle pattern array
  for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
    for (int i = 0; i < 8; i++) {
      patterns[bank][i].cycle();
      if (patterns[bank][i].step_flag) {
        patterns[bank][i].step_flag = false;
        step(patterns[bank][i]);
      }
    }
  }

  // ends notes triggered by patterns
  cleanPatternQueue();
}


AudioOutput_t updateAudio() {
  int32_t l_sample = 0;
  int32_t r_sample = 0;

  int32_t immediate_sample = 0;
  StereoSample pan_result;
  // add all samples to envelope
  for (int i = 0; i < POLYPHONY; i++) {
    if (sample_env[i].playing()) {
      sample_gain[i] = sample_env[i].next();
      immediate_sample = sample[i].next() * sample_gain[i];
      //apply panning algorithm

      if (voice_pan[i] == 0) {
        l_sample += immediate_sample;
        r_sample += immediate_sample;
      } else if (voice_pan[i] == -1) {
        l_sample += immediate_sample;
      } else if (voice_pan[i] == 1) {
        r_sample += immediate_sample;
      } else {
        l_sample += immediate_sample;
        r_sample += immediate_sample;
      }
    }
  }


  // send output through filter
  filter.next(l_sample);
  filter2.next(r_sample);
  l_sample = filter.low();
  r_sample = filter2.low();
  // //send output to DAC
  return StereoOutput::fromAlmostNBit(19, l_sample, r_sample);
}


void Meap::updateTouch(int pad_num, bool pressed) {

  // if pressed
  // check how close we are to nearest sixteenth note,,
  // if the nearest sixteenth note is in the future, proceed as normal
  // however if the nearest sixteenth is in the past, we need to play the first step
  // and get ready to play the next step on the next sixteenth
  if (pressed) {
    if (patterns[curr_pattern_bank][pad_num].active) {
      patterns[curr_pattern_bank][pad_num].stop();
    } else {
      patterns[curr_pattern_bank][pad_num].start();
      // check closest sixteenth note here! using ppq???? or maybe i store time with millis or micros?
      uint32_t curr_time = millis();
      if (curr_time > last_sixteenth_time && curr_time < last_sixteenth_time + half_sixteenth_length) {
        debugln("pressed late, accomodating!");
        //closest sixteenth is in the past,, do something here!!!
        // use step function
        patterns[curr_pattern_bank][pad_num].start_flag = false;
        step(patterns[curr_pattern_bank][pad_num]);
      }
    }
  }

  switch (pad_num) {
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
        midi_clock_mode = kEXTERNAL;
      } else {  // DIP 1 down
        midi_clock_mode = kINTERNAL;
      }
      break;
    case 1:
      if (up) {  // DIP 2 up

      } else {  // DIP 2 down
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
      } else {   // DIP 3 down
      }
      break;
    case 3:
      if (up) {  // DIP 4 up

      } else {  // DIP 4 down
      }
      break;
    case 4:
      if (up) {  // DIP 5 up

      } else {  // DIP 5 down
      }
      break;
    case 5:
      if (up) {  // DIP 6 up

      } else {  // DIP 6 down
      }
      break;
    case 6:
      if (up) {  // DIP 7 up

      } else {  // DIP 7 down
      }
      break;
    case 7:
      if (up) {  // DIP 8 up

      } else {  // DIP 8 down
      }
      break;
  }
}

void updateButtons() {
  for (int i = 0; i < 6; i++) {
    // Read the buttons
    button_vals[i] = digitalRead(button_pins[i]);
    // Check if state has changed
    if (button_vals[i] != last_button_vals[i]) {
      switch (i) {
        case 0:
          if (button_vals[i]) {  // button 0 pressed
            debugln("B0 pressed");
          } else {  // button 0 released
            debugln("B0 released");
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 pressed
            debugln("B1 pressed");
          } else {  // button 1 released
            debugln("B1 released");
          }
          break;
        case 2:                  // glockenspiel enable
          if (button_vals[i]) {  // button 2 pressed
            debugln("B2 pressed");
            glock_enable = true;
          } else {  // button 2 released
            debugln("B2 released");
            glock_enable = false;
          }
          break;
        case 3:                  // SAMPLE LOOPING MODE
          if (button_vals[i]) {  // button 3 pressed
            debugln("B3 pressed");
            for (int i = 0; i < POLYPHONY; i++) {
              sample[i].setLoopingOn();
            }
          } else {  // button 3 released
            debugln("B3 released");
            for (int i = 0; i < POLYPHONY; i++) {
              sample[i].setLoopingOff();
            }
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 pressed
            debugln("B4 pressed");
          } else {  // button 4 released
            debugln("B4 released");
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 pressed
            debugln("B5 pressed");
          } else {  // button 5 released
            debugln("B5 released");
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
  }
}


//executes when a clock step is received
void clockStep() {

  if ( =% 24 == 0) {  // quarter note
    if (retrigger_mode == kQUARTER) {
      retriggerNotes(true);
    }
  }

  if (midi_step_num % 12 == 0) {  // eighth note
    randomizeCutoff(rand_cutoff_range);
    // debugln(rand_cutoff_range);
    if (retrigger_mode == kEIGHTH) {
      retriggerNotes(true);
    }
    if (meap.auxMuxVals[0] > 30) {
      // debug("glockval: ");
      // debugln(meap.auxMuxVals[0]);
      int notes[] = { 67, 72, 76 };
      int index = meap.irand(map(meap.auxMuxVals[0], 0, 4095, -50, 0), 2);
      int my_note = notes[index];
      if (meap.irand(0, 1) == 1) {
        my_note -= 12;
      }
      if (index >= 0) {
        // debugln("glock_trig");
        noteOnHandler(my_note, 127, MIDI_NOTE_CHANNEL, 3);  // note vel channel override ---- NOTE THAT IF RELEASE IS DOWN THIS NOTE WILL NOT SOUND BECAUSE NOTE OFF IS SO SOON
        MIDI.sendNoteOn(my_note, 127, 16);                  // send midi note to glockenspiel
        noteOffHandler(my_note, MIDI_NOTE_CHANNEL);         // note channel
      }
    }
  }

  if (midi_step_num % 6 == 0) {  // sixteenth note
    last_sixteenth_time = millis();
    if (meap.irand(0, 4) == 0) {
      randomizeCutoff(rand_cutoff_range);
    }
    if (retrigger_mode == kSIXTEENTH) {
      retriggerNotes(true);
    }

    //send triggers to patterns
    for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
      for (int i = 0; i < 8; i++) {
        patterns[bank][i].incrementClock(midi_step_num);
      }
    }
  }

  if (midi_step_num % 3 == 0) {  // thirtysecond note
    if (retrigger_mode == kTHIRTYSECOND) {
      retriggerNotes(true);
    }
  }

  midi_step_num = (midi_step_num + 1) % 24;
}

/**
* @brief Randomizes cutoff of filter.
*
* @param range is the range to randomize filter over in "percentage"
*/
void randomizeCutoff(int range) {
  if (range > 3) {  // add a slight deadzone at the bottom of the pot
    debugln("cutoff rand");
    // range = range << 1;  // maps input range to 0-200
    int rand_cutoff = cutoff + meap.irand(-range, range);
    // clip to range 0-255
    if (rand_cutoff > 255) {
      rand_cutoff = 255;
    } else if (rand_cutoff < 0) {
      rand_cutoff = 0;
    }
    filter.setCutoffFreq(rand_cutoff);
    filter2.setCutoffFreq(rand_cutoff);
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
  if (channel == MIDI_NOTE_CHANNEL) {  // check on which channel we received the MIDI noteOn
    // send notes to midi glockenspiel
    if (glock_enable) {
      MIDI.sendNoteOn(note, velocity, glock_channel);
    }
    if (curr_program < NUM_SAMPLES) {  // play a sample
      // choose voice to play
      int my_program = curr_program;
      if (pgm_override != -1) {
        my_program = pgm_override;
      }
      // handle choosing chords for the guitar chord voice
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
      // find a free voice
      curr_voice = -1;
      for (int i = 0; i < POLYPHONY; i++) {
        if (!sample_env[i].playing()) {
          sample_active[i] = true;
          curr_voice = i;
          break;
        }
      }
      debug("voice num: ");
      debugln(curr_voice);

      if (curr_voice != -1) {  // if any voice is free, play the note, if not don't play it!
        sample[curr_voice].setTable(samples_list[my_program]);
        sample[curr_voice].setEnd(samples_lengths[my_program]);

        // calculate frequency offset for sample
        // float diff = note - 60;
        // sample_freq[curr_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample
        sample_freq[curr_voice] = sample_freq_table[note];

        // update the envelope
        sample_env[curr_voice].setAttackTime(map(attack_time, 0, 4095, 1, 2000));
        sample_env[curr_voice].setReleaseTime(map(release_time, 0, 4095, 1, 2000));

        // update panning ... for now all keyboard notes are right down the center, maybe add a knob that will place them randomly??
        voice_pan[curr_voice] = 0;

        // start the note
        sample[curr_voice].setFreq(sample_freq[curr_voice] * bend_amount);  // set frequency of sample
        sample_env[curr_voice].noteOn();
        sample[curr_voice].start();



        // keep track of note in active notes queue
        MidiNoteVoice *myNote = new MidiNoteVoice();  // create object to keep track of note num, voice num and freq
        myNote->note_num = note;
        myNote->voice_num = curr_voice;
        myNote->frequency = sample_freq[curr_voice];
        if (port_length == 0 || voiceQueue.size() == 0) {  // don't apply portamento if port time is zero or if no notes are active
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
    }
  }

  if (channel == MIDI_PAD_CHANNEL) {  // HANDLING PADS FRROM MIDI KEYBOARD
    switch (note) {
      case 50:
        if (retrigger_mode == kQUARTER) {
          retrigger_mode = kNONE;
        } else {
          retrigger_mode = kQUARTER;
        }
        break;
      case 45:
        if (retrigger_mode == kEIGHTH) {
          retrigger_mode = kNONE;
        } else {
          retrigger_mode = kEIGHTH;
        }
        break;
      case 51:
        if (retrigger_mode == kSIXTEENTH) {
          retrigger_mode = kNONE;
        } else {
          retrigger_mode = kSIXTEENTH;
        }
        break;
      case 49:
        if (retrigger_mode == kTHIRTYSECOND) {
          retrigger_mode = kNONE;
        } else {
          retrigger_mode = kTHIRTYSECOND;
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
    if (curr_program < NUM_SAMPLES) {
      int queue_size = voiceQueue.size();
      if (queue_size > 0) {
        bool entries_to_delete[queue_size];
        MidiNoteVoice *myNote;
        // loop through voice queue searching for note
        for (int i = 0; i < queue_size; i++) {
          myNote = voiceQueue.get(i);
          if (note == myNote->note_num) {
            sample_env[myNote->voice_num].noteOff();   // turn off note
            sample_active[myNote->voice_num] = false;  // free up voice
            // delete (myNote);
            entries_to_delete[i] = true;
          } else {
            entries_to_delete[i] = false;
          }
        }

        // delete tagged notes starting at end of array
        for (int i = queue_size - 1; i >= 0; i--) {
          if (entries_to_delete[i]) {
            delete (voiceQueue.get(i));
            voiceQueue.remove(i);
          }
        }
      }
    }
  }

  if (channel == MIDI_PAD_CHANNEL) {
  }
}

/**
* @brief Handles changing MIDI programs
*
* @param program_num is the MIDI program to change to
*/
void programChangeHandler(int program_num) {
  //GENERIC SAMPLE CHANGE
  if (program_num >= 0 && program_num < NUM_PROGRAMS) {
    curr_program = program_num;  // set current program to received program

    switch (program_num) {  // do we need to do anything for specific programs
      case 7:               // Guitar chord nonsense
        chord_tonic = -1;
        break;
    }
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
    case midi::NoteOn:  // ---------- MIDI NOTE ON RECEIVED ----------
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
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
        channel = MIDI.getChannel();
        data1 = MIDI.getData1();  // note
        probemidi("Note Off: ");
        probemidi("note: ");
        probemidiln(data1);
        noteOffHandler(data1, channel);
        break;
    case midi::ProgramChange:   // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      data1 = MIDI.getData1();  // program number
      probemidi("Pgm: ");
      probemidiln(data1);
      // programChangeHandler(data1);
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
          // cutoff = data2;
          break;
        case 74:  //knob #2
          // resonance = data2;
          break;
        case 84:  //knob #3
          // attack_time = data2;

          break;
        case 7:  //knob #4
          // release_time = data2;
          break;
        case 91:  // knob 5
          // rand_cutoff_range = data2;
          break;
        case 93:  //knob #6
          // port_length = map(data2, 0, 127, 0, 1000);
          // debugln(port_length);
          break;
        case 5:                         //knob #7
          programChangeHandler(data2);  // change program
          break;
        case 10:  //knob #8 -- pattern bank
          if (data2 >= 0 && data2 < NUM_PATTERN_BANKS) {
            curr_pattern_bank = data2;
            midi_micros = meap.midiPulseMicros(bank_tempos[data2]);  // update tempo to song tempo stored in patterns.h
            half_sixteenth_length = midi_micros * 3 / 1000;          // for fixing timing on pattern triggering with touch pads
          }
          break;
        case 1:  // mod wheel
          break;
        case 20:  // repeat button -- STOP sampler patterns
          for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
            for (int i = 0; i < 8; i++) {
              patterns[bank][i].stop();
            }
          }
          break;
        case 21:  // rewind button -- flush all midi notes
          flushPatternNotes();
          flushKeyboardNotes();
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
      if (midi_clock_mode == kEXTERNAL) {
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
* @brief Checks if any currently playing pattern notes need to end then ends and deletes them
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
        //turn off midi note
        int8_t pattern_midi_channel = my_pattern_note->channel_num;
        MIDI.sendNoteOff(my_pattern_note->voice_num, 0, pattern_midi_channel);
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
  uint8_t note = my_pattern.note[my_pattern.curr_step];
  uint8_t velocity = my_pattern.amp[my_pattern.curr_step];
  uint32_t my_end_time = curr_time + my_pattern.sus[my_pattern.curr_step];
  // my_pattern.next_step_time = curr_time + my_pattern.dur[my_pattern.curr_step];
  my_pattern.sclock = 0;
  my_pattern.next_step_time = my_pattern.dur[my_pattern.curr_step];

  if (note != 0) {

    //send a midi message
    int8_t pattern_midi_channel = my_pattern.getMidiChannel();
    if (pattern_midi_channel >= 0) {
      debug("midi out: n:");
      debug(note);
      debug(" v: ");
      debug(velocity);
      debug(" ch: ");
      debugln(pattern_midi_channel);
      MIDI.sendNoteOn(note, velocity, pattern_midi_channel);
    }

    if (my_pattern.sample_num != -1) {
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
        sample_env[my_voice].setAttackTime(map(attack_time, 0, 4095, 1, 2000));
        sample_env[my_voice].setReleaseTime(map(release_time, 0, 4095, 1, 2000));

        // calculate frequency offset for sample
        // float diff = note - 60;
        // sample_freq[my_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample
        sample_freq[my_voice] = sample_freq_table[note];

        // update panning
        voice_pan[my_voice] = my_pattern.pan;

        // start the note
        sample[my_voice].setFreq(sample_freq[my_voice]);  // set frequency of sample
        sample_env[my_voice].noteOn();
        sample[my_voice].start();


        PatternNote *my_pattern_note = new PatternNote();
        my_pattern_note->note_num = note;
        my_pattern_note->voice_num = my_voice;
        my_pattern_note->channel_num = my_pattern.getMidiChannel();
        my_pattern_note->end_time = my_end_time;

        patternQueue.add(my_pattern_note);
      }
    }
  }

  my_pattern.incrementStep();
}

/**
   * @brief Flushes all currently playing notes from pattern queue
   */
void flushPatternNotes() {
  PatternNote *my_pattern_note;
  // delete tagged notes starting at end of array
  int my_queue_size = patternQueue.size();
  for (int i = my_queue_size - 1; i >= 0; i--) {
    my_pattern_note = patternQueue.get(i);
    sample_env[my_pattern_note->voice_num].noteOff();
    int8_t pattern_midi_channel = my_pattern_note->channel_num;
    MIDI.sendNoteOff(my_pattern_note->voice_num, 0, pattern_midi_channel);
    delete (patternQueue.get(i));
    patternQueue.remove(i);
  }
}

/**
* @brief Flushes all currently playing keyboard notes
*/
void flushKeyboardNotes() {
  MidiNoteVoice *my_keyboard_note;
  int my_queue_size = voiceQueue.size();

  // delete tagged notes starting at end of array
  if (my_queue_size > 0) {
    for (int i = my_queue_size - 1; i >= 0; i--) {
      my_keyboard_note = voiceQueue.get(i);
      sample_env[my_keyboard_note->voice_num].noteOff();
      sample_active[my_keyboard_note->voice_num] = false;
      delete (voiceQueue.get(i));
      voiceQueue.remove(i);
    }
  }
}