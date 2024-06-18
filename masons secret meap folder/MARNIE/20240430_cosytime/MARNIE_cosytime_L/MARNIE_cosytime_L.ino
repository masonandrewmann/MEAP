/*
  Left half of Inchworm Race At Sunrise 20240329 show, plays a detuned sine drone

  Pot #1 controls detune amount, the rest control volume of various detuned sines

  Mason Mann, CC0
 */
#include <Meap.h>
#include <tables/sin2048_int8.h>  // sine table for oscillator
#include "sample_includes.h"
#include "sample_frequencies.h"
#include "MySample.h"
#include <LinkedList.h>
#include <tables/SAW2048_int8.h>
#include <tables/triangle_valve_2048_int8.h>
#include "warm.h"
#define MAX_SAMPLE_LENGTH 300000  // Max length of a sample

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#define NUM_OSC 8
#define NUM_SAMPLE_TRACKS 4
#define NUM_GRANULAR 40
#define PULSE_VOICES 16

enum KnobModes {
  kDRONE,
  kCHOP
} current_knob_mode;

enum ButtonModes {
  kNORMAL,
  kPULSE
} current_button_mode;

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} clock_mode;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };


int osc_midi[8] = { 48, 55, 60, 64, 67, 72, 79, 84 };

// 9 knobs, each controls volume of a detuned oscillator pair

Meap meap;

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sines[NUM_OSC];

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sines_detuned[NUM_OSC];

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sines_3[NUM_OSC];

int gain[NUM_OSC] = { 0 };

float detune_amount = 1.1;

int sample_range = 60;
int sample_offset = 50;
// sample choppin stuff
MySample<warm_NUM_CELLS, AUDIO_RATE> sample[NUM_SAMPLE_TRACKS];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float sample_freq[NUM_SAMPLE_TRACKS];
uint64_t start_point[NUM_SAMPLE_TRACKS];
uint64_t end_point[NUM_SAMPLE_TRACKS];
uint64_t sample_length[NUM_SAMPLE_TRACKS];
ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sample_env[NUM_SAMPLE_TRACKS]; /**< ADSR envelopes for all sample voices */
int sample_gain[NUM_SAMPLE_TRACKS];
float warm_default_freq;



MySample<warm_NUM_CELLS, AUDIO_RATE> granular[NUM_GRANULAR];  //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float granular_freq[NUM_GRANULAR];
ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> granular_env[NUM_GRANULAR]; /**< ADSR envelopes for all sample voices */
int granular_gain[NUM_GRANULAR];
uint64_t granular_end_time[NUM_GRANULAR];
uint64_t granular_timer = 100;
uint16_t granular_delay = 100;
bool granular_enable = false;
int granular_sample = 0;
int granular_random = 0;
float granular_pitch_options[8] = { 1, 0.5, 0.5, 0.5, 2, 2, 2, 4 };


// --------------- SAW VARIABLES START ---------------
bool saw_enable = false;
#include <LowPassFilter.h>

LowPassFilter lpf;
char resonance = 70;  // range 0-255, 255 is most resonant

#include <Line.h>
Line<unsigned int> filt;

#include <ADSR.h>  //エンベロープ
ADSR<AUDIO_RATE, AUDIO_RATE> envB1;
ADSR<AUDIO_RATE, AUDIO_RATE> envB2;
ADSR<AUDIO_RATE, AUDIO_RATE> envB3;
ADSR<AUDIO_RATE, AUDIO_RATE> envB4;


#include <mozzi_midi.h>

#include <IntMap.h>                   // IntMapをインクルードする。
IntMap areadAds(0, 1024, 5, 300);     // IntMapとしてabcを定義する。　0から1024の値を0から200に変換する
IntMap areadAds2(0, 1024, 0, 700);    // IntMapとしてabcを定義する。　0から1024の値を0から200に変換する
IntMap areadGain(0, 1024, 0, 190);    // IntMapとしてabcを定義する。　0から1024の値を0から200に変換する
IntMap areadTempo(0, 1024, 10, 700);  // IntMapとしてabcを定義する。　0から1024の値を0から200に変換する


