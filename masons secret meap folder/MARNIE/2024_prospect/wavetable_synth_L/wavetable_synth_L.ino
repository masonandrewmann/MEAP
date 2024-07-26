/*
  Basic template for a MARNIE L program with clock and midi
 */

#include <Meap.h>
#include "wavetable_includes.h"
#include "sample_includes.h"
#include <tables/cos2048_int8.h>  // table for Oscils to play

#include "SamplePattern.h"
#include "patterns.h"
#include "sample_frequencies.h"

#include "love_song.h"


#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define POLYPHONY 6

#define sPOLYPHONY 10              // How many voices
#define sMAX_SAMPLE_LENGTH 300000  // Max length of a sample

#define NUM_SAMPLE_TRACKS 4
#define NUM_GRANULAR 20

enum ClockModes {
  kINTERNAL,
  kEXTERNAL
} clock_mode;

enum KeyboardModes {
  kSINES,
  kWAVETABLES
} keyboard_mode = kWAVETABLES;

enum PadModes {
  kLOOPS,
  kSILENCE
} pad_mode = kLOOPS;

enum AudioModes {
  kSILENCEA,
  kCHOPA,
  kMAINA
} audio_mode = kMAINA;

enum CtrlModes {
  kMAIN,
  kAUX
} ctrl_mode = kMAIN;

enum LooperModes {
  kOff,
  kRecording,
  kPlaying,
  kOverdubbing,
  kPaused
} looper_mode;

int button_pins[6] = { 15, 16, 12, 13, 18, 35 };
int button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };
int last_button_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); /**< MEAP hardware serial port*/

// MIDI clock timer
uint32_t clock_timer = 0;
uint32_t clock_period_micros = 10000;
int clock_pulse_num = 0;
float clock_bpm = 80;

Meap meap;

mWavetableSynth<AUDIO_RATE, CONTROL_RATE, POLYPHONY, analog_table_FRAME_SIZE, analog_table_NUM_FRAMES> wave(analog_table_DATA, NULL);

Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> mod(COS2048_DATA);
int16_t mod_val = 0;

uint32_t freq_val = 440;
uint16_t frame_val = 0;
float alpha = 0.91;

int attack_time = 1;
int release_time = 500;

MultiResonantFilter<uint16_t> filter; /**< Filter for left channel */
int32_t cutoff = 10000;
int32_t resonance = 30000;
int32_t rand_cutoff_range = 0;

mOscBank<COS2048_NUM_CELLS, AUDIO_RATE, CONTROL_RATE, 10> bank(COS2048_DATA, NULL);

mSample16<333262, AUDIO_RATE> loops[8];

