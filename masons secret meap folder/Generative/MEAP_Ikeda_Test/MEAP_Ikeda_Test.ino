/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Sample.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <Sample.h>
#include <EventDelay.h>
#include <Oscil.h>
#include <Ead.h>
#include "click.h"
#include <tables/sin8192_int8.h> // loads sine wavetable
#include <tables/whitenoise8192_int8.h> // loads white noise wavetable

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

using namespace admux;




// variables for DIP switches
Mux mux(Pin(34, INPUT, PinType::Digital), Pinset(16, 17, 12));
int dipPins[] = {5, 6, 7, 4, 3, 0, 2, 1};
int dipVals[] = {1, 1, 1, 1, 1, 1, 1, 1};
int prevDipVals[] = {1, 1, 1, 1, 1, 1, 1, 1};

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

// variables for potentiometers
int potVals[] = {0, 0};


Sample <click_NUM_CELLS, AUDIO_RATE> clickSamp(click_DATA);
int clickSeq[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> sine(SIN8192_DATA);
Ead sineEnv(CONTROL_RATE);
int sineGain = 0;
int sineSeq[] = {1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0};


Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> bass(SIN8192_DATA);
int bassSeq[] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0};
int bassGain = 0;

Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> noise(WHITENOISE8192_DATA);
Ead noiseEnv(CONTROL_RATE);
int noiseGain = 0;
int noiseSeq[] = {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0};


int seqIndex = 0;

EventDelay sampleDelay;
int sixteenth = 80;

float clickFreq;

uint16_t myRandom = 0; // variable for raw random number
int myMappedNum = 0; // variable for mapped random number

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  //sample setup
  clickFreq = (float)click_SAMPLERATE / (float) click_NUM_CELLS;
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
  
  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));
}


void loop(){
  audioHook();
}


void updateControl(){  
  if(sampleDelay.ready()){ //time for one sixteenth note has passed
    seqIndex = (seqIndex + 1)%16; // move to next step of sequence
    
    if(sineSeq[seqIndex] == 1){ // begin high sine envelope if current step is high
      sineEnv.start();
    }

    if(noiseSeq[seqIndex] == 1){ // begin noise envelope if current step is high
      noiseEnv.start();
    }

    if(clickSeq[seqIndex] == 1){// begin click sample if current step is high
      myRandom = xorshift96(); // generate random number
      myMappedNum = map(myRandom, 0, 65535, 20, 200); // map number to range 20-200
      float myFreqMul = myMappedNum / 100.f; // divide number to range 0.2 - 2.0
      clickSamp.setFreq(clickFreq * myFreqMul); // mulitply click frequency by multiplier
      clickSamp.start(); // play click

      bassGain = bassSeq[seqIndex]; // turn bass on if current step is high
    }
    
    sampleDelay.start(sixteenth); // set time for next step of sequencer
  }

  sineGain = sineEnv.next(); // read gain from sine envelope
  noiseGain = noiseEnv.next(); // read gain from bass envelope
}

int updateAudio(){
  return MonoOutput::fromAlmostNBit(16, 
  clickSamp.next() * 255 + 
  sine.next() * sineGain + 
  bass.next() * bassGain * 127 + 
  noise.next() * noiseGain / 2 );
}