// SIN2048_DATA2048のデータをaSinに格納  その他も同様
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> triWarm1(SAW2048_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> triWarm2(SAW2048_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> triWarm3(SAW2048_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> triWarm4(SAW2048_DATA);

// Oscil<TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triWarm1(TRIANGLE_VALVE_2048_DATA);
// Oscil<TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triWarm2(TRIANGLE_VALVE_2048_DATA);
// Oscil<TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triWarm3(TRIANGLE_VALVE_2048_DATA);
// Oscil<TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triWarm4(TRIANGLE_VALVE_2048_DATA);


// トリガー(発音タイミング)のためのヘッダ
#include <EventDelay.h>
EventDelay kTriggerDelay;

// スムースに値を変化させるためのヘッダ

byte count = 0;
byte count1 = 0;
byte melcount = 0;
byte randmel;
uint8_t freq;
byte gainF = 240;


byte pin5;
byte pulse;
int tempo;
bool trigger;
byte check;  //パルスの立ち上がりの判定用


unsigned int attackBa1, decayBa1, sustainBa1, releaseBa1;
unsigned int attackBa2, decayBa2, sustainBa2, releaseBa2;
unsigned int attackBa3, decayBa3, sustainBa3, releaseBa3;
unsigned int attackBa4, decayBa4, sustainBa4, releaseBa4;




//byte melody1[30]={0,2,4,7,11,12 , 12, 14, 16, 19, 23,24, 24,26,28,31,35,36, 36,38,40,43,47,48, 48,43,52,55,40,35};   //メロディーのパターン
//byte melody1[20][4]={ {67,64,60,48},{67,62,59,47},{69,65,60,41},{71,65,62,43},  {69,64,61,45},{65,62,58,43},{67,64,58,36},{69,64,60,41},  {68,62,57,47},{66,62,57,47},{68,62,59,40},{64,60,55,36},   {68,62,59,40},{69,64,60,45},{70,65,62,43},{70,67,64,36},  {69,65,60,41},{67,64,61,45},{65,62,57,38},{65,60,56,46}  };

//byte melody1[8][3]={ {69,60,45}, {69,60,45}, {65,58,43},{67,58,36}, {69,60,41},{69,60,41}, {68,59,40},{68,59,40}};
//byte melody[15]={48,50,52,55,57, 60,62,64,67,69, 72,74,76,79,81};


//byte melody1[4][3]={ {61,57,42},  {66,62,47},{61,56,42},{64,61,42}     };
//byte melody[15]={44,47,59,61,64,66,69,71,73,76,78,81,83,85,88};


//byte melody1[4][3]={ {60,55,44},  {62,53,43}, {60,51,46},{61,51,46}  };
//byte melody1[4][3]={ {60,55,44},  {62,53,43}, {62,51,44},{62,51,46}  };
//byte melody1[4][3]={ {60,55,44},  {62,53,43}, {58,62,46},{67,51,46}  };
byte melody1[16][3] = { { 60, 55, 44 }, { 62, 53, 43 }, { 62, 51, 44 }, { 62, 51, 46 }, { 60, 55, 44 }, { 62, 53, 43 }, { 65, 55, 46 }, { 62, 51, 46 }, { 60, 55, 44 }, { 62, 53, 43 }, { 58, 62, 46 }, { 67, 51, 46 }, { 65, 51, 44 }, { 70, 58, 46 }, { 65, 62, 46 }, { 63, 55, 46 } };
// C-G-G# ... D-F-G ... G#-D#-D ... D-D#-A# ...
//byte melody[64]={63,65,67,70,72,67,70,67, 63,65,63,62,63,65,67,67, 70,65,70,75,74,75,68,67, 70,68,70,75,70,75,77,79, 82,77,79,75,79,74,79,74, 75,72,75,70,79,74,82,75 ,
//                 79,84,82,77,67,65,63,70, 75,72,77,79,82,86,84,87};
//byte melody[64]={60,65,67,70,67,62,63,60, 67,65,70,62,65,58,67,62, 70,67,65,62,74,75,75,72, 70,67,70,77,74,75,72,67, 82,77,79,74,63,67,77,70, 75,72,65,74,79,80,82,75,
//                 79,86,82,77,72,70,63,67, 75,72,79,74,82,72,80,75};
byte melody[128] = { 63, 65, 67, 70, 72, 67, 70, 67, 63, 65, 63, 62, 63, 65, 67, 67, 70, 65, 70, 75, 74, 75, 68, 67, 70, 68, 70, 75, 70, 75, 77, 79,
                     82, 77, 79, 75, 79, 74, 79, 74, 75, 72, 75, 70, 79, 74, 82, 75, 79, 84, 82, 77, 67, 65, 63, 70, 75, 72, 77, 79, 82, 86, 84, 87,
                     60, 65, 67, 70, 67, 62, 63, 60, 67, 65, 70, 62, 65, 58, 67, 62, 70, 67, 65, 62, 74, 75, 75, 72, 70, 67, 70, 77, 74, 75, 72, 67,
                     82, 77, 79, 74, 63, 67, 77, 70, 75, 72, 65, 74, 79, 80, 82, 75, 79, 86, 82, 77, 72, 70, 63, 67, 75, 72, 79, 74, 82, 72, 80, 75 };
int saw_transpose = -2;
unsigned long saw_timer = 0;
unsigned long saw_tempo_micros = 10000;
int tempo_pot_reading = 0;
bool tempo_pot_enable = false;
// --------------- SAW VARIABLES END ---------------

// --------------- PULSE VARIABLES START -----------------
int majScale[] = { 0, 2, 4, 5, 7, 9, 11 };  // template for building major scale on top of 12TET
int scale_root = 60;                        // root of major scale

int triad[] = { 0, 2, 4 };  // template for triad on top of major scale
int chord_root = 2;         // root of triad
int chord_tree_level = 4;   // what level of chord tree are we on

int entropy = 0;  // amount of entropy from 0 to 100

float tempos[5] = { 23.7047, 35.555, 53.333, 80, 120 };
int tempo_level = 3;

int rhythms[8] = { 4, 8, 12, 16, 18, 24, 36, 48 };

int timbre_level = 0;

int timbre_pairs[14][2] = {
  { 24, 24 },
  { 24, 4 },
  { 16, 4 },
  { 16, 21 },
  { 3, 4 },
  { 3, 18 },
  { 16, 18 },
  { 24, 22 },
  { 21, 22 },
  { 21, 12 },
  { 15, 22 },
  { 15, 12 },
  { 16, 12 },
  { 24, 12 }
};

int density_value = 2;
int density_direction = 1;

// Sample voices
MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> pulse_sample[PULSE_VOICES];                                                                               //maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float default_freq;                                                                                                                               //default freq of a sample, calculated using length of sample and sample rate
float pulse_freq[PULSE_VOICES];                                                                                                                   /**< stores current frequency of each sample voice */
bool pulse_active[PULSE_VOICES] = { true, true, true, true, true, true, true, true }; /**< keeps track of whether a sample voice is playing.  */  // voice from sample[] array being used

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> pulse_env[PULSE_VOICES]; /**< ADSR envelopes for all sample voices */
int pulse_gain[PULSE_VOICES];
int pulse_attack = 1;
int pulse_sustain_time = 10;
int pulse_release = 5;
int pulse_pan[PULSE_VOICES] = { 0, 1, -1, 0, 0, 1, -1, 0 };
int pulse_loop_step[PULSE_VOICES] = { 24, 12, 3, 16, 20, 4, 8, 36 };
int pulse_curr_step[PULSE_VOICES] = { 0 };
int pulse_note[PULSE_VOICES] = { 60, 67, 52, 48, 36, 43, 40, 31 };
int pulse_sample_num[PULSE_VOICES] = { 5, 5, 5, 5, 5, 5, 5, 5 };

int pulse_transpose = 0;

int note_bank[14] = { 24, 36, 48, 60, 72, 31, 43, 55, 67, 40, 52, 64, 47, 53 };

// --------------- PULSE VARIABLES END -----------------


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
  current_knob_mode = kDRONE;
  current_button_mode = kNORMAL;

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_OSC; i++) {
    sines[i].setTable(SIN2048_DATA);
    sines_detuned[i].setTable(SIN2048_DATA);
    sines_3[i].setTable(SIN2048_DATA);
  }

  warm_default_freq = (float)warm_SAMPLERATE / (float)warm_NUM_CELLS;
  for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
    sample[i].setTable(warm_DATA);
    sample[i].setEnd(warm_NUM_CELLS);
    sample[i].setLoopingOn();
    start_point[i] = 0;
    sample_freq[i] = warm_default_freq;
    sample[i].setFreq(sample_freq[i]);
    sample_gain[i] = 0;
    sample_env[i].setAttackTime(1);
    sample_env[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    sample_env[i].setReleaseTime(100);
    sample_env[i].setADLevels(255, 255);
  }

  sample_freq[2] = warm_default_freq * 2;
  sample[2].setFreq(sample_freq[2]);

  sample_freq[3] = warm_default_freq / 2;
  sample[3].setFreq(sample_freq[3]);


  for (int i = 0; i < NUM_GRANULAR; i++) {
    granular[i].setTable(warm_DATA);
    granular[i].setLoopingOn();
    granular_gain[i] = 0;
    granular_freq[i] = warm_default_freq;
    granular[i].setFreq(granular_freq[i]);
    granular_env[i].setAttackTime(2);
    granular_env[i].setSustainTime(4294967295);  // max value of unsigned 32 bit int,, notes can sustain for arbitrary time limit
    granular_env[i].setReleaseTime(2);
    granular_env[i].setADLevels(255, 255);
  }

  clock_mode = kINTERNAL;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);

  // initialize samples
  default_freq = (float)piano_SAMPLERATE / (float)MAX_SAMPLE_LENGTH;
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_loop_step[i] = pulse_loop_step[i] * 2;
    pulse_sample[i].setTable(piano_DATA);
    pulse_sample[i].setEnd(piano_NUM_CELLS);
    pulse_sample[i].setStart(1000);
    pulse_freq[i] = default_freq;
    pulse_sample[i].setFreq(pulse_freq[i]);
    pulse_gain[i] = 0;
    pulse_env[i].setAttackTime(pulse_attack);
    pulse_env[i].setSustainTime(pulse_sustain_time);
    pulse_env[i].setReleaseTime(pulse_release);
    pulse_env[i].setADLevels(255, 255);
    pulse_curr_step[i] = pulse_loop_step[i] - 1;
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
  meap.readTouch();
  meap.readDip();
  meap.readAuxMux();
  updateButtons();
  sawControl();

  detune_amount = map(meap.pot_vals[1], 0, 4095, 0, 150) / 10000.f;
  float detune_offset = 0;

  for (int i = 0; i < 8; i++) {
    // detune_offset = meap.frand()/50.f;
    float my_freq = mtof(osc_midi[i]);
    sines[i].setFreq(my_freq);
    sines_detuned[i].setFreq(my_freq * (1 + detune_amount + detune_offset));
    sines_3[i].setFreq(my_freq * (1 - detune_amount - detune_offset));
  }

  gain[0] = map(meap.aux_mux_vals[0], 0, 4095, 0, 255);
  gain[1] = map(meap.aux_mux_vals[1], 0, 4095, 0, 255);
  gain[2] = map(meap.aux_mux_vals[2], 0, 4095, 0, 255);
  gain[3] = map(meap.aux_mux_vals[3], 0, 4095, 0, 255);
  gain[4] = map(meap.aux_mux_vals[4], 0, 4095, 0, 255);
  gain[5] = map(meap.aux_mux_vals[5], 0, 4095, 0, 255);
  gain[6] = map(meap.aux_mux_vals[6], 0, 4095, 0, 255);
  gain[7] = map(meap.pot_vals[0], 0, 4095, 0, 255);

  sample_length[0] = (meap.aux_mux_vals[6] + sample_offset) * sample_range;
  sample_length[1] = (meap.aux_mux_vals[4] + sample_offset) * sample_range;
  sample_length[2] = (meap.aux_mux_vals[5] + sample_offset) * sample_range;
  sample_length[3] = (meap.aux_mux_vals[3] + sample_offset) * sample_range;

  end_point[0] = start_point[0] + sample_length[0];
  end_point[1] = start_point[1] + sample_length[1];
  end_point[2] = start_point[2] + sample_length[2];
  end_point[3] = start_point[3] + sample_length[3];

  for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
    if (end_point[i] > warm_NUM_CELLS) {
      end_point[i] = warm_NUM_CELLS;
    }
  }

  sample[0].setEnd(end_point[0]);
  sample[1].setEnd(end_point[1]);
  sample[2].setEnd(end_point[2]);
  sample[3].setEnd(end_point[3]);

  uint64_t current_time = millis();
  if (granular_enable) {
    if (current_time > granular_timer) {
      granular_random = map(meap.aux_mux_vals[0], 0, 4095, 0, 100);
      // float random_offset = 1.0 + (float)granular_random / 100 - 0.5; // random float between 0.5 and 2
      float random_offset = 1.f + (float)meap.irand(0, granular_random) / 100.f - 0.5;

      granular_timer = current_time + (4100 - meap.pot_vals[0]) / 5.f * random_offset;



      Serial.println(granular_sample);

      // granular_env[granular_sample].setReleaseTime( / 10);
      granular_env[granular_sample].noteOn();
      // granular_env[granular_sample].noteOff();
      granular_end_time[granular_sample] = current_time + meap.aux_mux_vals[2] / 10;

      // random_offset = 1.f + (float)meap.irand(0, granular_random) / 100.f - 0.5;
      if (meap.irand(0, 100) < granular_random) {
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

      granular_freq[granular_sample] = warm_default_freq * granular_random * octave_offset;
      // granular_freq[granular_sample] = warm_default_freq * (((float)meap.pot_vals[1] / 4095.f) * 3.5 + 0.5) * granular_random;

      granular[granular_sample].setFreq(granular_freq[granular_sample]);

      // granular[granular_sample].start(meap.irand(0, 2626188))
      granular_sample = (granular_sample + 1) % NUM_GRANULAR;
    }
  }

  for (int i = 0; i < NUM_GRANULAR; i++) {
    if (granular_end_time[i]) {
      if (current_time > granular_end_time[i]) {
        granular_env[i].noteOff();
        granular_end_time[i] = 0;
      }
    }
  }

  // update amplitude envelopes
  for (int i = 0; i < NUM_GRANULAR; i++) {
    granular_env[i].update();
  }

  if (current_button_mode == kPULSE) {
    // update amplitude envelopes
    for (int i = 0; i < PULSE_VOICES; i++) {
      pulse_env[i].update();
    }
  }
}


