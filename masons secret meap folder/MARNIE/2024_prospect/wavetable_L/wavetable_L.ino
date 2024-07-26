/*
  Basic template for a MARNIE L program with clock and midi
 */

// #include <MozziGuts.h>
// #include <Oscil.h>
// #include <mozzi_midi.h>
#include <Meap.h>
#include "MEAP_Wavetable.h"
#include <LinkedList.h>
#include "ActiveMidiNote.h"
#include "wavetable_includes.h"



#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define POLYPHONY 8
#define MIDI_NOTE_CHANNEL 1  // Channel to listen for MIDI note messages

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

int curr_program = 0;

LinkedList<int16_t> voice_queue;
LinkedList<ActiveMidiNote *> humanNoteQueue = LinkedList<ActiveMidiNote *>();

mWavetable<acytol_FRAME_SIZE, acytol_NUM_FRAMES, AUDIO_RATE, acytol_FRAME_SIZE> osc[POLYPHONY];

MultiResonantFilter<uint16_t> filter; /**< Filter for left channel */

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> osc_env[POLYPHONY]; /**< ADSR envelopes for all sample voices */
int osc_gain[POLYPHONY];
float osc_freq[POLYPHONY];
int attack_time = 1;
int release_time = 500;

uint32_t freq_val = 440;
uint16_t frame_val = 0;
float alpha = 0.91;


// template<uint64_t NUM_TABLE_CELLS, uint32_t UPDATE_RATE, uint64_t FRAME_SIZE, uint64_t NUM_FRAMES, uint16_t INTERP = kINTERP_NONE>
// mWavetable <524288, 32768, 2048, 256> my_wt(analog_table_DATA);

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

  for (int i = 0; i < POLYPHONY; i++) {
    osc[i].setTable(analog_table_DATA);
    osc_freq[i] = 440;
    osc[i].setFreq(osc_freq[i]);
    osc_gain[i] = 0;
    osc_env[i].setAttackTime(attack_time);
    osc_env[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    osc_env[i].setReleaseTime(release_time);
    osc_env[i].setADLevels(255, 255);
    voice_queue.unshift(i);  // add all voices to voice queue
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
  // meap.readTouch();
  // meap.readDip();
  meap.readAuxMux();
  updateButtons();


  filter.setCutoffFreq(meap.aux_mux_vals[4] << 4);
  filter.setResonance(meap.aux_mux_vals[3] << 4);

  attack_time = meap.aux_mux_vals[6] << 2;
  release_time = meap.aux_mux_vals[5] << 2;

  // update amplitude envelopes
  for (int i = 0; i < POLYPHONY; i++) {
    osc_env[i].update();
  }

  frame_val = (alpha)*frame_val + (1.0 - alpha) * (meap.pot_vals[1] >> 4);
  for (int i = 0; i < POLYPHONY; i++) {
    osc[i].setFrame(frame_val);
  }
}


AudioOutput_t updateAudio() {
  uint64_t immediate_sample = 0;

  for (int i = 0; i < POLYPHONY; i++) {
    osc_gain[i] = osc_env[i].next();
    immediate_sample += osc[i].next() * osc_gain[i];
  }

  filter.next(immediate_sample);
  immediate_sample = filter.low();

  return StereoOutput::fromAlmostNBit(28, immediate_sample, immediate_sample);
}

void Meap::updateTouch(int number, bool pressed) {
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
          } else {               // button 0 up
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 down
          } else {               // button 1 up
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 down
          } else {               // button 2 up
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 down
          } else {               // button 3 up
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 down
          } else {               // button 4 up
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 down
          } else {               // button 5 up
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 down
          } else {               // button 6 up
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
      noteOnHandler(data1, data2, channel, -1);
      break;
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
      noteOffHandler(data1, channel);
      break;
    case midi::ProgramChange:  // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      if (data1 >= 0 && data1 < NUM_WAVETABLES) {
        curr_program = data1;
      }
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
    int curr_voice = -1;
    if (voice_queue.size() > 0) {
      curr_voice = voice_queue.shift();  // remove head element and return it!
    } else {
      return;  // no free voices, move along...
    }

    osc_freq[curr_voice] = mtof(note);
    osc[curr_voice].setFreq(osc_freq[curr_voice]);  // set frequency of sample
    osc[curr_voice].setTable(wavetables_list[my_program]);

    // update the envelope
    osc_env[curr_voice].setAttackTime(attack_time >> 1 + 1);
    osc_env[curr_voice].setReleaseTime(release_time >> 1 + 1);
    osc_env[curr_voice].noteOn();

    // keep track of note in active notes queue
    ActiveMidiNote *myNote = new ActiveMidiNote();  // create object to keep track of note num, voice num and freq
    myNote->newNote(note, velocity, channel, osc_freq[curr_voice], curr_voice);

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
        osc_env[myNote->voice_num].noteOff();  // turn off note
        voice_queue.add(myNote->voice_num); 
        delete (myNote);
        humanNoteQueue.remove(i);
        return;
      }
    }
  }
}