/*
  Sampling keyboard for right half of MARNIE

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
  MARNIE Switch 1: When engaged, lowers output sample rate by 1
  MARNIE Switch 2: RESTING = midi thru off ||| ENGAGED = midi thru on
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
// #include "MySample.h"
#include <LinkedList.h>
#include "sample_includes.h"
#include "MyPortamento.h"
#include "extras/sample_frequencies.h"  // NEED TO RECALCULATE THESE VALUES IF DIFF SAMPLE SR OR MAX LENGTH IS USED

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
#define POLYPHONY 8               // How many voices
#define MAX_SAMPLE_LENGTH 300000  // Max length of a sample
#define MIDI_NOTE_CHANNEL 1       // Channel to listen for MIDI note messages
#define MIDI_PAD_CHANNEL 10       // Channel that M-audio interface sends pad messages on

// PROGRAM INITIALIZATION

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} midi_clock_mode;

/**
* Reading the extra organ switch inputs
*/
int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[6] = { 0, 0, 0, 0, 0, 0 };
int last_button_vals[6] = { 0, 0, 0, 0, 0, 0 };

Meap meap; /**< object containing all meap functions*/

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); /**< MEAP hardware serial port*/

// Sample voices
mSample16<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc

float default_freq;           //default freq of a sample, calculated using length of sample and sample rate
float sample_freq[POLYPHONY]; /**< stores current frequency of each sample voice */
int curr_voice = 0;           // voice from sample[] array being used

int curr_program = 0;  // current midi program, primarily used to choose sample

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY]; /**< ADSR envelopes for all sample voices */
int sample_gain[POLYPHONY];
int attack_time = 1;
int release_time = 500;

MyPortamento<CONTROL_RATE> portamento[POLYPHONY];

uint8_t sample_pan[POLYPHONY];

/**
* Keeps track of a single note turned on by a midi message
*/
class ActiveMidiNote {
public:
  uint8_t note_num;       /**< MIDI note number */
  uint8_t velocity;       /**< MIDI note velocity*/
  uint8_t channel;        /**< MIDI note channel */
  uint16_t voice_num;     /**< Voice allocated to play this note */
  float frequency;        /**< Frequency of note, used for portamento */
  bool port_done;         /**< True when portamento is done, False while portamento is sliding to new note */
  uint32_t port_end_time; /**< End time of portamento slide */
  uint32_t note_end_time; /**< Time to automatically end note (mostly used for computer triggered notes)*/

  void newNote(uint8_t _note_num, uint8_t _velocity, uint8_t _channel, float _frequency, uint16_t _voice_num) {
    note_num = _note_num;
    velocity = _velocity;
    channel = _channel;
    frequency = _frequency;
    voice_num = _voice_num;
    note_end_time = 0;
  };

  void setEndTime(uint32_t _note_end_time) {
    note_end_time = _note_end_time;
  }

  void portSet(bool _port_done, uint32_t _port_end_time) {
    port_done = _port_done;
    port_end_time = _port_end_time;
  }

  bool finished() {
    if (millis() > note_end_time) {
      return true;
    } else {
      return false;
    }
  }
};


LinkedList<ActiveMidiNote *> humanNoteQueue = LinkedList<ActiveMidiNote *>();
LinkedList<ActiveMidiNote *> computerNoteQueue = LinkedList<ActiveMidiNote *>();

LinkedList<int16_t> voice_queue;

MultiResonantFilter<uint16_t> filter;  /**< Filter for left channel */
MultiResonantFilter<uint16_t> filter2; /**< Filter for right channel */
int cutoff = 255;
int resonance = 127;
int rand_cutoff_range = 0;

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 10000;
int midi_step_num = 0;

// pitchbend
int pitchbend_semitones = 2;  // num of semitones the pitchbend wheel bends
float bend_amount = 1;

//portamento
uint16_t port_length = 0;  // length of portamento in milliseconds
Q16n16 last_port_note = 0;

// guitar chord voice
int chord_tonic = -1;
int chord_qualities[12] = { 7, 9, 8, 10, 8, 7, 9, 7, 9, 8, 7, 9 };  // 7=maj 8=min 9=dim 10=aug, chords present in a major scale (with chormatic stuff filled in for accidentals)