AudioOutput_t updateAudio() {
  int out_sample = 0;
  int bit_rate = 17;

  if (current_button_mode == kNORMAL) {
    if (current_knob_mode == kDRONE) {
      for (int i = 0; i < NUM_OSC; i++) {
        out_sample += (sines[i].next() + sines_detuned[i].next() + sines_3[i].next()) * gain[i];
      }
      bit_rate = 20;
    } else {
      for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
        // sample_gain[i] = sample_env[i].next();
        out_sample += sample[i].next() * sample_gain[i];
      }

      if (granular_enable) {
        for (int i = 0; i < NUM_GRANULAR; i++) {
          if (granular_env[i].playing()) {
            granular_gain[i] = granular_env[i].next();
            out_sample += granular[i].next() * granular_gain[i];
          }
        }
      }
    }

    if (saw_enable) {
      envB1.update();
      envB2.update();
      freq = envB2.next();
      int env2 = envB1.next();
      bit_rate = 8;
      out_sample = lpf.next(((((((env2 * triWarm1.next()) >> 8)) + (((env2 * triWarm2.next()) >> 8)) + (((env2 * triWarm3.next()) >> 8)) + (((env2 * triWarm4.next()) >> 8))) >> 2) * gainF) >> 7);  // サイン波とノイズを足す　ノイズはgainで256倍されているので8ビットシフト（=1/2^8倍）で音量を小さくする
    }
  }


  int32_t l_sample = out_sample;
  int32_t r_sample = out_sample;

  if (current_button_mode == kPULSE) {
    bit_rate = 19;
    // add all samples to envelope
    for (int i = 0; i < PULSE_VOICES; i++) {
      pulse_gain[i] = pulse_env[i].next();
      int32_t immediate_sample = pulse_sample[i].next();
      if (pulse_pan[i] == 0) {
        l_sample += immediate_sample * pulse_gain[i] >> 1;
        r_sample += immediate_sample * pulse_gain[i] >> 1;
      } else if (pulse_pan[i] == 1) {
        r_sample += immediate_sample * pulse_gain[i];
      } else {
        l_sample += immediate_sample * pulse_gain[i];
      }
    }
  }

  return StereoOutput::fromAlmostNBit(bit_rate, l_sample, r_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  if (current_button_mode == kNORMAL) {
    if (pressed && (number < 6)) {
      saw_transpose = -3;
    }
    if (number == 6) {
      saw_transpose--;
    }
    if (number == 7) {
      saw_transpose++;
    }
  } else {
    if (pressed) {
      if (pulse_active[number]) {
        pulse_active[number] = false;
      } else {
        pulse_active[number] = true;
      }
    }
  }

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
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
        current_button_mode = kPULSE;
      } else {  // DIP 1 down
        Serial.println("d1 down");
        current_button_mode = kNORMAL;
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
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
            if (current_button_mode == kNORMAL) {
              start_point[0] = meap.irand(0, 2626188);
              sample[0].setEnd(start_point[0] + (meap.aux_mux_vals[6] + sample_offset) * sample_range);
              sample[0].start(start_point[0]);
              sample_gain[0] = 255;
            } else {
              chordTreeStep();
            }
          } else {  // button 0 released
            if (current_button_mode == kNORMAL) {
              sample_gain[0] = 0;
            }
          }
          break;
        case 1:
          if (button_vals[i]) {  // button 1 pressed
            if (current_button_mode == kNORMAL) {
              start_point[1] = meap.irand(0, 2626188);
              sample[1].setEnd(start_point[1] + (meap.aux_mux_vals[4] + sample_offset) * sample_range);
              sample[1].start(start_point[1]);
              sample_gain[1] = 255;
            } else {
              tempoTreeStep();
            }
          } else {  // button 1 released
            if (current_button_mode == kNORMAL) {
              sample_gain[1] = 0;
            }
          }
          break;
        case 2:
          if (button_vals[i]) {  // button 2 pressed
            if (current_button_mode == kNORMAL) {
              start_point[2] = meap.irand(0, 2626188);
              sample[2].setEnd(start_point[2] + (meap.aux_mux_vals[5] + sample_offset) * sample_range);
              sample[2].start(start_point[2]);
              sample_gain[2] = 220;
            } else {
              densityTreeStep();
            }
          } else {  // button 2 released
            if (current_button_mode == kNORMAL) {
              sample_gain[2] = 0;
            }
          }
          break;
        case 3:
          if (button_vals[i]) {  // button 3 pressed
            if (current_button_mode == kNORMAL) {
              start_point[3] = meap.irand(0, 2626188);
              sample[3].setEnd(start_point[3] + (meap.aux_mux_vals[3] + sample_offset) * sample_range);
              sample[3].start(start_point[3]);
              sample_gain[3] = 320;
            } else {
              timbreTreeStep();
            }
          } else {  // button 3 released
            if (current_button_mode == kNORMAL) {
              sample_gain[3] = 0;
            }
          }
          break;
        case 4:
          if (button_vals[i]) {  // button 4 pressed
            if (current_button_mode == kNORMAL) {
              granular_enable = true;
            } else {
              generatePattern();
            }
          } else {  // button 4 released
            if (current_button_mode == kNORMAL) {
              granular_enable = false;
            }
          }
          break;
        case 5:
          if (button_vals[i]) {  // button 5 pressed
            if (current_button_mode == kNORMAL) {
              saw_enable = true;
              tempo_pot_reading = meap.aux_mux_vals[5];
              tempo_pot_enable = false;
              saw_transpose = -2;
            } else {
              for (int i = 0; i < PULSE_VOICES; i++) {
                int random_sample = meap.irand(0, 4095);
                if (random_sample < meap.aux_mux_vals[6]) {
                  pulse_sample_num[i] = timbre_pairs[timbre_level][0];
                } else {
                  pulse_sample_num[i] = timbre_pairs[timbre_level][1];
                }
              }
            }
          } else {  // button 5 released
            if (current_button_mode == kNORMAL) {
              saw_enable = false;
            }
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 pressed
            if (current_button_mode == kNORMAL) {
              current_knob_mode = kCHOP;
            } else {
              for (int i = 0; i < PULSE_VOICES; i++) {
                pulse_sample_num[i] = (pulse_sample_num[i] - 1);
                if (pulse_sample_num[i] < 0) {
                  pulse_sample_num[i] = 0;
                }
              }
            }
          } else {  // button 6 released
            if (current_button_mode == kNORMAL) {
              current_knob_mode = kDRONE;
            }
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
  }
}

