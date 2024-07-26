/*
  Sampler half of MARNIE patch for 4 years of fish

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
  MARNIE Switch 2:
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

#include <Meap.h>
#include "MySample.h"
#include <LinkedList.h>
#include "sample_includes.h"
#include "MyPortamento.h"
#include "SamplePattern.h"
#include "patterns.h"
#include "MyPDResonant.h"

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable

#define NUM_PROGRAMS 35

#define DEBUG 1
#define PROBEMIDI 1

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
#define MIDI_OUT_CHANNEL 5  // out to cz101

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


int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[6] = { 0, 0, 0, 0, 0, 0 };
int last_button_vals[6] = { 0, 0, 0, 0, 0, 0 };

Meap meap;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Sample voices
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;
float sample_freq[POLYPHONY];
bool sample_active[POLYPHONY] = { false };
int curr_voice = 0;  // voice from sample[] array being used

int curr_program = 0;  // current midi program, primarily used to choose sample

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
int retrigger_mode = 0;  // 0 = none, 1 =

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

// pattern stuff

int curr_pattern_bank = 0;

// digital synth voices
MyPDResonant pd_voice;
Portamento<CONTROL_RATE> pdPortamento;

// for easing timing of pattern starting
uint32_t last_sixteenth_time = 0;     /**< keeps track of the time in ms that the last 16th note happened */
uint32_t half_sixteenth_length = 100; /**< half of the period in ms of one sixteenth note */


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

  // initialize tempo to 120 BPM
  midi_micros = meap.midiPulseMicros(120);
  half_sixteenth_length = midi_micros * 3 / 1000;          // for fixing timing on pattern triggering with touch pads

  // set up organ switch pins
  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  //digital voice setup
  pdPortamento.setTime(500);

  patternSetup();
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
  meap.readAuxMux();
  updateButtons();

  // MARNIE Pot Controls
  cutoff = map(meap.aux_mux_vals[4], 0, 4095, 0, 255);
  filter.setCutoffFreq(cutoff);

  resonance = map(meap.aux_mux_vals[6], 0, 4095, 0, 255);
  filter.setResonance(resonance);

  attack_time = meap.aux_mux_vals[5];
  release_time = meap.aux_mux_vals[7];

  // update portamento
  port_length = map(meap.aux_mux_vals[2], 0, 4095, 0, 1000);
  // pdPortamento.setTime(port_length);
  handlePortamento();

  // update rand cutoff range
  rand_cutoff_range = map(meap.aux_mux_vals[3], 0, 4095, 0, 127);
  // if (rand_cutoff_range <= 2) {
  //   rand_cutoff_range = 0;
  // }


  // update amplitude envelopes
  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }

  // read tempo from pot 0
  midi_micros = meap.midiPulseMicros((int)map(meap.pot_vals[0], 0, 4095, 80, 160));
  half_sixteenth_length = midi_micros * 3 / 1000;          // for fixing timing on pattern triggering with touch pads

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

  // update PDresonant voice
  // pd_voice.freq = (int)Q16n16_to_float(pdPortamento.next());// fixed to int
  // pd_voice.freq = mtof((int)Q16n16_to_float(pdPortamento.next()));
  // Q16n16 my_pd_freq;
  // my_pd_freq = pdPortamento.next();
  // float my_float_freq = Q16n16_to_float(my_pd_freq);

  // debug(my_float_freq);
  // debug(" ");
  // debugln(mtof(my_float_freq));

  // pd_voice.noteUpdate((int)my_float_freq);
  pd_voice.update();

  // ends notes triggered by patterns
  cleanPatternQueue();
}


