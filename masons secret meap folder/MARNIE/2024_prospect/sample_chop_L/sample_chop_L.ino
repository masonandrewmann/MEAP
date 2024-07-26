/*
  Left half of Inchworm Race At Sunrise 20240329 show, plays a detuned sine drone

  Pot #1 controls detune amount, the rest control volume of various detuned sines

  Mason Mann, CC0
 */
#include <Meap.h>
#include "love_song.h"
#define MAX_SAMPLE_LENGTH 300000  // Max length of a sample

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#define NUM_SAMPLE_TRACKS 4
#define NUM_GRANULAR 20

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} clock_mode;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

// 9 knobs, each controls volume of a detuned oscillator pair

Meap meap;


int sample_offset = 50;
// sample choppin stuff
mSample<love_song_NUM_CELLS, AUDIO_RATE> sample[NUM_SAMPLE_TRACKS];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
uint64_t start_point[NUM_SAMPLE_TRACKS];
uint64_t end_point[NUM_SAMPLE_TRACKS];
uint64_t sample_length[NUM_SAMPLE_TRACKS];
int16_t sample_gain[NUM_SAMPLE_TRACKS];
float warm_default_freq;
int sample_pot_number[4] = { 6, 4, 5, 3 };
uint16_t sample_gains[NUM_SAMPLE_TRACKS] = { 255, 255, 220, 320 };




mSample<love_song_NUM_CELLS, AUDIO_RATE> granular[NUM_GRANULAR];       //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
ADSR<AUDIO_RATE, AUDIO_RATE, unsigned int> granular_env[NUM_GRANULAR]; /**< ADSR envelopes for all sample voices */
int granular_gain[NUM_GRANULAR];
uint64_t granular_end_time[NUM_GRANULAR];
uint64_t granular_timer = 100;
bool granular_enable = false;
int granular_sample = 0;
int granular_random = 0;
float granular_pitch_options[8] = { 1, 0.5, 0.5, 0.5, 2, 2, 2, 4 };

// MIDI clock timer
uint32_t clock_timer = 0;
uint32_t clock_period_micros = 10000;
int clock_pulse_num = 0;
float clock_bpm = 120;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  warm_default_freq = (float)love_song_SAMPLERATE / (float)love_song_NUM_CELLS;
  for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
    sample[i].setTable(love_song_DATA);
    sample[i].setEnd(love_song_NUM_CELLS);
    sample[i].setLoopingOn();
    start_point[i] = 0;
    sample[i].setFreq(warm_default_freq);
    sample_gain[i] = 0;
  }

  sample[2].setFreq(warm_default_freq * 2);
  sample[3].setFreq(warm_default_freq / 2.f);


  for (int i = 0; i < NUM_GRANULAR; i++) {
    granular[i].setTable(love_song_DATA);
    granular[i].setLoopingOn();
    granular_gain[i] = 0;
    granular[i].setFreq(warm_default_freq);
    granular_env[i].setTimes(4, 1, 4294967295, 4);
    granular_env[i].setADLevels(255, 255);
  }

  clock_mode = kINTERNAL;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);
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
  // meap.readDip();
  meap.readAuxMux();
  updateButtons();


  updateChops();
  updateGranular();
}


AudioOutput_t updateAudio() {
  int32_t out_sample = 0;

  for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
    out_sample += sample[i].next() * sample_gain[i];
  }

  int32_t granular_component = 0;
  if (granular_enable) {
    for (int i = 0; i < NUM_GRANULAR; i++) {
      // if (granular_env[i].playing()) {
      granular_env[i].update();
      granular_gain[i] = granular_env[i].next();
      granular_component += granular[i].next() * granular_gain[i];
      // }
    }
  }

  out_sample += (granular_component >> 1);

  return StereoOutput::fromAlmostNBit(20, out_sample, out_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed ");
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
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
      break;
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
  return;
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
            start_point[0] = meap.irand(0, 200000);
            sample[0].setEnd(start_point[0] + (meap.aux_mux_vals[6] + sample_offset) << 6);
            sample[0].start(start_point[0]);
            sample_gain[0] = 255;
          } else {  // button 0 released
            sample_gain[0] = 0;
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 pressed
            start_point[1] = meap.irand(200000, 400000);
            sample[1].setEnd(start_point[1] + (meap.aux_mux_vals[4] + sample_offset) << 6);
            sample[1].start(start_point[1]);
            sample_gain[1] = 255;
          } else {  // button 1 released
            sample_gain[1] = 0;
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 pressed
            start_point[2] = meap.irand(400000, 600000);
            sample[2].setEnd(start_point[2] + (meap.aux_mux_vals[5] + sample_offset) << 6);
            sample[2].start(start_point[2]);
            sample_gain[2] = 220;
          } else {  // button 2 released
            sample_gain[2] = 0;
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 pressed
            start_point[3] = meap.irand(600000, 800000);
            sample[3].setEnd(start_point[3] + (meap.aux_mux_vals[3] + sample_offset) << 6);
            sample[3].start(start_point[3]);
            sample_gain[3] = 320;
          } else {  // button 3 released
            sample_gain[3] = 0;
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 pressed
            granular_enable = true;
          } else {  // button 4 released
            granular_enable = false;
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 pressed
          } else {               // button 5 released
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 pressed
          } else {               // button 6 released
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
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

void updateChops() {
  for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
    sample_length[i] = (meap.aux_mux_vals[sample_pot_number[i]] + sample_offset) << 6;  // * 64
    end_point[i] = start_point[i] + sample_length[i];
    if (end_point[i] > love_song_NUM_CELLS) {
      end_point[i] = love_song_NUM_CELLS;
    }
    sample[i].setEnd(end_point[i]);
  }
}

void updateGranular() {
  uint64_t current_time = millis();
  if (granular_enable) {
    if (current_time > granular_timer) {
      //choosing length
      granular_random = meap.aux_mux_vals[0] >> 5;
      float random_offset = 0.5 + (float)meap.irand(0, granular_random) / 127.f;  // random time length addition
      granular_timer = current_time + ((4100 - meap.pot_vals[0]) >> 2) * random_offset;
      granular_env[granular_sample].noteOn();
      granular_end_time[granular_sample] = current_time + (meap.aux_mux_vals[2] >> 3);
      //choosing pitch
      if (meap.irand(0, 127) < granular_random) {
        granular_random = granular_pitch_options[meap.irand(0, 7)];
      } else {
        granular_random = 1;
      }
      float octave_offset = 4;
      if (meap.pot_vals[1] < 1000) {
        octave_offset = 0.5;
      } else if (meap.pot_vals[1] < 2000) {
        octave_offset = 1;
      } else if (meap.pot_vals[1] < 3000) {
        octave_offset = 2;
      } else {
        octave_offset = 4;
      }
      granular[granular_sample].setFreq(warm_default_freq * granular_random * octave_offset);
      granular_sample = (granular_sample + 1) % NUM_GRANULAR;
    }

    for (int i = 0; i < NUM_GRANULAR; i++) {
      if (granular_end_time[i]) {
        if (current_time > granular_end_time[i]) {
          granular_env[i].noteOff();
          granular_end_time[i] = 0;
        }
      }
    }

    // // update amplitude envelopes
    // for (int i = 0; i < NUM_GRANULAR; i++) {
    //   granular_env[i].update();
    // }
  }
}