void sawControl() {
  trigger = kTriggerDelay.ready();
  unsigned long curr_time = micros();
  if (curr_time > saw_timer) {
    trigger = true;
    saw_timer = curr_time + saw_tempo_micros;
  } else {
    trigger = false;
  }

  // gainF = areadGain(mozziAnalogRead(5));
  // gainF = areadGain(meap.aux_mux_vals[6] / 5);  // VOLUME CONTROL
  gainF = 310;
  resonance = map(meap.aux_mux_vals[6], 0, 4095, 0, 246);


  // int tempo = areadTempo(mozziAnalogRead(4));
  // int tempo = areadTempo(meap.aux_mux_vals[5] / 4);  // TEMPO CONTROL ---- backwards //70bpm
  int curr_tempo_pot_reading = meap.aux_mux_vals[5];
  // if (curr_tempo_pot_reading > (tempo_pot_reading + 20) || curr_tempo_pot_reading < (tempo_pot_reading - 20)) {
  //   tempo_pot_enable = true;
  // }

  // if (tempo_pot_enable) {
  saw_tempo_micros = 24 * meap.midiPulseMicros(map(curr_tempo_pot_reading, 0, 4095, 70, 840));
  // } else {
  // saw_tempo_micros = 24 * meap.midiPulseMicros(140);
  // }


  // char mod1 = rand(8) - 4;
  // char mod2 = rand(8) - 4;
  // char mod3 = rand(8) - 4;
  // char mod4 = rand(8) - 4;

  char mod1 = 0;
  char mod2 = 0;
  char mod3 = 0;
  char mod4 = 0;

  // byte lf1 = map(mozziAnalogRead(0), 0, 1024, 80, 240);
  byte lf1 = map(meap.aux_mux_vals[4], 0, 4095, 80, 240);  // FILTER CUTOFF
  byte lf2 = map(lf1, 100, 180, 40, 70);

  // int ads = areadAds(mozziAnalogRead(1));
  int ads = areadAds(meap.pot_vals[1] / 4);  // CONTROLS ATTACK
  // int ads2 = areadAds2(mozziAnalogRead(2));
  int ads2 = areadAds2(meap.pot_vals[0] / 4);  // CONTROLS DECAY SUSTAIN AND RELEASE

  int att1 = ads;
  int de1 = ads2 + 40;
  int sus1 = ads2 + 10;
  int re1 = ads2 + 40;

  int att2 = ads;
  int de2 = ads2 + 20;
  int sus2 = ads2;
  int re2 = ads2 + 20;

  //  int att1=map(ads,0,1024,5,200);
  //  int de1=map(ads2,0,1024,10,450);
  //  int sus1=map(ads2,0,1024,10,450);
  //int re1=map(ads2,0,1024,10,450);


  //int att2=map(ads,0,1024,5,150);
  // int de2=map(ads2,0,1024,5,400);
  //int sus2=map(ads2,0,1024,5,400);
  // int re2=map(ads2,0,1024,5,400);


  //   char ac=12;
  //  int tt=map(mozziAnalogRead(1),0,1024,2,20);

  attackBa1 = att1;
  decayBa1 = de1;
  sustainBa1 = sus1;
  releaseBa1 = re1;

  attackBa2 = att2;
  decayBa2 = de2;
  sustainBa2 = sus2;
  releaseBa2 = re2;

  if (trigger) {  // sequencing
    // triWarm.setFreq(((int)rand((int)map(mozziAnalogRead(2),0,1024,0,12))+1)*80);
    //  triWarm1.setFreq( mtof( (int)melody1[(int)rand(6)]+(int)((rand((int)map(mozziAnalogRead(2),0,1024,1,5))+2)*12)  )  );
    //  triWarm2.setFreq( mtof( (int)melody1[(int)rand(6)]+(int)((rand((int)map(mozziAnalogRead(2),0,1024,2,6))+2)*12)  )  );
    //  triWarm3.setFreq( mtof( (int)melody1[(int)rand(6)]+(int)((rand((int)map(mozziAnalogRead(2),0,1024,2,4))+2)*12)  )  );
    //  triWarm4.setFreq( mtof( (int)melody1[(int)rand(6)]+(int)((rand((int)map(mozziAnalogRead(2),0,1024,3,5))+2)*12)  )  );

    //  triWarm1.setFreq( mtof( (byte)melody1[(byte)map(mozziAnalogRead(3),0,1024,0,19)][0] +12 ))  ;    //0 1 4 6
    //  triWarm2.setFreq( mtof( (byte)melody1[(byte)map(mozziAnalogRead(3),0,1024,0,19)][1] +12))  ;
    //  triWarm3.setFreq( mtof( (byte)melody1[(byte)map(mozziAnalogRead(3),0,1024,0,19)][2] +12))  ;
    //  triWarm4.setFreq( mtof( (byte)melody1[(byte)map(mozziAnalogRead(3),0,1024,0,19)][3] +12))  ;
    // triWarm1.setFreq( mtof( (byte)melody[(byte)map(mozziAnalogRead(3),0,1024,0,47)] +12 ))  ;
    // triWarm1.setFreq(mtof((byte)melody[melcount + ((byte)map(mozziAnalogRead(3), 0, 1024, 0, 32) * 4)]) + mod1);
    triWarm1.setFreq(mtof((byte)melody[melcount + ((byte)map(meap.aux_mux_vals[3], 0, 4095, 0, 32) * 4)] + saw_transpose) + mod1);

    triWarm2.setFreq(mtof((byte)melody1[count1][0] + saw_transpose) + mod2);
    triWarm3.setFreq(mtof((byte)melody1[count1][1] + saw_transpose) + mod3);
    triWarm4.setFreq(mtof((byte)melody1[count1][2] + saw_transpose) + mod4);

    envB1.setADLevels((byte)250, (byte)150);
    envB1.setTimes(attackBa1, decayBa1, sustainBa1, releaseBa1);
    envB1.noteOn();

    envB2.setADLevels((byte)lf1, (byte)lf2);
    envB2.setTimes(attackBa2, decayBa2, sustainBa2, releaseBa2);
    envB2.noteOn();

    // filt.set(lf1,lf2, tc);
    count += 1;
    melcount += 1;
    if (count == 11) {
      count = 0;
      count1 += 1;
    }
    if (count1 == 16) {
      count1 = 0;
      randmel = rand(120);
    }

    if (melcount == 5) {
      melcount = 0;
    }
  }  //  kTrigger
  //   freq=filt.next();
  lpf.setCutoffFreqAndResonance(freq, resonance);
}