int16_t loop_gains[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool loop_active[8] = { false, false, false, false, false, false, false, false };



//  ---------- silence variables start ----------

int current_voice = 0;
/**
* Keeps track of a single note turned on by a SamplePattern object
*/
class PatternNote {
public:
  uint8_t note_num;    /**< MIDI note number */
  uint16_t voice_num;  /**< Num of sample voice sounding this note */
  uint8_t channel_num; /**< MIDI channel number to output on */
  uint32_t end_time;   /**< Time to end the note automatically */
};

LinkedList<PatternNote *> patternQueue = LinkedList<PatternNote *>();

// for easing timing of pattern starting
uint32_t last_sixteenth_time = 0;     /**< keeps track of the time in ms that the last 16th note happened */
uint32_t half_sixteenth_length = 100; /**< half of the period in ms of one sixteenth note */

bool glock_enable = false;

// Sample voices
mSample<sMAX_SAMPLE_LENGTH, AUDIO_RATE> sil_sample[sPOLYPHONY];  //maybe change the sMAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float s_default_freq;                                            //default freq of a sample, calculated using length of sample and sample rate
float s_sample_freq[sPOLYPHONY];                                 /**< stores current frequency of each sample voice */

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sil_sample_env[sPOLYPHONY]; /**< ADSR envelopes for all sample voices */
int sil_sample_gain[sPOLYPHONY];
int sil_attack_time = 1;
int sil_release_time = 500;

// ---------- silence variables end ----------


// ---------- chopping variables start ----------

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

// // MIDI clock timer
// uint32_t clock_timer = 0;
// uint32_t clock_period_micros = 10000;
// int clock_pulse_num = 0;
// float clock_bpm = 120;

// // ---------- chopping variables end ----------


// ----------looper variables start----------
// think ill have a problem if i play the same note on
// multiple programs, they will turn each other off :~(
class LooperNote {
public:
  uint16_t message_type;  // 0 for note off, 1 for note on
  uint16_t pitch;         // midi note number
  uint16_t program;       //
  uint64_t time_offset;   // offset from start of loop in ms
  bool triggered;         // was this note already triggered this time through the loop?
  //

  LooperNote(uint16_t _message_type, uint16_t _pitch, uint16_t _program, uint64_t _time_offset, bool _triggered) {
    message_type = _message_type;
    pitch = _pitch;
    program = _program;
    time_offset = _time_offset;
    triggered = _triggered;
  }
};

uint64_t loop_start_time = 0;
uint64_t loop_end_offset = 1000;
uint64_t loop_end_time = 1000;
uint64_t pause_time = 0;
uint16_t looper_random_chance = 127;

LinkedList<LooperNote *> looperNoteQueue = LinkedList<LooperNote *>();

// ----------looper variables end----------

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();

  clock_mode = kINTERNAL;
  clock_period_micros = meap.midiPulseMicros(clock_bpm);
  half_sixteenth_length = clock_period_micros * 3 / 1000;

  for (int i = 0; i < 6; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  mod.setFreq(.768f);  // sync with kNoteChangeDelay

  bank.setTranspose(12);
  bank.enableDetune();

  float default_freq = 32768.f / 333262.f;
  for (uint8_t i = 0; i < 8; i++) {
    loops[i].setTable(loop_list[i]);
    loops[i].setEnd(loop_lengths[i]);
    loops[i].setFreq(default_freq);
    loops[i].setLoopingOn();
  }
  loops[4].setFreq(default_freq / 2);

  // ---------- silence setup start ----------

  patternSetup();

  // initialize samples
  s_default_freq = (float)piano_SAMPLERATE / (float)sMAX_SAMPLE_LENGTH;
  for (int i = 0; i < sPOLYPHONY; i++) {
    sil_sample[i].setTable(piano_DATA);
    sil_sample[i].setEnd(piano_NUM_CELLS);
    sil_sample[i].setLoopingOff();
    s_sample_freq[i] = s_default_freq;
    sil_sample[i].setFreq(s_sample_freq[i]);
    sil_sample_gain[i] = 0;
    sil_sample_env[i].setTimes(sil_attack_time, 1, 4294967295, sil_release_time);
    sil_sample_env[i].setADLevels(255, 255);
  }

  filter.setCutoffFreq(cutoff);
  filter.setResonance(resonance);

  rand_cutoff_range = 50000;
  // ---------- silence setup end ----------

  // ---------- chopping setup start ----------

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

  // ---------- chopping setup end ----------

  looper_mode = kOff;
  meap.turnLedDOff();
}


void loop() {
  audioHook();

  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }

  // handle generating midi clock
  // if (clock_mode == kINTERNAL) {
    uint32_t t = micros();
    if (t > clock_timer) {
      clock_timer = t + clock_period_micros;
      // MIDI.sendRealTime(midi::Clock);
      clockStep();
    }
  // }
}


void updateControl() {
  meap.readPots();
  meap.readTouch();
  // meap.readDip();
  meap.readAuxMux();
  updateButtons();
  
  switch (audio_mode) {
    case kMAINA:
      mainControl();
      break;
    case kCHOPA:
      updateChops();
      updateGranular();
      break;
    case kSILENCEA:
      silenceControl();
      break;
  }

  updateLooper();
}


