/*
  Basic template for a MARNIE L program with clock and midi
 */

#include <Meap.h>
#include "sample_includes.h"


#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#define POLYPHONY 4

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

// ********************************

enum MainModes {
  kSAMPLER,
  kFM
} main_mode = kSAMPLER;

mRompler<300000, AUDIO_RATE, 8, int16_t> rompler;

MultiResonantFilter<uint16_t> filter; /**< Filter for left channel */
int32_t cutoff = 255;
int32_t resonance = 127;

int16_t curr_program = 0;

mFM4Poly<SIN2048_NUM_CELLS, AUDIO_RATE, CONTROL_RATE, int8_t, 4> poly_voice;

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

  rompler.init(samples_list, samples_lengths);

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
  meap.readInputs();
  updateButtons();

  //1 pole smoothing here
  float alpha = 0.82;
  int32_t prev_cutoff = cutoff;
  cutoff = (alpha)*cutoff + (1.0 - alpha) * (meap.aux_mux_vals[4] << 4);  // 4095 -> 65535

  if (cutoff > 63000) {
    cutoff = 63000;
  }

  if (prev_cutoff != cutoff) {
    filter.setCutoffFreq(cutoff);
  }


  resonance = meap.aux_mux_vals[6] << 4;  // 4095 -> 65535
  filter.setResonance(resonance);


  if (main_mode == kSAMPLER) {
    rompler.setAttackTime(meap.aux_mux_vals[5]);
    rompler.setReleaseTime(meap.aux_mux_vals[7]);
  } else if (main_mode == kFM) {
    for (uint16_t i = 0; i < POLYPHONY; i++) {
      poly_voice.voices[i].op[0].setAttackTime(map(meap.aux_mux_vals[5] >> 4, 0, 255, 1, 3000));
      poly_voice.voices[i].op[0].setReleaseTime(map(meap.aux_mux_vals[7] >> 4, 0, 255, 1, 3000));

      poly_voice.voices[i].op[1].setAttackTime(map(meap.pot_vals[1] >> 4, 0, 255, 1, 3000));
      poly_voice.voices[i].op[1].setReleaseTime(map(meap.pot_vals[0] >> 4, 0, 255, 1, 3000));

      poly_voice.voices[i].op[2].setAttackTime(map(meap.pot_vals[2] >> 4, 0, 255, 1, 3000));
      poly_voice.voices[i].op[2].setReleaseTime(3000);

      poly_voice.voices[i].op[3].setAttackTime(2000);
      poly_voice.voices[i].op[3].setReleaseTime(2000);


      poly_voice.voices[i].op[0].setGain(255);
      poly_voice.voices[i].op[1].setGain(meap.aux_mux_vals[0] >> 4);
      poly_voice.voices[i].op[2].setGain(meap.aux_mux_vals[1] >> 4);
      poly_voice.voices[i].op[3].setGain(meap.aux_mux_vals[3] >> 4);
    }
    // vel = 127;

    poly_voice.update();
  }
}


AudioOutput_t updateAudio() {

  int32_t out_sample = 0;

  if (main_mode == kSAMPLER) {
    out_sample = rompler.next();
  } else {
    out_sample = poly_voice.next();
  }


  filter.next(out_sample);
  out_sample = filter.low();
  return StereoOutput::fromNBit(18, out_sample, out_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        Serial.println("Pad 1 pressed");
      } else {  // Pad 1 released
        Serial.println("Pad 1 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        Serial.println("Pad 2 pressed");
      } else {  // Pad 2 released
        Serial.println("Pad 2 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        Serial.println("Pad 3 pressed");
      } else {  // Pad 3 released
        Serial.println("Pad 3 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        Serial.println("Pad 4 pressed");
      } else {  // Pad 4 released
        Serial.println("Pad 4 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
        Serial.println("Pad 5 pressed");
      } else {  // Pad 5 released
        Serial.println("Pad 5 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
        Serial.println("Pad 6 pressed");
      } else {  // Pad 6 released
        Serial.println("Pad 6 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
        Serial.println("Pad 7 pressed");
      } else {  // Pad 7 released
        Serial.println("Pad 7 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
        Serial.println("Pad 8 pressed");
      } else {  // Pad 8 released
        Serial.println("Pad 8 released");
      }
      break;
  }
}

void Meap::updateDip(int number, bool up) {
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
        Serial.println("dip 1 up");
      } else {  // DIP 1 down
        Serial.println("dip 1 down");
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
        Serial.println("dip 2 up");
      } else {  // DIP 2 down
        Serial.println("dip 2 down");
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
        Serial.println("dip 3 up");
      } else {  // DIP 3 down
        Serial.println("dip 3 down");
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
        Serial.println("dip 4 up");
      } else {  // DIP 4 down
        Serial.println("dip 4 down");
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
        Serial.println("dip 5 up");
      } else {  // DIP 5 down
        Serial.println("dip 5 down");
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
        Serial.println("dip 6 up");
      } else {  // DIP 6 down
        Serial.println("dip 6 down");
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
        Serial.println("dip 7 up");
      } else {  // DIP 7 down
        Serial.println("dip 7 down");
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
        Serial.println("dip 8 up");
      } else {  // DIP 8 down
        Serial.println("dip 8 down");
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
            Serial.println("Button 0 down");
          } else {  // button 0 up
            Serial.println("Button 0 up");
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 down
            Serial.println("Button 1 down");
          } else {  // button 1 up
            Serial.println("Button 1 up");
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 down
            Serial.println("Button 2 down");
          } else {  // button 2 up
            Serial.println("Button 2 up");
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 down
            Serial.println("Button 3 down");
          } else {  // button 3 up
            Serial.println("Button 3 up");
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 down
            Serial.println("Button 4 down");
          } else {  // button 4 up
            Serial.println("Button 4 up");
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 down
            Serial.println("Button 5 down");
          } else {  // button 5 up
            Serial.println("Button 5 up");
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 down
            Serial.println("Button 6 down");
          } else {  // button 6 up
            Serial.println("Button 6 up");
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
      switch (main_mode) {
        case kSAMPLER:
          rompler.noteOn(data1, data2);
          break;
        case kFM:
          poly_voice.noteOn(data1, data2);
          break;
      }

      break;
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
      switch (main_mode) {
        case kSAMPLER:
          rompler.noteOff(data1);
          break;
        case kFM:
          poly_voice.noteOff(data1);
          break;
      }
      break;
    case midi::ProgramChange:  // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      curr_program = data1;
      if (curr_program >= NUM_SAMPLES) {
        // fm mode
        poly_voice.setAlgorithm(curr_program - NUM_SAMPLES);
        main_mode = kFM;
      } else if (curr_program < NUM_SAMPLES) {
        rompler.setProgram(data1);
        main_mode = kSAMPLER;
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

  if (clock_pulse_num % 3 == 0) {  // thirtysecond note
  }

  clock_pulse_num = (clock_pulse_num + 1) % 24;
}