void sawSetup() {
  kTriggerDelay.start(100);
  check = 0;
}


void samplePulse(int voice_num, int midi_note_num, int pulse_sample_num) {
  //set sample

  pulse_sample[voice_num].setTable(samples_list[pulse_sample_num]);
  pulse_sample[voice_num].setEnd(samples_lengths[pulse_sample_num]);

  //set note
  pulse_freq[voice_num] = sample_freq_table[midi_note_num + pulse_transpose];
  pulse_sample[voice_num].setFreq(pulse_freq[voice_num]);
  pulse_sample[voice_num].start();

  //set envelope
  int sustain_min = map(clock_bpm, 30, 120, 40, 20);
  int sustain_max = map(clock_bpm, 30, 120, 1000, 500);
  pulse_sustain_time = map(pulse_loop_step[voice_num], 1, 48, sustain_min, sustain_max);
  int atk = pulse_sustain_time / 4;
  int rel = pulse_sustain_time / 2;
  pulse_env[voice_num].setAttackTime(atk);
  pulse_env[voice_num].setSustainTime(pulse_sustain_time);
  pulse_env[voice_num].setReleaseTime(rel);
  pulse_env[voice_num].noteOn();
}

void generatePattern() {
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_pan[i] = meap.irand(-1, 1);
    // choose timbre

    pulse_curr_step[i] = meap.irand(0, 24);

    pulse_loop_step[i] = rhythms[meap.irand(0, 7)] * 2;
  }
}

