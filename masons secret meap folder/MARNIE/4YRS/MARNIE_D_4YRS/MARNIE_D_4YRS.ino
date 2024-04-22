/*
  Drum machine half of MARNIE

  Mason Mann, CC0
 */

// #include <MozziGuts.h>
// #include <mozzi_midi.h>
#include <MIDI.h>
#include <Meap.h>
// #include <Sample.h>
// #include <ResonantFilter.h>
#include "samples/kick.h"
#include "samples/snare1.h"
#include "samples/snare2.h"
#include "samples/snare3.h"
#include "samples/hhc.h"
#include "samples/hho.h"
#include "samples/shaker.h"
#include "samples/claves.h"
#include "patterns.h"

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

enum sequencerStates {
  PAUSED,
  PLAYING
};

enum clockModes {
  CK_INTERNAL,
  CK_EXTERNAL
};

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

Meap meap;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

Sample<16715, AUDIO_RATE> sample[NUM_DRUMS];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float sample_freq[NUM_DRUMS];
int sample_gains[NUM_DRUMS] = { 150, 127, 80, 80, 127, 127, 127, 127 };

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 100000;
int midi_step_num = 0;
bool start_flag = false;
bool stop_flag = false;
int midi_clock_mode = CK_INTERNAL;

// //sequencer patterns
int pattern_pointer = 0;
int sequencer_state = PAUSED;
int curr_pattern = 0;
int rand_threshold = 4095;

//filter
MultiResonantFilter<uint8_t> filter;
int cutoff = 255;
int resonance = 127;


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  midi_micros = meap.midiPulseMicros(240);

  sample_freq[0] = (float)kick_SAMPLERATE / (float)kick_NUM_CELLS;
  sample[0].setTable(kick_DATA);
  sample[0].setFreq(sample_freq[0] / 6);
  sample[0].setEnd(kick_NUM_CELLS);

  sample_freq[1] = (float)snare1_SAMPLERATE / (float)snare1_NUM_CELLS;
  sample[1].setTable(snare1_DATA);
  sample[1].setFreq(sample_freq[1] / 4);
  sample[1].setEnd(snare1_NUM_CELLS);

  sample_freq[2] = (float)hhc_SAMPLERATE / (float)hhc_NUM_CELLS;
  sample[2].setTable(hhc_DATA);
  sample[2].setFreq(sample_freq[2] / 5);
  sample[2].setEnd(hhc_NUM_CELLS);

  sample_freq[3] = (float)hho_SAMPLERATE / (float)hho_NUM_CELLS;
  sample[3].setTable(hho_DATA);
  sample[3].setFreq(sample_freq[3]);
  sample[3].setEnd(hho_NUM_CELLS);

  sample_freq[4] = (float)shaker_SAMPLERATE / (float)shaker_NUM_CELLS;
  sample[4].setTable(shaker_DATA);
  sample[4].setFreq(sample_freq[4]);
  sample[4].setEnd(shaker_NUM_CELLS);

  sample_freq[5] = (float)claves_SAMPLERATE / (float)claves_NUM_CELLS;
  sample[5].setTable(claves_DATA);
  sample[5].setFreq(sample_freq[5]);
  sample[5].setEnd(claves_NUM_CELLS);

  sample_freq[6] = (float)snare2_SAMPLERATE / (float)snare2_NUM_CELLS;
  sample[6].setTable(snare2_DATA);
  sample[6].setFreq(sample_freq[6]);
  sample[6].setEnd(snare2_NUM_CELLS);

  sample_freq[7] = (float)snare3_SAMPLERATE / (float)snare3_NUM_CELLS;
  sample[7].setTable(snare3_DATA);
  sample[7].setFreq(sample_freq[7]);
  sample[7].setEnd(snare3_NUM_CELLS);

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  // initialize filter
  filter.setCutoffFreq(cutoff);
  filter.setResonance(resonance);
}


void loop() {
  audioHook();

  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }

  if (midi_clock_mode == CK_INTERNAL) {
    uint32_t t = micros();
    if (t > midi_timer) {  // we have hit a 24ppq step
      midi_timer = t + midi_micros;

      if (start_flag) {  // we need to send a start message
        start_flag = false;
        pattern_pointer = 0;
        sequencer_state = PLAYING;
        MIDI.sendStart();
      }
      if (stop_flag) {  // we need to send a stop message
        stop_flag = false;
        MIDI.sendStop();
      }
      clockStep();
      // debugln(midi_step_num);
      MIDI.sendRealTime((midi::MidiType)248);
    }
  }
}


void updateControl() {
  meap.readPots();
  meap.readTouch();
  meap.readDip();
  meap.readAuxMux();
  updateButtons();

  midi_micros = meap.midiPulseMicros(map(meap.pot_vals[0], 0, 4095, 70, 180));

  cutoff = map(meap.aux_mux_vals[4], 0, 4095, 0, 255);
  filter.setCutoffFreq(cutoff);

  resonance = map(meap.aux_mux_vals[3], 0, 4095, 0, 255);
  filter.setResonance(resonance);

  rand_threshold = 4095 - meap.aux_mux_vals[1];
}


