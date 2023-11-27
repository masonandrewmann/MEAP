/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <EventDelay.h>
#include <tables/cos2048_int8.h> // loads sine wavetable
#include <ADSR.h>

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#define NUM_OSC 80

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

// variables for potentiometers
int potVals[] = {0, 0};

uint16_t myRandom = 0; // variable for raw random number
int myMappedNum = 0; // variable for mapped random number


Oscil<COS2048_NUM_CELLS, AUDIO_RATE> sines[NUM_OSC] = Oscil<COS2048_NUM_CELLS, AUDIO_RATE> (COS2048_DATA);

ADSR <AUDIO_RATE, AUDIO_RATE> envelope[NUM_OSC];

int grainIndex = 0;

int gains[NUM_OSC];

EventDelay cloudTimer;
int cloudLength = 1000;
int cloudOrSilence = 0;

int density = 0;


int centerFreq = 400;
int freqMin = 200;
int freqMax = 2000;

float freqMinStart = 500;
float freqMinEnd = 4000;

float freqMaxStart = 600;
float freqMaxEnd = 4500;

float currMin = 500;
float currMax = 600;

float startTime = 0;
float endTime = 1000;
float currTime = 0;

int grainAtk = 2;
int grainDec = 50;

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));

  for (byte i = 0; i < NUM_OSC; i++)
  {
    sines[i].setFreq((float)irand(200, 2000));
    gains[i] = 0;
    envelope[i].setADLevels(255,0);
    envelope[i].setTimes(10,100,0,0);
  }

  cloudTimer.start(cloudLength);
  
}


void loop(){
  audioHook();
}


void updateControl(){
  //----TRIGGER SOME GRAINS----
  if(cloudOrSilence){ // only trigger grains when in a cloud
    int densVal = irand(0, 1000);
    if (densVal < density){
      //calculate current frequency ranges by linearly interpolating
      currTime = millis();
      currMin = freqMinStart + ( ( (currTime - startTime)/(endTime-startTime) ) * (freqMinEnd - freqMinStart)); 
      currMax = freqMaxStart + ( ( (currTime - startTime)/(endTime-startTime) ) * (freqMinEnd - freqMaxStart));
   
      // generate frequency
      sines[grainIndex].setFreq((float)irand(currMin, currMax));
      envelope[grainIndex].noteOn();
      grainIndex = (grainIndex + 1) % NUM_OSC;
    }
  }

  //-----DETERMINE STATE------
  if(cloudTimer.ready()){
    cloudOrSilence = !cloudOrSilence;
    if(cloudOrSilence){ // initialize new cloud
      //-------GENERATE START AND END FREQS--------
      freqMinStart = irand(50, 6000);
      freqMinStart = min((int)freqMinStart, (int)irand(50, 6000));
      freqMinEnd = irand(50, 6000);
      freqMinEnd = min((int)freqMinEnd, (int)irand(50, 6000));

      freqMaxStart = irand(freqMinStart, freqMinStart * 5);
      freqMaxEnd = irand(freqMinEnd, freqMinEnd * 3);

      cloudLength = irand(500, 4000);
      cloudTimer.start(cloudLength);
      startTime = millis();
      endTime = startTime + cloudLength;

      //------GENERATE DENSITY-------
      density = irand(50, 1000);

      //generate attack/dec
      grainAtk = irand(0, 8) + 2;
      grainDec = grainAtk;
      for (byte i = 0; i < NUM_OSC; i++)
      {
        envelope[i].setAttackTime(grainAtk);
        envelope[i].setDecayTime(grainDec);
      }
      
    } else { //prepare for silence
      cloudLength = irand(2, 300);
      cloudTimer.start(cloudLength);
    }
  }
}


int updateAudio(){
  int sample = 0;
  for (byte i = 0; i < NUM_OSC; i++)
    {
      envelope[i].update();
      gains[i] = envelope[i].next();
      sample += sines[i].next() * gains[i];
    }
  return MonoOutput::fromAlmostNBit(17, sample);
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