void chordTreeStep() {
  // --------------------WHAT STATE SHOULD WE MOVE TO NEXT?--------------------------------------
  if (chord_tree_level != 0) {
    chord_tree_level = chord_tree_level - 1;  // move one level down the tree
  } else {
    chord_tree_level = meap.irand(0, 4);  // if already at bottom of tree jump to a random level
  }

  // --------------------WHAT HAPPENS IN EACH STATE?-------------------------------------------
  switch (chord_tree_level) {  // choose chord based on tree level
    case 4:                    // leaves : iii
      chord_root = 2;          // iii chord (E minor)
      break;
    case 3:                         // twigs: I or vi
      if (meap.irand(0, 3) == 0) {  // 25% chance of I, 75% chance of vi
        chord_root = 0;             // I chord (C major)
      } else {
        chord_root = 5;  // vi chord (A minor)
      }
      break;
    case 2:                         // branches: IV or ii
      if (meap.irand(0, 2) == 0) {  // 33% chance of ii, 66% chance off IV
        chord_root = 1;             // ii chord (D minor)
      } else {
        chord_root = 3;  // IV chord (F major)
      }
      break;
    case 1:                         // boughs: V or vii˚
      if (meap.irand(0, 3) == 0) {  // 25% chance of vii˚, 75% chance of V
        chord_root = 6;             // vii˚ chord (B diminished)
      } else {
        chord_root = 4;  // V chord (G major)
      }
      break;
    case 0:                         // trunk: I or vi
      if (meap.irand(0, 3) == 0) {  // 25% chance of vi, 75% chance of I
        chord_root = 5;             // vi chord (A minor)
      } else {
        chord_root = 0;  // I chord (C major)
      }
      break;
  }
  note_bank[0] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[1] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[2] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[3] = scale_root + majScale[(triad[0] + chord_root) % 7] - 12;
  note_bank[4] = scale_root + majScale[(triad[1] + chord_root) % 7] - 12;
  note_bank[5] = scale_root + majScale[(triad[2] + chord_root) % 7] - 12;
  note_bank[6] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[7] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[8] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[9] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[10] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[11] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[12] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[13] = scale_root + majScale[(triad[2] + chord_root) % 7];

  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_note[i] = note_bank[meap.irand(0, 13)];
  }
}