int output_bits = 21;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();
  MIDI.turnThruOff();

  // initialize samples
  default_freq = (float)ambitone_SAMPLERATE / (float)MAX_SAMPLE_LENGTH;
  for (int i = 0; i < POLYPHONY; i++) {
    sample[i].setTable(ambitone_DATA);
    sample[i].setEnd(ambitone_NUM_CELLS);
    sample_freq[i] = default_freq;
    sample[i].setFreq(sample_freq[i]);
    sample_gain[i] = 0;
    sample_env[i].setAttackTime(attack_time);
    sample_env[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    sample_env[i].setReleaseTime(release_time);
    sample_env[i].setADLevels(255, 255);
    voice_queue.unshift(i);  // add all voices to voice queue
    sample_pan[i] = 127;
  }

  // initialize filter
  filter.setCutoffFreq(cutoff);
  filter.setResonance(resonance);
  filter2.setCutoffFreq(cutoff);
  filter2.setResonance(resonance);

  // initialize tempo to 80 BPM
  midi_micros = meap.midiPulseMicros(80);

  // set up organ switch pins
  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }
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
  // meap.readTouch();
  // meap.readDip();
  meap.readAuxMux();
  updateButtons();

  // updateComputerNotes();  // check if any computer triggered notes need to end

  // MARNIE Pot Controls
  //1 pole smoothing here
  float alpha = 0.82;
  int32_t prev_cutoff = cutoff;
  cutoff = (alpha)*cutoff + (1.0 - alpha) * (meap.aux_mux_vals[4] << 4);  // 4095 -> 65535

  if (prev_cutoff != cutoff) {
    filter.setCutoffFreq(cutoff);
    filter2.setCutoffFreq(cutoff);
  }


  resonance = meap.aux_mux_vals[6] << 4;  // 4095 -> 65535
  filter.setResonance(resonance);
  filter2.setResonance(resonance);

  attack_time = meap.aux_mux_vals[5];
  release_time = meap.aux_mux_vals[7];

  // update portamento
  port_length = meap.aux_mux_vals[2] >> 2;  // 4095 -> 1023
  handlePortamento();

  // update rand cutoff range
  rand_cutoff_range = meap.aux_mux_vals[3] >> 5;  // 4095 -> 127

  // update amplitude envelopes
  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }
}


AudioOutput_t updateAudio() {
  int32_t l_sample = 0;
  int32_t r_sample = 0;

  int32_t immediate_sample = 0;
  StereoSample pan_result;

  for (int i = 0; i < POLYPHONY; i++) {
    sample_gain[i] = sample_env[i].next();
    immediate_sample = sample[i].next() * sample_gain[i];
    // l_sample += immediate_sample;
    pan_result = meap.pan2(immediate_sample, sample_pan[i]);
    l_sample += pan_result.l;
    r_sample += pan_result.r;
  }


  // send output through filter
  filter.next(l_sample);
  filter2.next(r_sample);
  l_sample = filter.low();
  r_sample = filter2.low();
  // //send output to DAC
  return StereoOutput::fromNBit(output_bits + 8, l_sample, r_sample);
}