AudioOutput_t updateAudio() {
  uint64_t immediate_sample = 0;


  switch (audio_mode) {
    case kMAINA:
      immediate_sample += wave.next();
      immediate_sample += bank.next();

      for (int i = 0; i < 8; i++) {
        immediate_sample += (loops[i].next() * loop_gains[i]) << 2;
      }
      filter.next(immediate_sample);
      immediate_sample = filter.low();
      break;

    case kSILENCEA:
      for (int i = 0; i < sPOLYPHONY; i++) {
        sil_sample_gain[i] = sil_sample_env[i].next();
        immediate_sample += sil_sample[i].next() * sil_sample_gain[i];
      }
      filter.next(immediate_sample);
      immediate_sample = filter.low();
      break;

    case kCHOPA:
      for (int i = 0; i < NUM_SAMPLE_TRACKS; i++) {
        immediate_sample += sample[i].next() * sample_gain[i];
      }
      int32_t granular_component = 0;
      if (granular_enable) {
        for (int i = 0; i < NUM_GRANULAR; i++) {
          granular_env[i].update();
          granular_gain[i] = granular_env[i].next();
          granular_component += granular[i].next() * granular_gain[i];
        }
      }
      immediate_sample += (granular_component >> 1);
      break;
  }
  return StereoOutput::fromNBit(20, immediate_sample, immediate_sample);
}

