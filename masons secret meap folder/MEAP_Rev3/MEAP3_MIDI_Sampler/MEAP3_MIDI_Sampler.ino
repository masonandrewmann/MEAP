/*
  // DIP 1 - up enables sample looping
  // DIP 2 - 
  // DIP 3 - 
  // DIP 4 - 
  // DIP 5 - 
  // DIP 6 - 
  // DIP 7 - up enables filter cutoff randomization
  // DIP 8 - up is bandpass, down is lowpass
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <MIDI.h>
#include <SPI.h>
#include <ADSR.h>
#include <Sample.h>
#include <LinkedList.h>
#include "piano.h"
#include "C_maj.h"
#include "tpt.h"
#include "glock.h"
#include "gtr.h"
#include "sine.h"
#include "harp.h"
#include "clari.h"
#include <ResonantFilter.h>
#include <AudioDelayFeedback.h>

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable

#define DEBUG 1  // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define POLYPHONY 40
#define MAX_SAMPLE_LENGTH 300000

enum sequencerStates {
  PAUSED,
  PLAYING
};

using namespace admux;

// variables for DIP switches
Mux mux(Pin(14, INPUT, PinType::Digital), Pinset(38, 45, 46));
int dipPins[] = { 5, 6, 7, 4, 3, 0, 2, 1 };
int dipVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int prevDipVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// variables for capacitive touch pads
int touchPins[] = { 2, 4, 6, 8, 1, 3, 5, 7 };
int touchAvgs[] = { 100, 100, 100, 100, 100, 100, 100, 100 };
int touchVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int prevTouchVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int touchThreshold = 500;

// variables for potentiometers
int potVals[] = { 0, 0 };

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Sample voices
Sample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;
float sample_freq[POLYPHONY];
bool sample_active[POLYPHONY] = { false };
int curr_voice = 0;

const int8_t *samples_list[8] = { piano_DATA, clari_DATA, tpt_DATA, glock_DATA, gtr_DATA, sine_DATA, harp_DATA, C_maj_DATA };
int samples_lengths[8] = { piano_NUM_CELLS, clari_NUM_CELLS, tpt_NUM_CELLS, glock_NUM_CELLS, gtr_NUM_CELLS, sine_NUM_CELLS, harp_NUM_CELLS, C_maj_NUM_CELLS };
int curr_sample = 0;

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[POLYPHONY];
int sample_gain[POLYPHONY];
int attack_time = 1;
int release_time = 2000;
bool attack_flag = false;
bool release_flag = false;

class MidiNoteVoice {
public:
  int note_num;
  int voice_num;
};

LinkedList<MidiNoteVoice *> voiceQueue = LinkedList<MidiNoteVoice *>();

MultiResonantFilter<uint8_t> filter;
int cutoff = 120;
int resonance = 200;
bool cutoff_flag = false;
bool resonance_flag = false;

// MIDI clock timer
uint32_t midi_timer = 0;
uint32_t midi_micros = 10000;
int midi_step_num = 0;
bool start_flag = false;
bool stop_flag = false;

int sequence[] = { 48, 52, 55, 59, 60, 59, 55, 52,
                   48, 52, 55, 59, 60, 59, 55, 52,
                   48, 53, 57, 60, 64, 60, 57, 53,
                   48, 53, 57, 60, 64, 60, 57, 53 }; // sine arps
int sequence_index = 0;

int seq2[] = { 67, -1, -1, 67, 60, 67, -1, 69,
               -1, -1, -1, -1, -1, -1, -1, -1 }; // gtr
int seq3[] = { 64, -1, -1, 64, -1, 64, -1, -1,
               -1, -1, -1, -1, -1, -1, 64, -1 }; // gtr
int seq4[] = { -1, 48, 52, 50, 43, -1, -1, -1,
               -1, -1, 52, -1, 45, -1, -1, -1 }; // piano
int seq2_index = 0;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1);                      // sets up touch pads

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

  midi_micros = midiPulseMicros(120);
}


void loop() {
  audioHook();
  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    switch (MIDI.getType())  // Get the type of the message we caught
    {
      case midi::NoteOn:
        {
          debug("Note On: ");
          int note = MIDI.getData1();
          int velocity = MIDI.getData2();
          debug("note: ");
          debug(note);
          debug(" velocity: ");
          debugln(velocity);
          noteOnHandler(note, velocity, -1);
          break;
        }
      case midi::NoteOff:
        {
          int note = MIDI.getData1();
          debug("Note Off: ");
          debug("note: ");
          debugln(note);

          noteOffHandler(note);
          break;
        }
      case midi::ControlChange:
        //p1=74 p2=71 p3=73&23 p4=72&26 mod=1
        {
          int controller = MIDI.getData1();
          debug("c ");
          debug(controller);
          int data = MIDI.getData2();
          debug(" : ");
          debugln(data);
          switch (controller) {
            case 74:  // knob #1
              {
                // cutoff = map(data, 0, 127, 0, 255);
                cutoff = data;
                cutoff_flag = true;
                break;
              }
            case 71:  //knob #2
              {
                // resonance = map(data, 0, 127, 0, 255);
                resonance = data;
                resonance_flag = true;
                break;
              }
            case 73:  //knob #3
              {
                // attack_time = map(data, 0, 127, 1, 2000);
                attack_time = data;
                attack_flag = true;
                break;
              }
            case 72:  //knob #4
              {
                // decay_time = map(data, 0, 127, 1, 2000);
                release_time = data;
                release_flag = true;
                break;
              }
            case 1:  // mod wheel
              {
              }
          }
        }
    }
  }
  uint32_t t = micros();
  if (t > midi_timer) {
    midi_timer = t + midi_micros;
    clockStep();
  }
}


void updateControl() {
  readDip();    // reads DIP switches
  readTouch();  // reads capacitive touch pads
  readPots();   // reads potentiometers

  for (int i = 0; i < POLYPHONY; i++) {
    sample_env[i].update();
  }

  if (attack_flag) {
    attack_flag = false;
    attack_time = map(attack_time, 0, 127, 1, 2000);
    for (int i = 0; i < POLYPHONY; i++) {
      sample_env[i].setAttackTime(attack_time);
    }
  }

  if (release_flag) {
    release_flag = false;
    release_time = map(release_time, 0, 127, 1, 2000);
    for (int i = 0; i < POLYPHONY; i++) {
      sample_env[i].setReleaseTime(release_time);
    }
  }

  if (cutoff_flag) {
    cutoff_flag = false;
    cutoff = cutoff << 1;  // map to 0-255
    filter.setCutoffFreq(cutoff);
  }

  if (resonance_flag) {
    resonance_flag = false;
    resonance = resonance << 1;  // map to 0-255
    filter.setResonance(resonance);
    filter.setCutoffFreq(cutoff);
  }

  midi_micros = midiPulseMicros(map(potVals[0], 0, 4095, 80, 160));
  // debugln(midi_micros);
}


AudioOutput_t updateAudio() {
  int out_sample = 0;
  for (int i = 0; i < POLYPHONY; i++) {
    sample_gain[i] = sample_env[i].next();
    out_sample += sample[i].next() * sample_gain[i];
  }
  filter.next(out_sample);
  int filtered_out;
  if (dipVals[7]) {
    filtered_out = filter.band();
  } else {
    filtered_out = filter.low();
  }
  return StereoOutput::fromAlmostNBit(19, filtered_out, filtered_out);
}

void readDip() {
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
    mux.read(i);  //read once and throw away result (for reliability)
    dipVals[dipPins[i]] = !mux.read(i);
  }
  //Check if any switches changed position
  for (int i = 0; i < 8; i++) {
    switch (i) {
      case 0:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 1 up");
            for (int i = 0; i < POLYPHONY; i++) {
              sample[i].setLoopingOn();
            }
          } else {
            debugln("DIP 1 down");
            for (int i = 0; i < POLYPHONY; i++) {
              sample[i].setLoopingOff();
            }
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 2 up");
          } else {
            debugln("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 3 up");
          } else {
            debugln("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 4 up");
          } else {
            debugln("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 5 up");
          } else {
            debugln("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 6 up");
          } else {
            debugln("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 7 up");
          } else {
            debugln("DIP 7 down");
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 8 up");
          } else {
            debugln("DIP 8 down");
          }
        }
        break;
    }
    prevDipVals[i] = dipVals[i];  // update prevDipVals array
  }
}

void readTouch() {
  int pinVal = 0;
  for (int i = 0; i < 8; i++) {
    pinVal = touchRead(touchPins[i]);
    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal;
    if (touchAvgs[i] > touchThreshold) {
      touchVals[i] = 1;
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]) {
      //HAPPENS WHEN ANY PAD IS PRESSED
      debugln(i);
      curr_sample = i;
      switch (i) {
        case 0:
          if (touchVals[i]) {  // pad 0 pressed
            debugln("pad 0 pressed");
          } else {  // pad 0 released
            debugln("pad 0 released");
          }
          break;
        case 1:
          if (touchVals[i]) {  // pad 1 pressed
            debugln("pad 1 pressed");
          } else {  // pad 1 released
            debugln("pad 1 released");
          }
          break;
        case 2:
          if (touchVals[i]) {  // pad 2 pressed
            debugln("pad 2 pressed");
          } else {  // pad 2 released
            debugln("pad 2 released");
          }
          break;
        case 3:
          if (touchVals[i]) {  // pad 3 pressed
            debugln("pad 3 pressed");
          } else {  // pad 3 released
            debugln("pad 3 released");
          }
          break;
        case 4:
          if (touchVals[i]) {  // pad 4 pressed
            debugln("pad 4 pressed");
          } else {  // pad 4 released
            debugln("pad 4 released");
          }
          break;
        case 5:
          if (touchVals[i]) {  // pad 5 pressed
            debugln("pad 5 pressed");
          } else {  // pad 5 released
            debugln("pad 5 released");
          }
          break;
        case 6:
          if (touchVals[i]) {  // pad 6 pressed
            debugln("pad 6 pressed");
          } else {  // pad 6 released
            debugln("pad 6 released");
          }
          break;
        case 7:
          if (touchVals[i]) {  // pad 7 pressed
            debugln("pad 7 pressed");
          } else {  // pad 7 released
            debugln("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void readPots() {
  potVals[0] = mozziAnalogRead(9);
  potVals[1] = mozziAnalogRead(10);
}

// long irand(long howsmall, long howbig) {  // generates a random integer between howsmall and howbig (inclusive of both numbers)
//   howbig++;
//   if (howsmall >= howbig) {
//     return howsmall;
//   }
//   long diff = howbig - howsmall;
//   return (xorshift96() % diff) + howsmall;
// }

// float frand()  // generates a random float between 0 and 1 with 4 decimals of precision
// {
//   return (xorshift96() % 10000 / 10000.f);
// }

// returns the number of ms in one quarter note given a tempo in BPM
float tempoToPeriod(float tempo) {
  return 60000.f / tempo;
}

//returns the number of us (non rounded) in one MIDI Timing Clock pulse given a tempo in bpm
float midiPulseMicros(float tempo) {
  return (2500000.f / tempo);  // 2500 = 60000ms/24pulses
}

//executes when a clock step is received
void clockStep() {
  midi_step_num = (midi_step_num + 1) % 24;
  if (midi_step_num % 24 == 0) {  // quarter note
    if (dipVals[5]) {
      noteOnHandler(sequence[sequence_index], 100, 5);
      noteOffHandler(sequence[sequence_index]);
      sequence_index = (sequence_index + 1) % 32;

      if (seq2[seq2_index] != -1) {
        noteOnHandler(seq2[seq2_index], 100, 4);
        noteOffHandler(seq2[seq2_index]);
      }

      if (seq3[seq2_index] != -1) {
        noteOnHandler(seq3[seq2_index], 100, 4);
        noteOffHandler(seq3[seq2_index]);
      }

      if (seq4[seq2_index] != -1) {
        noteOnHandler(seq4[seq2_index], 100, 0);
        noteOffHandler(seq4[seq2_index]);
      }
      seq2_index = (seq2_index + 1) % 16;
    }
  }
  if (midi_step_num % 12 == 0) {  // eighth note
    // debugln("qq");
    if (potVals[1] > 30) {
      int notes[] = { 67, 72, 76 };
      int index = irand(map(potVals[1], 0, 4095, -50, 0), 2);
      int my_note = notes[index];
      if (irand(0, 1) == 1) {
        my_note -= 12;
      }
      if (index >= 0) {
        noteOnHandler(my_note, 100, 3);
        noteOffHandler(my_note);
      }
      if (dipVals[6]) {
        randomizeCutoff();
      }
    }
  }
  if (midi_step_num % 6 == 0) {  // sixteenth note
    if (dipVals[6]) {
      if (irand(0, 4) == 0) {
        randomizeCutoff();
      }
    }
  }
}

void randomizeCutoff() {
  int rand_cutoff = cutoff + irand(-50, 50);
  if (rand_cutoff > 255) {
    rand_cutoff = 255;
  } else if (rand_cutoff < 0) {
    rand_cutoff = 0;
  }
  filter.setCutoffFreq(rand_cutoff);
}

void noteOnHandler(int note, int velocity, int voice_override) {
  float diff = note - 60;
  debugln(curr_voice);
  curr_voice = -1;
  for (int i = 0; i < POLYPHONY; i++) {
    // if(!sample_active[i]){
    //   sample_active[i] = true;
    //   curr_voice = i;
    //   break;
    // }
    if (!sample_env[i].playing()) {
      sample_active[i] = true;
      curr_voice = i;
      break;
    }
  }
  if (curr_voice != -1) {
    if (voice_override == -1) {
      sample[curr_voice].setTable(samples_list[curr_sample]);
      sample[curr_voice].setEnd(samples_lengths[curr_sample]);
    } else {
      sample[curr_voice].setTable(samples_list[voice_override]);
      sample[curr_voice].setEnd(samples_lengths[voice_override]);
    }
    sample_freq[curr_voice] = default_freq * pow(2, diff / 12.f);  // calc freq of sample
    sample[curr_voice].setFreq(sample_freq[curr_voice]);           // set frequency of sample
    sample_env[curr_voice].noteOn();
    sample[curr_voice].start();
    MidiNoteVoice *myNote = new MidiNoteVoice();  // create object with pair of note num and voice num
    myNote->note_num = note;
    myNote->voice_num = curr_voice;
    voiceQueue.add(myNote);  // add to active notes queue
  }
}

void noteOffHandler(int note) {
  int queue_size = voiceQueue.size();
  if (queue_size > 0) {
    MidiNoteVoice *myNote;
    for (int i = 0; i < queue_size; i++) {
      myNote = voiceQueue.get(i);
      if (note == myNote->note_num) {
        sample_env[myNote->voice_num].noteOff();  // turn off note
        sample_active[myNote->voice_num] = false;
        delete (voiceQueue.get(i));
        voiceQueue.remove(i);  // remove from active note queue
        break;
      }
    }
  }
}

long irand(long howsmall, long howbig)
{ // generates a random integer between howsmall and howbig (inclusive of both numbers)
  howbig++;
  if (howsmall >= howbig)
  {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return (xorshift96() % diff) + howsmall;
}

float frand() // generates a random float between 0 and 1 with 4 decimals of precision
{
  return (xorshift96() % 10000 / 10000.f);
}