/*
  Basic template for a MARNIE L program with clock and midi
 */

#include <Meap.h>
#include <tables/cos2048_int8.h> // table for Oscils to play


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

// mWavetableSynth<AUDIO_RATE, POLYPHONY, analog_table_FRAME_SIZE, analog_table_NUM_FRAMES> wave(analog_table_DATA, NULL);

mOscBank<COS2048_NUM_CELLS, AUDIO_RATE, CONTROL_RATE, 10> bank(COS2048_DATA);

uint32_t freq_val = 440;
uint16_t frame_val = 0;
float alpha = 0.91;

int attack_time = 1;
int release_time = 500;


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

  bank.setTranspose(24);
  bank.enableDetune();
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

  attack_time = meap.aux_mux_vals[6] << 2;
  release_time = meap.aux_mux_vals[5] << 2;

  bank.setAttackTime(attack_time);
  bank.setReleaseTime(release_time);


  // mod_val = mod.next();

  // frame_val = (alpha)*frame_val + (1.0 - alpha) * (meap.pot_vals[1] >> 4);

  // int16_t frame_mod = mod_val + frame_val;

  // if(frame_mod > analog_table_NUM_FRAMES){
  //   frame_mod -= analog_table_NUM_FRAMES;
  // } else if (frame_mod < 0){
  //   frame_mod += analog_table_NUM_FRAMES;
  // }
  // wave.setFrame(frame_mod);
  float amt = ((float)meap.pot_vals[1]) /  60000.0 + 0.965;
  bank.setDetuneAmount(amt);
  Serial.println(amt);

}


AudioOutput_t updateAudio() {
  uint64_t immediate_sample = 0;

  immediate_sample += bank.next();

  return StereoOutput::fromAlmostNBit(20, immediate_sample, immediate_sample);
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
      // if (data1 >= 0 && data1 < NUM_WAVETABLES) {
      //   // curr_program = data1;
      //   wave.setTable(wavetables_list[data1]);
      // }
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
  // choose voice to play
  // int my_program = curr_program;
  // if (pgm_override != -1) {
  //   my_program = pgm_override;
  // }

  bank.noteOn(note);
}



/**
* @brief Handles turning off sample notes.
*
* @param note is the MIDI note number to turn off
* @param channel is the MIDI channel of the note to turn off
*/
void noteOffHandler(int note, int channel) {
  bank.noteOff(note);
}