void tempoTreeStep() {
  if (tempo_level == 0) {
    tempo_level++;
  } else if (tempo_level == 4) {
    tempo_level--;
  } else {
    tempo_level = tempo_level + (1 - 2 * meap.irand(0, 1));
  }
  updateTempo(tempos[tempo_level]);
}

void densityTreeStep() {
  density_value += meap.irand(1, 3) * density_direction;
  if (density_value > PULSE_VOICES) {
    density_value = PULSE_VOICES;
    density_direction = -1;
  } else if (density_value < 2) {
    density_value = 2;
    density_direction = 1;
  }

  for (int i = 0; i < PULSE_VOICES; i++) {
    if (i < density_value) {
      pulse_active[i] = true;
    } else {
      pulse_active[i] = false;
    }
  }
}

void initializePulse() {
  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_pan[i] = meap.irand(-1, 1);
    pulse_curr_step[i] = meap.irand(0, 24);
    pulse_loop_step[i] = rhythms[meap.irand(0, 7)] * 2;
  }
  updateTempo(tempos[tempo_level]);

  note_bank[0] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[1] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[2] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[3] = scale_root + majScale[(triad[0] + chord_root) % 7] - 12;
  note_bank[4] = scale_root + majScale[(triad[1] + chord_root) % 7] - 12;
  note_bank[5] = scale_root + majScale[(triad[2] + chord_root) % 7] - 12;
  note_bank[6] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[7] = scale_root + majScale[(triad[1] + chord_root) % 7];
  note_bank[8] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[9] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[10] = scale_root + majScale[(triad[0] + chord_root) % 7] - 24;
  note_bank[11] = scale_root + majScale[(triad[2] + chord_root) % 7];
  note_bank[12] = scale_root + majScale[(triad[0] + chord_root) % 7];
  note_bank[13] = scale_root + majScale[(triad[2] + chord_root) % 7];

  for (int i = 0; i < PULSE_VOICES; i++) {
    pulse_note[i] = note_bank[meap.irand(0, 13)];
  }

  for (int i = 0; i < PULSE_VOICES; i++) {
    if (i < density_value) {
      pulse_active[i] = true;
    } else {
      pulse_active[i] = false;
    }
  }
}

