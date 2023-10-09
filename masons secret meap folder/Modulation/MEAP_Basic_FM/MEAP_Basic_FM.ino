/*
  Basic FM Patch
  
  Plays a constant sine wave at 440Hz modulates its frequency using another sine wave.
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

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc1(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc2(SIN8192_DATA);

int modDepth = 0;

// variables for potentiometers
int potVals[] = {0, 0};

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);
  osc1.setFreq(440); //set frequency of sine oscillator
  osc2.setFreq(440); //set frequency of sine oscillator
}


void loop(){
  audioHook();
}


void updateControl(){
  readPots(); // reads potentiometers
  osc1.setFreq((float)map(potVals[0], 0, 4095, 1, 1000));
  modDepth = map(potVals[1], 0, 4095, 0, 127);
}


int updateAudio(){
  Q16n16 deviation = Q7n0_to_Q7n8(modDepth); // convert mod depth to 16n16 unsigned int
  Q15n16 modulation = deviation * osc1.next() >> 8; // calculate mod depth in 15n16 signed int
  return MonoOutput::from8Bit(osc2.phMod(modulation)); // apply phase mod to carrier oscillator
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}
