/*
  hiii... Basic template for a MARNIE L program with clock and midi
 */

#include <Meap.h>
#include "sample_includes.h"
#include "midi/summer/summer_song.h"
#include "midi/summer/summer_song_arp.h"
// #include "midi/bass.h"
#include "midi/indie/indie_melody.h"
#include "midi/indie/indie_arp.h"
#include "midi/indie/indie_chorus.h"

#include <tables/BandLimited_SAW/2048/saw_max_136_at_16384_2048_int8.h>
#include <tables/triangle2048_int8.h>

#include "midi/collar/collar_delay_drums.h"
#include "midi/collar/collar_aux_drums.h"
#include "midi/collar/collar_arp.h"
#include "midi/collar/collar_sample.h"

#include "midi/sealed/sealed_samples.h"

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

#define DEBUG 0

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

enum Songs {
  kSUMMER,
  kINDIE,
  kCOLLAR,
  kSEALED
} current_song = kSUMMER;

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); /**< MEAP hardware serial port*/

// MIDI clock timer
uint32_t clock_timer = 0;
uint32_t clock_period_micros = 10000;
int clock_pulse_num = 0;
float clock_bpm = 121;

float default_freq;

Meap meap;

// for summer song
mDrumRack<300000, SUMMER_NUM_SAMPLES, AUDIO_RATE> summer_song(summer_list, summer_lengths, summer_song_data);
// mSamplerVoice<300000, AUDIO_RATE> bass(upright_bass_DATA, upright_bass_NUM_CELLS, bass_data);
mBasicFM<SAW_MAX_136_AT_16384_2048_NUM_CELLS, AUDIO_RATE> summer_fm(SAW_MAX_136_AT_16384_2048_DATA, summer_song_arp_data);

// for indie melody
mDrumRack<300000, INDIE_NUM_SAMPLES, AUDIO_RATE> indie_melody(indie_list, indie_lengths, indie_melody_data);
mBasicFM<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> indie_fm(TRIANGLE2048_DATA, indie_arp_data);
mSamplerVoice<300000, AUDIO_RATE, int16_t> indie_chorus(round_DATA, round_NUM_CELLS, indie_chorus_data);
mBasicFM<SAW_MAX_136_AT_16384_2048_NUM_CELLS, AUDIO_RATE> indie_chorus_layer(SAW_MAX_136_AT_16384_2048_DATA, indie_chorus_data);

//for collar button
mDrumRack<300000, COLLAR_DELAY_NUM_SAMPLES, AUDIO_RATE> collar_main(collar_delay_list, collar_delay_lengths, collar_delay_drums_data);
mDrumRack<300000, COLLAR_AUX_NUM_SAMPLES, AUDIO_RATE> collar_aux(collar_aux_list, collar_aux_lengths, collar_aux_drums_data);
// mBasicFM<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> collar_fm(TRIANGLE2048_DATA, collar_arp_data);
mBasicFMPOLY<TRIANGLE2048_NUM_CELLS, AUDIO_RATE, 2> collar_fm(TRIANGLE2048_DATA, collar_arp_data);

mDrumRack<300000, COLLAR_SPEECH_SAMPLES, AUDIO_RATE> collar_speech(collar_speech_list, collar_speech_lengths, collar_sample_data);
// MEAP_Delay_Allpass<int32_t> collar_delay(8192, 32768);  // 8191
// uint32_t collar_delay_sample = 0;
// float collar_delay_volume = 0.5;
// AudioDelayFeedback<9000>

// for sealed
mDrumRack<300000, SEALED_NUM_SAMPLES, AUDIO_RATE> sealed_main(sealed_samples_list, sealed_samples_length, sealed_samples_data);