AudioOutput_t updateAudio() {
  int my_sample = 0;
  for (int i = 0; i < NUM_DRUMS; i++) {
    my_sample += sample[i].next() * sample_gains[i];
  }
  filter.next(my_sample);
  my_sample = filter.low();
  return StereoOutput::fromAlmostNBit(18, my_sample, my_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  curr_pattern = number;

  if (pressed) {
    if (sequencer_state == PAUSED) {
      start_flag = true;
      midi_step_num = -1;
      pattern_pointer = 0;
      sequencer_state = PLAYING;      
    } else {
      stop_flag = true;
      sequencer_state = PAUSED;
    }
  }

  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed");
        // start_flag = true;
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
        // stop_flag = true;
        // sequencer_state = PAUSED;
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
        // MIDI.sendContinue();
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
        // MIDI.sendRealTime((midi::MidiType)248);
      } else {  // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
      } else {  // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
      } else {  // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
      } else {  // Pad 7 released
        Serial.println("t7 released");
      }
    case 7:
      if (pressed) {  // Pad 8 pressed
        Serial.println("t8 pressed");
      } else {  // Pad 8 released
        Serial.println("t8 released");
      }
      break;
  }
}


void Meap::updateDip(int number, bool up) {
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
        midi_clock_mode = CK_EXTERNAL;
      } else {  // DIP 2 down
        Serial.println("d2 down");
        midi_clock_mode = CK_INTERNAL;
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
        Serial.println("d8 up");
      } else {  // DIP 8 down
        Serial.println("d8 down");
      }
      break;
  }
}

void updateButtons() {
  for (int i = 0; i < 7; i++) {
    // Read the buttons
    if (i == 0) {
      if (meap.aux_mux_vals[7] < 1000) {
        button_vals[i] = 0;
      } else {
        button_vals[i] = 1;
      }
    } else {
      button_vals[i] = digitalRead(button_pins[i - 1]);
    }

    // Check if state has changed
    if (button_vals[i] != last_button_vals[i]) {
      switch (i) {
        case 0:
          if (button_vals[i]) {  // button 0 pressed
            Serial.println("Button 0 pressed");
          } else {  // button 0 released
            Serial.println("Button 0 released");
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 pressed
            Serial.println("Button 1 pressed");
          } else {  // button 1 released
            Serial.println("Button 1 released");
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 pressed
            Serial.println("Button 2 pressed");
          } else {  // button 2 released
            Serial.println("Button 2 released");
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 pressed
            Serial.println("Button 3 pressed");
          } else {  // button 3 released
            Serial.println("Button 3 released");
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 pressed
            Serial.println("Button 4 pressed");
          } else {  // button 4 released
            Serial.println("Button 4 released");
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 pressed
            Serial.println("Button 5 pressed");
          } else {  // button 5 released
            Serial.println("Button 5 released");
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 pressed
            Serial.println("Button 6 pressed");
          } else {  // button 6 released
            Serial.println("Button 6 released");
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
  }
}


void clockStep() {
  if (sequencer_state == PLAYING) {
    if (midi_step_num % 24 == 0) {
      // debug("q");
    }
    if (midi_step_num % 12 == 0) {
      // debug("e");
    }
    if (midi_step_num % 6 == 0) {
      // debug("s");
      int curr_step = pattern_pointer;
      if(meap.irand(0, 4095) > rand_threshold){
        curr_step = meap.irand(0, 31);
      }
      for (int i = 0; i < NUM_DRUMS; i++) {
        if (sample_pattern[curr_pattern][i][curr_step] == 1) {
          sample[i].start();
        }
      }
      pattern_pointer = (pattern_pointer + 1) % 32;
    }
    // debugln(" ");
  }

  midi_step_num = (midi_step_num + 1) % 24;
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

    case midi::Clock:  // ---------- MIDI CLOCK PULSE RECEIVED ----------
      if (midi_clock_mode == CK_EXTERNAL && sequencer_state == PLAYING) {
        clockStep();
        debug("clock: ");
        debugln(midi_step_num);
      }
      break;
    case midi::Start:  // ---------- MIDI START MESSAGE RECEIVED ----------
      midi_step_num = -1;
      pattern_pointer = 0;
      sequencer_state = PLAYING;
      debugln("start");
      break;
    case midi::Stop:  // ---------- MIDI STOP MESSAGE RECEIVED ----------
      sequencer_state = PAUSED;
      debugln("stop");
      break;
    case midi::Continue:  // ---------- MIDI CONTINUE MESSAGE RECEIVED ----------
      sequencer_state = PLAYING;
      debugln("continue");
      break;
    case midi::ControlChange:  // ---------- MIDI CONTROL CHANGE RECEIVED ----------
      data1 = MIDI.getData1();
      data2 = MIDI.getData2();
      switch (data1) {
        case 71:  // knob #1
          break;
        case 74:  //knob #2
          break;
        case 84:  //knob #3
          break;
        case 7:  //knob #4
          break;
        case 91:  // knob 5
          break;
        case 93:  //knob #6
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
          // MIDI.sendRealTime(midi::Stop);
          break;
        case 24:  // play button
          // midi_step_num = 0;
          // MIDI.sendRealTime(midi::Start);
          break;
        case 25:  // record button
          midi_step_num = 0;
          pattern_pointer = 0;
          break;
      }
      break;
  }
}
