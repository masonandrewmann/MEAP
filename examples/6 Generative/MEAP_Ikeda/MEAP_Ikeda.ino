/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <Meap.h>  // MEAP library, includes all dependent libraries, including all Mozzi modules
#include "guit.h"
#include "click.h"
#include <tables/sin8192_int8.h>         // loads sine wavetable
#include <tables/whitenoise8192_int8.h>  // loads white noise wavetable

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


Sample<guit_NUM_CELLS, AUDIO_RATE> guitSamp(guit_DATA);


Sample<click_NUM_CELLS, AUDIO_RATE> clickSamp(click_DATA);
int clickSeq[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int clickDens[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> sine(SIN8192_DATA);
Ead sineEnv(CONTROL_RATE);
int sineGain = 0;
int sineSeq[] = { 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0 };
int sineDens[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> bass(SIN8192_DATA);
int bassSeq[] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0 };
int bassDens[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int bassGain = 0;

Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> noise(WHITENOISE8192_DATA);
Ead noiseEnv(CONTROL_RATE);
int noiseGain = 0;
int noiseSeq[] = { 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 };
int noiseDens[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


int seqIndex = 0;

EventDelay sampleDelay;
int sixteenth = 80;

float clickFreq;

uint16_t myRandom = 0;  // variable for raw random number
int myMappedNum = 0;    // variable for mapped random number

int state = 0;  // 0=theme    1=increasing density    2=decreasing density
int density = 5;

int startRepeats = 0;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  //sample setup
  clickFreq = (float)click_SAMPLERATE / (float)click_NUM_CELLS;
  clickSamp.setFreq(clickFreq);

  //high sine
  sine.setFreq(7000);
  sineEnv.set(10, 20);

  //bass sine
  bass.setFreq(70);

  //white noise
  noise.setFreq(10000);
  noiseEnv.set(5, 5);

  sampleDelay.start(sixteenth);
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  if (sampleDelay.ready()) {
    seqIndex = (seqIndex + 1) % 16;  // move to next step of sequence and cycle back to beginning whenn we hit step 16

    // --------WHAT STATE DO WE MOVE TO?--------
    if (seqIndex == 0) {          // we finished one cycle, check what state we should be in
      if (state == 0) {           // ----IN THEME STATE----
        if (startRepeats >= 3) {  // if we repeated starting state 4 times, move on to crescendo
          state = 1;
          fillDens(density);  //fill up density arrays
          startRepeats = 0;
        } else {
          startRepeats++;  // if we haven't repeated 4 times; repeat and increment counter
        }
      } else if (state == 1) {  // ----IN CRESCENDO STATE----
        if (density > 60) {     // if density is above threshold, move on to decrescendo
          state = 2;
        } else {  // otherwise, increase density and repeat
          myRandom = xorshift96();
          density += map(myRandom, 0, 65535, 2, 10);
          fillDens(density);  // fill up pattern arrays
        }
      } else if (state == 2) {  // ----IN DECRESCENDO STATE----
        if (density < 5) {      // if density is above threshold, move to theme state
          state = 0;
          bass.setFreq(70);  // reset bass note freq
        } else {             // otherwise, decrease density and repeat
          myRandom = xorshift96();
          density -= map(myRandom, 0, 65535, 2, 10);
          fillDens(density);  // fill up pattern arrays
        }
      }
    }


    // ----------------WHAT DO WE DO IN EACH STATE---------------
    // the only difference is that we are reading from different arrays

    if (state == 0) {                // ----IN THEME STATE READ FROM ...Seq[] arrays----
      if (sineSeq[seqIndex] == 1) {  // begin high sine envelope if current step is high
        sineEnv.start();
      }

      if (noiseSeq[seqIndex] == 1) {  // begin noise envelope if current step is high
        noiseEnv.start();
      }

      if (clickSeq[seqIndex] == 1) {                     // begin click sample if current step is high
        myRandom = xorshift96();                         // generate random number
        myMappedNum = map(myRandom, 0, 65535, 20, 200);  // map number to range 20-200
        float myFreqMul = myMappedNum / 100.f;           // divide number to range 0.2 - 2.0
        clickSamp.setFreq(clickFreq * myFreqMul);        // mulitply click frequency by multiplier
        clickSamp.start();                               // play click
      }

      bassGain = bassSeq[seqIndex];         //turn bass on/off based on sequence
    } else if (state == 1 || state == 2) {  // ----IN CRES/DECRES STATES READ FROM ...Dens[] arrays----
      if (sineDens[seqIndex] == 1) {        // begin high sine envelope if current step is high
        sineEnv.start();
      }

      if (noiseDens[seqIndex] == 1) {  // begin noise envelope if current step is high
        noiseEnv.start();
      }

      if (clickDens[seqIndex] == 1) {                    // begin click sample if current step is high
        myRandom = xorshift96();                         // generate random number
        myMappedNum = map(myRandom, 0, 65535, 20, 200);  // map number to range 20-200
        float myFreqMul = myMappedNum / 100.f;           // divide number to range 0.2 - 2.0
        clickSamp.setFreq(clickFreq * myFreqMul);        // mulitply click frequency by multiplier
        clickSamp.start();                               // play click
      }

      bassGain = bassDens[seqIndex];  // turn bass on if current step is high
    }

    sampleDelay.start(sixteenth);  // set time for next step of sequencer
  }

  // Read gains from envelopes
  sineGain = sineEnv.next();
  noiseGain = noiseEnv.next();
}

 AudioOutput_t updateAudio() {
  int sample = clickSamp.next() * 255 + sine.next() * sineGain + bass.next() * bassGain * 127 + noise.next() * noiseGain / 2;
  return StereoOutput::fromAlmostNBit(16, sample, sample);
}

void fillDens(int densVal) {
  for (int i = 0; i < 16; i++) {
    myRandom = xorshift96();
    if (map(myRandom, 0, 65535, 0, 100) < densVal) {
      clickDens[i] = 1;
    } else {
      clickDens[i] = 0;
    }

    myRandom = xorshift96();
    if (map(myRandom, 0, 65535, 0, 100) < densVal) {
      sineDens[i] = 1;
    } else {
      sineDens[i] = 0;
    }

    myRandom = xorshift96();
    if (map(myRandom, 0, 65535, 0, 100) < (densVal / 3)) {
      bassDens[i] = 1;
    } else {
      bassDens[i] = 0;
    }
    myRandom = xorshift96();
    if (map(myRandom, 0, 65535, 0, 100) < 70) {
      bass.setFreq(70);
    } else {
      bass.setFreq(35);
    }

    myRandom = xorshift96();
    if (map(myRandom, 0, 65535, 0, 100) < (densVal / 2)) {
      noiseDens[i] = 1;
    } else {
      noiseDens[i] = 0;
    }
  }
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