void Meap::updateTouch(int pad_num, bool pressed) {
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
            output_bits = 20;
          } else {  // button 1 released
            debugln("B1 released");
            output_bits = 21;
          }
          break;
        case 2:                  // glockenspiel enable
          if (button_vals[i]) {  // button 2 pressed
            debugln("B2 pressed");
            MIDI.turnThruOn();
          } else {  // button 2 released
            debugln("B2 released");
            MIDI.turnThruOff();
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
  }

  if (midi_step_num % 12 == 0) {  // eighth note
    randomizeCutoff(rand_cutoff_range);
  }

  if (midi_step_num % 6 == 0) {  // sixteenth note
    if (meap.irand(0, 4) == 0) {
      randomizeCutoff(rand_cutoff_range);
    }
  }

  if (midi_step_num % 3 == 0) {  // thirtysecond note
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
    // range = range << 1;  // maps input range to 0-200
    int rand_cutoff = cutoff + meap.irand(0, range);
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
    // choose voice to play
    int my_program = curr_program;
    if (pgm_override != -1) {
      my_program = pgm_override;
    }


    // find a free voice
    curr_voice = -1;
    if (voice_queue.size() > 0) {
      curr_voice = voice_queue.shift();  // remove head element and return it!
    } else {
      return;  // no free voices, move along...
    }

    sample[curr_voice].setTable(samples_list[my_program]);
    sample[curr_voice].setEnd(samples_lengths[my_program]);
    sample_freq[curr_voice] = sample_freq_table[note];

    // update the envelope
    sample_env[curr_voice].setAttackTime(attack_time >> 1 + 1);
    sample_env[curr_voice].setReleaseTime(release_time >> 1 + 1);

    // start the note
    sample[curr_voice].setFreq(sample_freq[curr_voice] * bend_amount);  // set frequency of sample
    sample_env[curr_voice].noteOn();
    sample[curr_voice].start();

    // pan it!
    int32_t my_reading = meap.pot_vals[0]>>4;
    sample_pan[curr_voice] = my_reading;

    // keep track of note in active notes queue
    ActiveMidiNote *myNote = new ActiveMidiNote();  // create object to keep track of note num, voice num and freq
    myNote->newNote(note, velocity, channel, sample_freq[curr_voice], curr_voice);


    if (port_length == 0 || humanNoteQueue.size() == 0) {  // don't apply portamento if port time is zero or if no notes are active
      myNote->portSet(true, 0);
    } else {
      myNote->portSet(false, millis() + port_length);
      portamento[curr_voice].setTime(0);
      portamento[curr_voice].start(last_port_note);
      portamento[curr_voice].next();  // jump this voice to the prev note pitch, this is really hacky
      portamento[curr_voice].next();
      portamento[curr_voice].setTime(port_length);
      portamento[curr_voice].start(float_to_Q16n16(sample_freq[curr_voice]));
    }

    last_port_note = float_to_Q16n16(sample_freq[curr_voice]);  // set this as last note played for portamento

    humanNoteQueue.add(myNote);  // add to active notes queue
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
    int num_active_notes = humanNoteQueue.size();
    ActiveMidiNote *myNote;
    // loop through voice queue searching for note
    for (int i = 0; i < num_active_notes; i++) {
      myNote = humanNoteQueue.get(i);
      if (note == myNote->note_num && channel == myNote->channel) {
        sample_env[myNote->voice_num].noteOff();  // turn off note
        voice_queue.add(myNote->voice_num);
        delete (myNote);
        humanNoteQueue.remove(i);
        return;
      }
    }
  }
}

