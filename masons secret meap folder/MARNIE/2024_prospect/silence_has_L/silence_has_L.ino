/*
  Basic template for a MARNIE L program with clock and midi
 */

#include <Meap.h>
#include "SamplePattern.h"
#include "patterns.h"
#include "sample_frequencies.h"
#include "sample_includes.h"

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

#define sPOLYPHONY 20              // How many voices
#define sMAX_SAMPLE_LENGTH 300000  // Max length of a sample

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
float clock_bpm = 80;

// for easing timing of pattern starting
uint32_t last_sixteenth_time = 0;     /**< keeps track of the time in ms that the last 16th note happened */
uint32_t half_sixteenth_length = 100; /**< half of the period in ms of one sixteenth note */

bool glock_enable = false;

Meap meap;

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


// Sample voices
mSample<sMAX_SAMPLE_LENGTH, AUDIO_RATE> sil_sample[sPOLYPHONY];  //maybe change the sMAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
float s_default_freq;                                        //default freq of a sample, calculated using length of sample and sample rate
float s_sample_freq[sPOLYPHONY];                              /**< stores current frequency of each sample voice */

ADSR<CONTROL_RATE, AUDIO_RATE, unsigned int> sil_sample_env[sPOLYPHONY]; /**< ADSR envelopes for all sample voices */
int sil_sample_gain[sPOLYPHONY];
int sil_attack_time = 1;
int sil_release_time = 500;

MultiResonantFilter<uint16_t> filter; /**< Filter for left channel */
int32_t cutoff = 10000;
int resonance = 30000;
int rand_cutoff_range = 0;

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

  // MARNIE Pot Controls
  // 1 pole smoothing here
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
  if(resonance > 60000){
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
}


AudioOutput_t updateAudio() {
  int out_sample = 0;

  for (int i = 0; i < sPOLYPHONY; i++) {
    // if (sil_sample_env[i].playing()) {
    sil_sample_gain[i] = sil_sample_env[i].next();
    out_sample += sil_sample[i].next() * sil_sample_gain[i];
    // }
  }

  filter.next(out_sample);
  out_sample = filter.low();

  return StereoOutput::fromAlmostNBit(20, out_sample, out_sample);
}

void Meap::updateTouch(int number, bool pressed) {

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
            for (uint8_t i = 0; i < sPOLYPHONY; i++) {
              sil_sample[i].setLoopingOn();
            }
          } else {  // button 6 up
            for (uint8_t i = 0; i < sPOLYPHONY; i++) {
              sil_sample[i].setLoopingOff();
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
    randomizeCutoff(rand_cutoff_range);
    if (meap.aux_mux_vals[1] > 30) {

      int notes[] = { 67, 72, 76 };
      int index = meap.irand(map(meap.aux_mux_vals[1], 0, 4095, -50, 0), 2);
      int my_note = notes[index];
      if (meap.irand(0, 1) == 1) {
        my_note -= 12;
      }
      if (index >= 0) {
        // noteOnHandler(my_note, 127, MIDI_NOTE_CHANNEL, 3);  // note vel channel override ---- NOTE THAT IF RELEASE IS DOWN THIS NOTE WILL NOT SOUND BECAUSE NOTE OFF IS SO SOON
        // noteOffHandler(my_note, MIDI_NOTE_CHANNEL);  // note channel
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

  if (clock_pulse_num % 6 == 0) {  // sixteenth note
    //send triggers to patterns
    for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
      for (int i = 0; i < 8; i++) {
        patterns[bank][i].incrementClock(clock_pulse_num);
      }
    }
    if (meap.irand(0, 4) == 0) {
      randomizeCutoff(rand_cutoff_range);
    }
  }

  if (clock_pulse_num % 3 == 0) {  // thirtysecond notex
  }

  clock_pulse_num = (clock_pulse_num + 1) % 24;
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
    // range = range << 1;  // maps input range to 0-200
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