void updateTempo(float new_tempo) {
  clock_bpm = new_tempo;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);
}

void timbreTreeStep() {
  int rand_num = 0;
  switch (timbre_level) {
    case 0:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 1;
      } else {
        timbre_level = 7;
      }
      break;
    case 1:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 2;
      } else {
        timbre_level = 4;
      }
      break;
    case 2:
      timbre_level = 3;
      break;
    case 3:
      timbre_level = 12;
      break;
    case 4:
      timbre_level = 5;
      break;
    case 5:
      timbre_level = 6;
      break;
    case 6:
      timbre_level = 12;
      break;
    case 7:
      rand_num = meap.irand(0, 1);
      if (rand_num == 0) {
        timbre_level = 8;
      } else {
        timbre_level = 10;
      }
      break;
    case 8:
      timbre_level = 9;
      break;
    case 9:
      timbre_level = 12;
      break;
    case 10:
      timbre_level = 11;
      break;
    case 11:
      timbre_level = 12;
      break;
    case 12:
      timbre_level = 13;
      break;
    case 13:
      timbre_level = 0;
      break;
  }
  for (int i = 0; i < PULSE_VOICES; i++) {
    int random_sample = meap.irand(0, 4095);
    if (random_sample < meap.aux_mux_vals[6]) {
      pulse_sample_num[i] = timbre_pairs[timbre_level][0];
    } else {
      pulse_sample_num[i] = timbre_pairs[timbre_level][1];
    }
  }
}


//executes when a clock step is received
void clockStep() {
  if (current_button_mode == kPULSE) {
    for (int i = 0; i < PULSE_VOICES; i++) {
      if (pulse_active[i]) {
        if (pulse_curr_step[i] == 0) {
          samplePulse(i, pulse_note[i], pulse_sample_num[i]);
        }
        pulse_curr_step[i]--;
        if (pulse_curr_step[i] < 0) {
          pulse_curr_step[i] = pulse_loop_step[i] - 1;
        }
      }
    }
  }

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