/**
* @brief Handles changing MIDI programs
*
* @param program_num is the MIDI program to change to
*/
void programChangeHandler(int program_num) {
  //GENERIC SAMPLE CHANGE
  if (program_num >= 0 && program_num < NUM_SAMPLES) {
    curr_program = program_num;  // set current program to received program
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
          break;
        case 5:  //knob #7
          // programChangeHandler(data2);  // change program
          break;
        case 10:  //knob #8
          break;
        case 1:  // mod wheel
          break;
        case 20:  // big button
          break;
        case 21:  // rewind button -- flush all midi notes
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
        int queue_size = humanNoteQueue.size();
        ActiveMidiNote *my_note;
        for (int i = 0; i < queue_size; i++) {
          my_note = humanNoteQueue.get(i);
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
  int queue_size = humanNoteQueue.size();
  int curr_time = millis();
  if (queue_size > 0) {
    ActiveMidiNote *myNote;
    // loop through voice queue searching for note
    for (int i = 0; i < queue_size; i++) {
      myNote = humanNoteQueue.get(i);
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
* @brief Flushes all currently playing keyboard notes
*/
void flushKeyboardNotes() {
  ActiveMidiNote *my_keyboard_note;
  int my_queue_size = humanNoteQueue.size();

  // delete tagged notes starting at end of array
  if (my_queue_size > 0) {
    for (int i = my_queue_size - 1; i >= 0; i--) {
      my_keyboard_note = humanNoteQueue.get(i);
      sample_env[my_keyboard_note->voice_num].noteOff();
      delete (humanNoteQueue.get(i));
      humanNoteQueue.remove(i);
    }
  }
}

int constrainMIDI(int noteNum) {
  while (noteNum > 127) {
    noteNum -= 12;
  }
  while (noteNum < 0) {
    noteNum += 12;
  }
  return noteNum;
}

int chooseGuitarChord(int note) {
  if (chord_tonic == -1) {  // tonic hasn't been chosen yet, choose it now and don't play a note
    chord_tonic = note % 12;
    return -1;
  } else {  // tonic has been chosen, choose correct chord
    int mod_note = note % 12;
    if (mod_note < chord_tonic) {
      mod_note += 12;
    }
    int root_diff = mod_note - chord_tonic;  // how many semitones above tonic is the note
    return chord_qualities[root_diff];
  }
}

// ***********************************************

/**
* @brief Handles turning on sample notes.
*
* @param note is the MIDI note number to turn on
* @param velocity is the velocity of the MIDI note to turn on (NOT CURRENTLY USED)
* @param channel is the velocity of the MIDI note to turn on (NOT CURRENTLY USED)
* @param pgm_override overrides the current program to play note on specified program 
*/
void computerNoteOn(int note, int velocity, int channel, int pgm_override, uint32_t note_length) {
  if (channel == MIDI_NOTE_CHANNEL) {  // check on which channel we received the MIDI noteOn

    // choose program to play
    int my_program = curr_program;
    if (pgm_override != -1) {
      my_program = pgm_override;
    }

    // handle choosing chords for the guitar chord program
    if (my_program >= 7 && my_program <= 10) {  // guitar chord program
      int tmp = chooseGuitarChord(note);
      if (tmp == -1) {
        return;
      } else {
        my_program = tmp;
      }
    }
    // find a free voice
    curr_voice = -1;
    for (int i = 0; i < POLYPHONY; i++) {
      if (!sample_env[i].playing()) {
        curr_voice = i;
        break;
      }
    }

    if (curr_voice != -1) {  // if any voice is free, play the note, if not don't play it!
      sample[curr_voice].setTable(samples_list[my_program]);
      sample[curr_voice].setEnd(samples_lengths[my_program]);
      sample_freq[curr_voice] = sample_freq_table[note];

      // update the envelope
      sample_env[curr_voice].setAttackTime(map(attack_time, 0, 4095, 1, 2000));
      sample_env[curr_voice].setReleaseTime(map(release_time, 0, 4095, 1, 2000));

      // start the note
      sample[curr_voice].setFreq(sample_freq[curr_voice] * bend_amount);  // set frequency of sample
      sample_env[curr_voice].noteOn();
      sample[curr_voice].start();

      // keep track of note in active notes queue
      ActiveMidiNote *myNote = new ActiveMidiNote();  // create object to keep track of note num, voice num and freq
      myNote->newNote(note, velocity, channel, sample_freq[curr_voice], curr_voice);
      myNote->setEndTime(millis() + note_length);


      // ATM computer notes do note factor into portamento in any way
      myNote->portSet(true, 0);
      // if (port_length == 0 || computerNoteQueue.size() == 0) {  // don't apply portamento if port time is zero or if no notes are active
      //   myNote->portSet(true, 0);
      // } else {
      //   myNote->portSet(false, millis() + port_length);
      //   portamento[curr_voice].setTime(0);
      //   portamento[curr_voice].start(last_port_note);
      //   portamento[curr_voice].next();  // jump this voice to the prev note pitch, this is really hacky
      //   portamento[curr_voice].next();
      //   portamento[curr_voice].setTime(port_length);
      //   portamento[curr_voice].start(float_to_Q16n16(sample_freq[curr_voice]));
      // }

      // last_port_note = float_to_Q16n16(sample_freq[curr_voice]);  // set this as last note played for portamento

      computerNoteQueue.add(myNote);  // add to active notes queue
    }
  }
}


/**
* @brief Handles turning off sample notes.
*
* @param note is the MIDI note number to turn off
* @param channel is the MIDI channel of the note to turn off
*/
void computerNoteOff(int note, int channel) {
  if (channel == MIDI_NOTE_CHANNEL) {
    int num_active_notes = computerNoteQueue.size();
    ActiveMidiNote *myNote;
    // loop through voice queue searching for note
    for (int i = 0; i < num_active_notes; i++) {
      myNote = computerNoteQueue.get(i);
      if (note == myNote->note_num && channel == myNote->channel) {
        sample_env[myNote->voice_num].noteOff();  // turn off note
        delete (myNote);
        computerNoteQueue.remove(i);
        return;
      }
    }
  }
}

void updateComputerNotes() {
  int num_active_notes = computerNoteQueue.size();
  if (num_active_notes > 0) {
    bool entries_to_delete[num_active_notes];
    ActiveMidiNote *myNote;
    // loop through voice queue searching for note
    for (int i = 0; i < num_active_notes; i++) {
      myNote = computerNoteQueue.get(i);
      if (myNote->finished()) {
        entries_to_delete[i] = true;
        sample_env[myNote->voice_num].noteOff();  // turn off note
      } else {
        entries_to_delete[i] = false;
      }
    }

    // delete tagged notes starting at end of array
    for (int i = num_active_notes - 1; i >= 0; i--) {
      if (entries_to_delete[i]) {
        delete (computerNoteQueue.get(i));
        computerNoteQueue.remove(i);
      }
    }
  }
}
