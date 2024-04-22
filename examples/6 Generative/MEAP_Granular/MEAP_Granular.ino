/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <Meap.h>                 // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/cos2048_int8.h>  // loads sine wavetable

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


#define NUM_OSC 80



Oscil<COS2048_NUM_CELLS, AUDIO_RATE> sines[NUM_OSC];

ADSR<AUDIO_RATE, AUDIO_RATE> envelope[NUM_OSC];

int grain_index = 0;

int gains[NUM_OSC];

EventDelay cloud_timer;
int cloud_length = 1000;
int cloud_or_silence = 0;

int density = 0;


int center_freq = 400;
int freq_min = 200;
int freq_max = 2000;

float freq_min_start = 500;
float freq_minEnd = 4000;

float freq_max_start = 600;
float freq_maxEnd = 4500;

float curr_min = 500;
float curr_max = 600;

float start_time = 0;
float end_time = 1000;
float curr_time = 0;

int grain_attack = 2;
int grain_decay = 50;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  for (byte i = 0; i < NUM_OSC; i++) {
    sines[i].setTable(COS2048_DATA);
    sines[i].setFreq((float)meap.irand(200, 2000));
    gains[i] = 0;
    envelope[i].setADLevels(255, 0);
    envelope[i].setTimes(10, 100, 0, 0);
  }

  cloud_timer.start(cloud_length);
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches
  //----TRIGGER SOME GRAINS----
  if (cloud_or_silence) {  // only trigger grains when in a cloud
    int dens_val = meap.irand(0, 1000);
    if (dens_val < density) {
      //calculate current frequency ranges by linearly interpolating
      curr_time = millis();
      curr_min = freq_min_start + (((curr_time - start_time) / (end_time - start_time)) * (freq_minEnd - freq_min_start));
      curr_max = freq_max_start + (((curr_time - start_time) / (end_time - start_time)) * (freq_minEnd - freq_max_start));

      // generate frequency
      sines[grain_index].setFreq((float)meap.irand(curr_min, curr_max));
      envelope[grain_index].noteOn();
      grain_index = (grain_index + 1) % NUM_OSC;
    }
  }

  //-----DETERMINE STATE------
  if (cloud_timer.ready()) {
    cloud_or_silence = !cloud_or_silence;
    if (cloud_or_silence) {  // initialize new cloud
      //-------GENERATE START AND END FREQS--------
      freq_min_start = meap.irand(50, 6000);
      freq_min_start = min((int)freq_min_start, (int)meap.irand(50, 6000));
      freq_minEnd = meap.irand(50, 6000);
      freq_minEnd = min((int)freq_minEnd, (int)meap.irand(50, 6000));

      freq_max_start = meap.irand(freq_min_start, freq_min_start * 5);
      freq_maxEnd = meap.irand(freq_minEnd, freq_minEnd * 3);

      cloud_length = meap.irand(500, 4000);
      cloud_timer.start(cloud_length);
      start_time = millis();
      end_time = start_time + cloud_length;

      //------GENERATE DENSITY-------
      density = meap.irand(50, 1000);

      //generate attack/dec
      grain_attack = meap.irand(0, 8) + 2;
      grain_decay = grain_attack;
      for (byte i = 0; i < NUM_OSC; i++) {
        envelope[i].setAttackTime(grain_attack);
        envelope[i].setDecayTime(grain_decay);
      }

    } else {  //prepare for silence
      cloud_length = meap.irand(2, 300);
      cloud_timer.start(cloud_length);
    }
  }
}


AudioOutput_t updateAudio() {
  int sample = 0;
  for (byte i = 0; i < NUM_OSC; i++) {
    envelope[i].update();
    gains[i] = envelope[i].next();
    sample += sines[i].next() * gains[i];
  }
  return StereoOutput::fromAlmostNBit(17, sample, sample);
}


/** User defined function called whenever a touch pad is pressed or released
  @param number is the number of the pad that was pressed or released: 0-7
  @param pressed is true if the pad was pressed and false if the pad was released
	*/
void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released
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

/** User defined function called whenever a DIP switch is toggled up or down
  @param number is the number of the switch that was toggled up or down: 0-7
  @param up is true if the switch was toggled up, and false if the switch was toggled down
	*/
void Meap::updateDip(int number, bool up) {
  if (up) {  // Any DIP up

  } else {  //Any DIP down
  }
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