AudioOutput_t updateAudio() {
  int out_sample = 0;
  // add all samples to envelope
  for (int i = 0; i < POLYPHONY; i++) {
    sample_gain[i] = sample_env[i].next();
    out_sample += sample[i].next() * sample_gain[i];
  }
  //add pd voice
  out_sample = out_sample + (pd_voice.next() << 8);  // calculate pd output and shift up to match volume of samples
  // send output through filter
  filter.next(out_sample);
  int filtered_out;
  if (meap.dip_vals[7]) {
    filtered_out = filter.band();
  } else {
    filtered_out = filter.low();
  }
  //send output to DAC
  return StereoOutput::fromAlmostNBit(19, filtered_out, filtered_out);
}

void Meap::updateTouch(int pad_num, bool pressed) {
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
        midi_clock_mode = CK_EXTERNAL;
      } else {  // DIP 1 down
        midi_clock_mode = CK_INTERNAL;
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
        case 2:
          if (button_vals[i]) {  // button 2 pressed
            debugln("B2 pressed");
          } else {  // button 2 released
            debugln("B2 released");
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
    last_sixteenth_time = millis();
    if (meap.irand(0, 4) == 0) {
      randomizeCutoff(rand_cutoff_range);
    }
    if (retrigger_mode == RT_SIXTEENTH) {
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
    if (retrigger_mode == RT_THIRTYSECOND) {
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
  if (range > 2) {
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
    if (curr_program < NUM_SAMPLES) {  // play a sample
      // choose voice to play
      int my_program = curr_program;
      if (pgm_override = !-1) {
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
      debugln(curr_voice);

      if (curr_voice != -1) {  // if any voice is free, play the note, if not don't play it!
        sample[curr_voice].setTable(samples_list[my_program]);
        sample[curr_voice].setEnd(samples_lengths[my_program]);

        // calculate frequency offset for sample
        float diff = note - 60;
        sample_freq[curr_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample

        // update the envelope
        sample_env[curr_voice].setAttackTime(map(attack_time, 0, 4095, 1, 2000));
        sample_env[curr_voice].setReleaseTime(map(release_time, 0, 4095, 1, 2000));

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
    } else if (curr_program == NUM_SAMPLES) {  // pd resonant voice
      pd_voice.setPDEnv(attack_time, attack_time);
      // debugln((uint8_t)note);
      // if(voiceQueue.size() > 0){
      //   pdPortamento.setTime(port_length);
      // } else{
      //   pdPortamento.setTime(0);
      // }

      // pdPortamento.start((uint8_t)note);
      // pd_voice.noteOn(1, note, 127);  // channel has no effect here
      // Q16n16 my_pd_freq;
      // my_pd_freq = pdPortamento.next();
      // float my_float_freq = Q16n16_to_float(my_pd_freq);

      pd_voice.noteOn(1, note, 127);  // channel has no effect here
    }
  }

  if (channel == MIDI_PAD_CHANNEL) {  // HANDLING PADS FRROM MIDI KEYBOARD
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
    } else if (curr_program == NUM_SAMPLES) {
      pd_voice.noteOff(channel, note, 0);
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
          // cutoff_flag = true;
          break;
        case 74:  //knob #2
          // resonance = data2;
          // resonance_flag = true;
          break;
        case 84:  //knob #3
          // attack_time = data2;
          // attack_flag = true;
          break;
        case 7:  //knob #4
          // release_time = data2;
          // release_flag = true;
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
          }
          break;
        case 1:  // mod wheel
          break;
        case 20:  // repeat button -- flush all notes
          flushPatternNotes();
          flushKeyboardNotes();
          break;
        case 21:  // rewind button -- STOP sampler patterns
          for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
            for (int i = 0; i < 8; i++) {
              patterns[bank][i].stop();
            }
          }
          break;
        case 22:  // fastforward buttton
          for (int i = 0; i < 8; i++) {
            if (pattern_auto_start[curr_pattern_bank][i]) {
              patterns[curr_pattern_bank][i].start();
            }
          }
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
      float diff = note - 60;
      sample_freq[my_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample

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
  //turns off PD digital synth voice
  pd_voice.noteOff(0, 0, 0);
}