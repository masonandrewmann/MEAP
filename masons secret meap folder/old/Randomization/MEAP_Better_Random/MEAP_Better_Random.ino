/*
  Basic demo of randomization. 

  Touch pads function as a basic C major keyboard.

  Each time a pad is pressed, the corresponding sine wave is played with first 8 harmonics 
  at random amplitudes.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <tables/sin8192_int8.h> // loads sine wavetable

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable


void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi();

  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function

  Serial.println(irand(11, 12));
  Serial.println(frand());
}


void loop(){
  audioHook();
}


void updateControl(){
}


int updateAudio(){
  return MonoOutput::from8Bit(0);
}

long irand(long howsmall, long howbig){ // generates a random integer between howsmall and howbig (inclusive of both numbers)
  howbig++;
  if (howsmall >= howbig){
    return howsmall;
  }
  long diff = howbig - howsmall;
  return (xorshift96() % diff) + howsmall;
}

float frand() // generates a random float between 0 and 1 with 4 decimals of precision
{
  return (xorshift96()%10000 / 10000.f);
}