void Meap::updateTouch(int number, bool pressed) {
  if (pad_mode == kLOOPS) {
    if (pressed) {
      if (loop_active[number]) {
        loop_gains[number] = 0;
        loop_active[number] = false;
      } else {
        loops[number].start();
        loop_gains[number] = 1;
        loop_active[number] = true;
      }
    }
  } else if (pad_mode == kSILENCE) {
    if (pressed) {
      if (patterns[0][number].active) {
        patterns[0][number].stop();
      } else {
        patterns[0][number].start();
        // check closest sixteenth note here! using ppq???? or maybe i store time with millis or micros?
        uint32_t curr_time = millis();
        if (curr_time > last_sixteenth_time && curr_time < last_sixteenth_time + half_sixteenth_length) {
          //closest sixteenth is in the past,, do something here!!!
          // use step function
          patterns[0][number].start_flag = false;
          step(patterns[0][number]);
        }
      }
    }
  }

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
            start_point[1] = meap.irand(600000, 800000);
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
            start_point[3] = meap.irand(200000, 400000);
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
          if (button_vals[i]) {  // button 5 down
            keyboard_mode = kSINES;
          } else {  // button 5 up
            keyboard_mode = kWAVETABLES;
          }
          break;
        case 6:
          if (button_vals[i]) {  // button 6 down
            if (ctrl_mode == kAUX) {
              pad_mode = kSILENCE;
              audio_mode = kSILENCEA;
            }
          } else {  // button 6 up
            if (ctrl_mode == kAUX) {
              audio_mode = kCHOPA;
            }
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
  // debug(MIDI.getType());
  // debug(" ");
  // debug(data1);
  // debug(" ");
  // debugln(data2);
  switch (MIDI.getType())  // Get the type of the message we caught
  {
    case midi::NoteOn:       // ---------- MIDI NOTE ON RECEIVED ----------
      if (channel == 15) {   // special commands channel
        if (data1 == 100) {  // looping message from marnie R on note 100 vel 0
          if (data2 == 0) {  // turn off looping
            for (uint8_t i = 0; i < sPOLYPHONY; i++) {
              sil_sample[i].setLoopingOff();
            }
          } else if (data2 == 1) {  // turn on looping on note 100 vel 1
            for (uint8_t i = 0; i < sPOLYPHONY; i++) {
              sil_sample[i].setLoopingOn();
            }
          }
        } else if (data1 == 101) {  // switch between aux and main modes on note 101 vel 0
          if (data2 == 0) {         // go to main
            ctrl_mode = kMAIN;
            pad_mode = kLOOPS;
            audio_mode = kMAINA;
          } else if (data2 == 1) {  // go to aux (pad audio need change) on note 101 vel 1
            ctrl_mode = kAUX;
            //figure out which aux mode we are entering
            if (button_vals[6]) {  // entering silence
              pad_mode = kSILENCE;
              audio_mode = kSILENCEA;
            } else {  // entering chops
              audio_mode = kCHOPA;
            }
          }
        }

      } else {  // normal keyboard channels
        noteOnHandler(data1, data2, channel, -1);
      }
      break;
    case midi::NoteOff:  // ---------- MIDI NOTE OFF RECEIVED ----------
      noteOffHandler(data1, channel);
      break;
    case midi::ProgramChange:  // ---------- MIDI PROGRAM CHANGE RECEIVED ----------
      if (data1 >= 0 && data1 < NUM_WAVETABLES) {
        wave.setTable(wavetables_list[data1]);
      }
      break;
    case midi::ControlChange:  // ---------- MIDI CONTROL CHANGE RECEIVED ----------
      switch (data1) {
        case 1:  // bottom pot -- loop speed
          break;
        case 12:  // top pot -- loop density
          looper_random_chance = data2;
          break;
        case 20:  // big button
          // debugln(data2);
          if (data2 == 127) {
            switch (looper_mode) {
              case kOff:
                looper_mode = kRecording;
                meap.turnLedDOn();
                loop_start_time = millis();
                debugln("looper recording");
                break;
              case kRecording:
                loop_end_time = millis();
                loop_end_offset = loop_end_time - loop_start_time;
                looper_mode = kPlaying;
                meap.turnLedDOff();
                debugln("looper playing");
                break;
              case kPlaying:
                looper_mode = kOverdubbing;
                meap.turnLedDOn();
                debugln("looper overdubbing");
                break;
              case kOverdubbing:
                looper_mode = kPlaying;
                meap.turnLedDOff();
                debugln("looper playing");
                break;
            }
          }
          break;
        case 15:  // top green button
          if (data2 == 127) {
            if (looper_mode == kPaused) {
              looper_mode = kPlaying;
              loop_start_time = millis() - pause_time;
              loop_end_time = loop_start_time + loop_end_offset;
              debugln("looper playing");
            } else {
              looper_mode = kPaused;
              pause_time = millis() - loop_start_time;
              debugln("looper paused");
              bank.flush();
              wave.flush();


              // //turn off active notes
              // uint16_t num_active_notes = computerNoteQueue.size();
              // // delete tagged notes starting at end of array
              // for (int i = num_active_notes - 1; i >= 0; i--) {
              //   sample_env[computerNoteQueue.get(i)->voice_num].noteOff();  // turn off note
              //   delete (computerNoteQueue.get(i));
              //   computerNoteQueue.remove(i);

              // REPLACE WITH SMTH TO STOP ALL NOTES HERE,, maybe make a method in bank class!!
              // }
            }
          }
          break;
        case 14:  // bottom green button

          if (data2 == 127) {
            looper_mode = kOff;
            for (int16_t i = looperNoteQueue.size() - 1; i >= 0; i--) {
              delete (looperNoteQueue.get(i));
              looperNoteQueue.remove(i);
            }
            // uint16_t num_active_notes = computerNoteQueue.size();
            // // delete tagged notes starting at end of array
            // for (int i = num_active_notes - 1; i >= 0; i--) {
            //   sample_env[computerNoteQueue.get(i)->voice_num].noteOff();  // turn off note
            //   delete (computerNoteQueue.get(i));
            //   computerNoteQueue.remove(i);
            // }

            // REPLACE WITH SMTH TO STOP ALL NOTES HERE

            //clear cpu note,, send notse off
            debugln("looper off");
          }
          break;
      }
      break;
    case midi::PitchBend:  // ---------- MIDI PITCH BEND RECEIVED ----------
      break;
    case midi::Clock:  // ---------- MIDI CLOCK PULSE RECEIVED ----------
      // if (clock_mode == kEXTERNAL) {
      //   clockStep();
      // }
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
    if (audio_mode == kSILENCEA) {
      randomizeCutoff(rand_cutoff_range);  // randomize the cutoff
      if (meap.aux_mux_vals[1] > 30) {     // play a glockenspiel!
        int notes[] = { 67, 72, 76 };
        int index = meap.irand(map(meap.aux_mux_vals[1], 0, 4095, -50, 0), 2);
        int my_note = notes[index];
        if (meap.irand(0, 1) == 1) {
          my_note -= 12;
        }
        if (index >= 0) {
          current_voice = (current_voice + 1) % sPOLYPHONY;
          sil_sample[current_voice].setTable(samples_list[5]);
          sil_sample[current_voice].setEnd(samples_lengths[5]);
          sil_sample_env[current_voice].setAttackTime(sil_attack_time >> 1 + 1);
          sil_sample_env[current_voice].setReleaseTime(sil_release_time >> 1 + 1);

          // calculate frequency offset for sample
          s_sample_freq[current_voice] = sample_freq_table[my_note];

          // start the note
          sil_sample[current_voice].setFreq(s_sample_freq[current_voice]);  // set frequency of sample
          sil_sample_env[current_voice].noteOn();
          sil_sample[current_voice].start();

          sil_sample_env[current_voice].noteOff();
        }
      }
    }
  }

  if (clock_pulse_num % 6 == 0) {  // sixteenth note
    if (audio_mode == kSILENCEA) {
      for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
        for (int i = 0; i < 8; i++) {
          patterns[bank][i].incrementClock(clock_pulse_num);
        }
      }
      if (meap.irand(0, 4) == 0) {
        randomizeCutoff(rand_cutoff_range);
      }
    }
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

  if (ctrl_mode == kMAIN) {
    // store note in looper linked list here!~!!
    if (looper_mode == kRecording || looper_mode == kOverdubbing) {
      // calculate time offset
      uint64_t my_time_offset = millis() - loop_start_time;
      uint16_t my_program;
      if (keyboard_mode == kSINES) {
        my_program = 0;
      } else {
        my_program = 1;
      }
      LooperNote *my_note = new LooperNote(1, note, my_program, my_time_offset, true);  // create object to keep track of note num, voice num and freq
      looperNoteQueue.add(my_note);                                                     // add to looper notes queue
      debug("add note on ");
      debugln(my_time_offset);
    }
  }

  if (keyboard_mode == kSINES) {
    bank.noteOn(note);
  } else if (keyboard_mode == kWAVETABLES) {
    wave.noteOn(note);
  }
}



/**
* @brief Handles turning off sample notes.
*
* @param note is the MIDI note number to turn off
* @param channel is the MIDI channel of the note to turn off
*/
void noteOffHandler(int note, int channel) {
  // store note in looper linked list here!~!!
  if (looper_mode == kRecording || looper_mode == kOverdubbing) {
    // calculate time offset
    uint64_t my_time_offset = millis() - loop_start_time;
    uint16_t my_program;
    if (keyboard_mode == kSINES) {
      my_program = 0;
    } else {
      my_program = 1;
    }
    LooperNote *my_note = new LooperNote(0, note, my_program, my_time_offset, true);  // create object to keep track of note num, voice num and freq
    looperNoteQueue.add(my_note);                                                       // add to looper notes queue
  }
  if (keyboard_mode == kSINES) {
    bank.noteOff(note);
  } else if (keyboard_mode == kWAVETABLES) {
    wave.noteOff(note);
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
        sil_sample_env[my_pattern_note->voice_num].noteOff();  // turn off note
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

  my_pattern.sclock = 0;
  my_pattern.next_step_time = my_pattern.dur[my_pattern.curr_step];

  if (note != 0) {
    if (my_pattern.sample_num != -1) {
      //find a free voice
      current_voice = (current_voice + 1) % sPOLYPHONY;
      int my_voice = current_voice;

      sil_sample[my_voice].setTable(samples_list[my_pattern.sample_num]);
      sil_sample[my_voice].setEnd(samples_lengths[my_pattern.sample_num]);
      sil_sample_env[my_voice].setAttackTime(map(sil_attack_time, 0, 4095, 1, 2000));
      sil_sample_env[my_voice].setReleaseTime(map(sil_release_time, 0, 4095, 1, 2000));

      // calculate frequency offset for sample
      s_sample_freq[my_voice] = sample_freq_table[note];

      // start the note
      sil_sample[my_voice].setFreq(s_sample_freq[my_voice]);  // set frequency of sample
      sil_sample_env[my_voice].noteOn();
      sil_sample[my_voice].start();


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
    sil_sample_env[my_pattern_note->voice_num].noteOff();
    int8_t pattern_midi_channel = my_pattern_note->channel_num;
    MIDI.sendNoteOff(my_pattern_note->voice_num, 0, pattern_midi_channel);
    delete (patternQueue.get(i));
    patternQueue.remove(i);
  }
}

/**
* @brief Randomizes cutoff of filter.
*
* @param range is the range to randomize filter over in "percentage"
*/
void randomizeCutoff(int32_t range) {
  // if (range > 3) {  // add a slight deadzone at the bottom of the pot
  range = range << 1;  // maps input range to 0-200
  int rand_cutoff = cutoff + meap.irand(0, range);
  // clip to range 0-255
  if (rand_cutoff > 63000) {
    rand_cutoff = 63000;
  } else if (rand_cutoff < 0) {
    rand_cutoff = 0;
  }
  filter.setCutoffFreq(rand_cutoff);
  // }
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
  }
}

void mainControl() {
  wave.update();
  bank.update();

  filter.setCutoffFreq(meap.aux_mux_vals[4] << 4);
  filter.setResonance(meap.aux_mux_vals[3] << 4);

  attack_time = meap.aux_mux_vals[6] << 2;
  release_time = meap.aux_mux_vals[5] << 2;

  wave.setAttackTime(attack_time);
  wave.setReleaseTime(release_time);

  bank.setAttackTime(attack_time);
  bank.setReleaseTime(release_time);

  mod_val = mod.next();

  frame_val = (alpha)*frame_val + (1.0 - alpha) * (meap.pot_vals[1] >> 4);

  int16_t frame_mod = mod_val + frame_val;

  if (frame_mod > analog_table_NUM_FRAMES) {
    frame_mod -= analog_table_NUM_FRAMES;
  } else if (frame_mod < 0) {
    frame_mod += analog_table_NUM_FRAMES;
  }
  wave.setFrame(frame_mod);

  mod.setFreq((float)((float)meap.aux_mux_vals[2] / 2000.0));

  float amt = ((float)meap.pot_vals[0]) * 0.000008333333333 + 0.985;  // 1/120000
  bank.setDetuneAmount(amt);
}

void silenceControl() {
  // ---------- silence control start ----------
  float alpha = 0.96;
  int32_t prev_cutoff = cutoff;
  cutoff = (alpha)*cutoff + (1.0 - alpha) * (meap.aux_mux_vals[4] << 4);  // 4095 -> 65535

  if (cutoff > 63000) {
    cutoff = 63000;
  }

  if (cutoff > (prev_cutoff + 400) || cutoff < (prev_cutoff - 400)) {
    filter.setCutoffFreq(cutoff);
    prev_cutoff = cutoff;
  }


  resonance = meap.aux_mux_vals[3] << 4;  // 4095 -> 65535
  if (resonance > 60000) {
    resonance = 60000;
  }
  filter.setResonance(resonance);

  // update rand cutoff range
  rand_cutoff_range = meap.pot_vals[1] << 3;

  //handle pattern array
  for (int i = 0; i < 6; i++) {
    patterns[0][i].cycle();
    if (patterns[0][i].step_flag) {
      patterns[0][i].step_flag = false;
      step(patterns[0][i]);
    }
  }

  // ends notes triggered by patterns
  cleanPatternQueue();
  // ---------- silence control end ----------
}

void updateLooper() {
  if (looper_mode == kPlaying || looper_mode == kOverdubbing) {
    uint64_t curr_time = millis();
    // reached end of loop, reset to beginning
    if (curr_time > loop_end_time) {
      debugln("restarting loop");
      loop_start_time = millis();
      loop_end_time = loop_start_time + loop_end_offset;
      uint16_t num_notes = looperNoteQueue.size();
      for (uint16_t i = 0; i < num_notes; i++) {
        looperNoteQueue.get(i)->triggered = false;
      }
    }

    // checking for notes to trigger
    LooperNote *my_note;
    uint16_t num_notes = looperNoteQueue.size();
    for (uint16_t i = 0; i < num_notes; i++) {
      my_note = looperNoteQueue.get(i);
      if (!my_note->triggered && curr_time > (loop_start_time + my_note->time_offset)) {
        my_note->triggered = true;
        // trigger the event here!
        if (my_note->message_type == 1) {  // note on
          if (meap.irand(0, 127) <= looper_random_chance) {
            // computerNoteOn(my_note->pitch, 127, MIDI_NOTE_CHANNEL, my_note->program, 100000);
            // replace this with my own note on/off bits!

            if (my_note->program == 0) {
              bank.noteOn(my_note->pitch);
            } else if (my_note->program == 1) {
              wave.noteOn(my_note->pitch);
            }
          }
        } else {  // note off
          // computerNoteOff(my_note->pitch, MIDI_NOTE_CHANNEL, my_note->program);
          // replace this with my own note on/off bits!
          if (my_note->program == 0) {
            bank.noteOff(my_note->pitch);
          } else if (my_note->program == 1) {
            wave.noteOff(my_note->pitch);
          }
        }
      }
    }
  }
}