void setup() {
// #if DEBUG == 1
  Serial.begin(115200);  // begins Serial communication with computer
// #endif
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  clock_mode = kINTERNAL;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  default_freq = 32768.0 / 300000.0;

  summer_song.init(summer_list, summer_lengths, summer_song_data);

  indie_melody.init(indie_list, indie_lengths, indie_melody_data);

  collar_main.init(collar_delay_list, collar_delay_lengths, collar_delay_drums_data);
  collar_aux.init(collar_aux_list, collar_aux_lengths, collar_aux_drums_data);
  collar_speech.init(collar_speech_list, collar_speech_lengths, collar_sample_data);
  // collar_delay.setDelay(8192);
  // collar_delay.clear();
  collar_main.setSampleVolume(0, 204);
  collar_main.setSampleVolume(1, 102);
  collar_main.setSampleVolume(3, 382);




  indie_chorus.setDecay(1000);

  sealed_main.init(sealed_samples_list, sealed_samples_length, sealed_samples_data);
  sealed_main.setSampleVolume(2, 0);
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
  // meap.readPots();
  // meap.readTouch();
  // meap.readDip();
  // meap.readAuxMux();
  meap.readInputs();
  updateButtons();

  switch (current_song) {
    case kSUMMER:
      // for summer song
      summer_fm.setDecayTime(0, meap.pot_vals[0] >> 2);
      summer_fm.setDecayTime(1, meap.pot_vals[1] >> 2);
      summer_fm.setBDepth(meap.aux_mux_vals[6] >> 5);
      summer_fm.setCDepth(meap.aux_mux_vals[5] >> 5);
      break;
    case kINDIE:
      // for indie song
      indie_fm.setDecayTime(0, meap.pot_vals[0] >> 2);
      indie_fm.setDecayTime(1, meap.pot_vals[1] >> 2);
      indie_fm.setBDepth(meap.aux_mux_vals[6] >> 5);
      indie_fm.setCDepth(meap.aux_mux_vals[5] >> 5);

      // for indie song
      indie_chorus_layer.setDecayTime(0, meap.pot_vals[0] >> 2);
      indie_chorus_layer.setDecayTime(1, meap.pot_vals[1] >> 2);
      indie_chorus_layer.setBDepth(meap.aux_mux_vals[6] >> 5);
      indie_chorus_layer.setCDepth(meap.aux_mux_vals[5] >> 5);
      break;
    case kCOLLAR:
      //for collar buttons
      collar_fm.setDecayTime(0, meap.pot_vals[0] >> 2);
      collar_fm.setDecayTime(1, meap.pot_vals[1] >> 2);
      collar_fm.setBDepth(meap.aux_mux_vals[6] >> 5);
      collar_fm.setCDepth(meap.aux_mux_vals[5] >> 5);
      break;      
  }
}


AudioOutput_t updateAudio() {
  int32_t out_sample = 0;

  switch (current_song) {
    case kSUMMER:
      out_sample += summer_song.next();
      out_sample += summer_fm.next();
      break;
    case kINDIE:
      out_sample += indie_melody.next();
      out_sample += indie_fm.next();
      out_sample += indie_chorus.next();
      out_sample += indie_chorus_layer.next() >> 1;
      break;
    case kCOLLAR:
      out_sample += collar_main.next();
      out_sample += collar_aux.next();
      out_sample += collar_fm.next(); 
      out_sample += collar_speech.next();
      break;
    case kSEALED:
      out_sample += sealed_main.next();
      break;
  }

  return StereoOutput::fromNBit(18, out_sample, out_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        if (summer_song.isPlaying()) {
          summer_song.stop();
          summer_fm.stop();
        } else {
          clock_bpm = 121;
          clock_period_micros = meap.midiPulseMicros(clock_bpm);
          current_song = kSUMMER;
          summer_song.begin();
          summer_fm.begin();
        }
      } else {  // Pad 1 released
      }
      break;
    case 1:
      if (pressed) {
        if (indie_melody.isPlaying()) {
          indie_melody.stop();
          indie_fm.stop();
          indie_chorus.stop();
          indie_chorus_layer.stop();
        } else {
          clock_bpm = 121;
          clock_period_micros = meap.midiPulseMicros(clock_bpm);
          current_song = kINDIE;
          indie_melody.begin();
          indie_fm.begin();
          indie_chorus.begin();
          indie_chorus_layer.begin();
        }
      }
      break;
    case 2:
      if (pressed) {
        if (collar_main.isPlaying()) {
          collar_main.stop();
          collar_aux.stop();
          collar_fm.stop();
          collar_speech.stop();
        } else {
          clock_bpm = 128;
          clock_period_micros = meap.midiPulseMicros(clock_bpm);
          collar_main.begin();
          collar_aux.begin();
          collar_fm.begin();
          collar_speech.begin();
          current_song = kCOLLAR;
        }
      }
      break;
    case 3:
      if (pressed) {
        if (sealed_main.isPlaying()) {
          sealed_main.stop();
        } else {
          clock_bpm = 143;
          clock_period_micros = meap.midiPulseMicros(clock_bpm);
          sealed_main.begin();
          current_song = kSEALED;
        }
      }
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed

      } else {  // Pad 8 released
      }
      break;
  }
}

void Meap::updateDip(int number, bool up) {
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
  switch (current_song) {
    case kSUMMER:
      //for summer song
      summer_song.updateMidi();
      summer_fm.updateMidi();
      break;
    case kINDIE:
      //for indie melody
      indie_melody.updateMidi();
      indie_fm.updateMidi();
      indie_chorus.updateMidi();
      indie_chorus_layer.updateMidi();
      break;
    case kCOLLAR:
      // for collar buttons
      collar_main.updateMidi();
      collar_aux.updateMidi();
      collar_fm.updateMidi();
      collar_speech.updateMidi();
      break;
    case kSEALED:
      sealed_main.updateMidi();
      break;
  }
}