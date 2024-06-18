/*
  Basic AM Patch
  
  Plays a constant sine wave at 440Hz and modulates its amplitude using another sine wave.
  Pot 1 controls the modulation oscillator's frequency.
  Pot 2 controls modulation depth.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <tables/sin8192_int8.h> // loads sine wavetable
#include <tables/triangle_warm8192_int8.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

// variables for DIP switches
Mux mux(Pin(34, INPUT, PinType::Digital), Pinset(16, 17, 12));
int dipPins[] = {5, 6, 7, 4, 3, 0, 2, 1};
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> carrierOsc(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> modulatorOsc(TRIANGLE_WARM8192_DATA);

//Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> carrierOsc(SIN8192_DATA);
//Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> modulatorOsc(SIN8192_DATA);

int modDepth = 0;

// variables for potentiometers
int potVals[] = {0, 0};

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);
  carrierOsc.setFreq(440); //set frequency of sine oscillator
  modulatorOsc.setFreq(440); //set frequency of sine oscillator
}


void loop(){
  audioHook();
}


void updateControl(){
  readPots(); // reads potentiometers
  
  carrierOsc.setFreq((float)map(potVals[0], 0, 4095, 1, 1000));
  modDepth = map(potVals[1], 0, 4095, 0, 255);
}


int updateAudio(){
  int carrierVal = carrierOsc.next() * modDepth;
  int modulatorVal = modulatorOsc.next();
  return MonoOutput::fromAlmostNBit(24, carrierVal * modulatorVal);
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}
