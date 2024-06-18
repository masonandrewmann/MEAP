/*
  Press touch pads to toggle sequences on/off
  Pot #0 controls how often glitches occur (entropy)
  Pot #1 controls length of envelopes allowed in glitches (cw = long notes, ccw = short notes)

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Sample.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <EventDelay.h>
#include <ADSR.h>
#include "vox.h"

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#define NUM_SAMP 6

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

// variables for samples
Sample <vox_NUM_CELLS, AUDIO_RATE> samp[NUM_SAMP](vox_DATA);
float sampFreq[NUM_SAMP];
float voxFreq = 0;

EventDelay myTimer[NUM_SAMP];
int myTime[NUM_SAMP];

EventDelay envTimer[NUM_SAMP];
int noteLen = 50;
int myGain[NUM_SAMP];
int mute[NUM_SAMP];
int pitchOffset[NUM_SAMP];

int masterGains[NUM_SAMP];

EventDelay clockTimer;
int clockLen = 160;

int majScale[] = {0, 2, 4, 5, 7, 9, 11}; // template for building major scale on top of 12TET
int scaleRoot = 48; // root of major scale

int seventh[] = {0, 2, 4, 6}; // template for triad on top of major scale
int chordRoot = 0; // root of triad




//PATTERNS

class Sequence {       // The class
  public:             // Access specifier
    int notes[50];
    int times[50];
    int lengths[50];
    int index = 0;
    int length = 0;
    int beatCounter = 0;

    float currSampleMul = 0;
    int currMidiOffset = 0;
    int currNoteLength = 0;
    bool ready = false;

    void update(int offset){
      beatCounter++;
      if (beatCounter >= times[index]){
        beatCounter = 0;
        index++;
        if(index >= length){
          index = 0;
        }
        //set freq
        currMidiOffset = getMidiOfStep(index);
        currSampleMul = getSampleOffOfStepOffset(index, offset);
        currNoteLength = getLengthOfStep(index);
        ready = true;
      }
    }

  float scaleDegtoSemi(int scaleDeg)
  {
        int scaleOct = 0;
        while(scaleDeg < 0){ // bring up to octave
          scaleDeg += 7;
          scaleOct -= 1;
        }
        while(scaleDeg > 6){ // bring down to octave
          scaleDeg -= 7;
          scaleOct += 1;
        }
        float mySemi = majScale[scaleDeg] + 12 * scaleOct;
        return mySemi;
  }

  int getMidiOfStep(int stepNum){
    return scaleDegtoSemi(notes[stepNum]);
  }

  float getSampleOffOfStep(int stepNum){
    return pow(2, (float)getMidiOfStep(stepNum) / 12.0);
  }

  float getSampleOffOfStepOffset(int stepNum, int offset){
    return pow(2, (float)(scaleDegtoSemi(notes[stepNum] + offset)) / 12.0);
  }

  int getLengthOfStep(int stepNum){
    return lengths[stepNum] * clockLen;
  }
};

Sequence seq[NUM_SAMP];


int glitchFactor = 0;

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));

  //setup all the voice objects
  voxFreq = (float)vox_SAMPLERATE / (float) vox_NUM_CELLS;
  for (int i = 0; i < NUM_SAMP; i++){
    sampFreq[i] = (float)(voxFreq * pow(2, (float)(2*i)/12.0));
    samp[i].setFreq(sampFreq[i]);
    myTime[i] = 200 + 100 * i;
    myTimer[i].start(myTime[i]);
    myGain[i] = 0;
    masterGains[i] = 255;
    mute[i] = 0;
    pitchOffset[i] = 0;
  }

  clockTimer.start(clockLen);

  // create seq 0
  int m1N[] = {-5, -1, 0, 0, 2, -1}; // E3 B3 C4 C4 E4 B3 = -5 -1 0 0 2 -1
  int m1T[] = {12, 4, 22, 9, 1, 12}; // 0 12 16 38 47 48
  int m1L[] = {12, 4, 18, 9, 1, 12}; // 12 4 18 9 1 12
  int l1 = sizeof(m1N)/sizeof(int);
  seq[0].length = l1;
    for (int i = 0; i < l1; i++){
      seq[0].notes[i] = m1N[i];
      seq[0].times[i] = m1T[i];
      seq[0].lengths[i] = m1L[i];
    }
  //start seq 0
  myGain[0] = 1;
  envTimer[0].start(seq[0].getLengthOfStep(0));
  sampFreq[0] = voxFreq * seq[0].getSampleOffOfStep(0);
  samp[0].setFreq(sampFreq[0]*2);
  // masterGains[0] = 200;

  // create seq 1
  int m2N[] = {-3, -3, -3, -1, -3};
  int m2T[] = {4, 4, 4, 2, 2};
  int m2L[] = {2, 2, 2, 2, 2};
  int l2 = sizeof(m2N)/sizeof(int);
  seq[1].length = l2;
    for (int i = 0; i < l2; i++){
      seq[1].notes[i] = m2N[i];
      seq[1].times[i] = m2T[i];
      seq[1].lengths[i] = m2L[i];
    }
  //start seq 1
  myGain[1] = 1;
  envTimer[1].start(seq[1].getLengthOfStep(0));
  sampFreq[1] = voxFreq * seq[1].getSampleOffOfStep(0);
  samp[1].setFreq(sampFreq[1]);

  // create seq 2
  int m3N[] = {-5, -7, -5, -7, -5, -7, -5, -6, -7, -6, -7, -6, -7, -5};
  int m3T[] = {2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};
  int m3L[] = {2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};
  int l3 = sizeof(m3N)/sizeof(int);
  seq[2].length = l3;
    for (int i = 0; i < l3; i++){
      seq[2].notes[i] = m3N[i];
      seq[2].times[i] = m3T[i];
      seq[2].lengths[i] = m3L[i];
    }
  //start seq 2
  myGain[2] = 1;
  envTimer[2].start(seq[2].getLengthOfStep(0));
  sampFreq[2] = voxFreq * seq[2].getSampleOffOfStep(0);
  samp[2].setFreq(sampFreq[2]);

  // create seq 3
  int m4N[] = {11, 9, 7};
  int m4T[] = {1, 1, 1};
  int m4L[] = {1, 1, 1};
  int l4 = sizeof(m4N)/sizeof(int);
  seq[3].length = l4;
    for (int i = 0; i < l4; i++){
      seq[3].notes[i] = m4N[i];
      seq[3].times[i] = m4T[i];
      seq[3].lengths[i] = m4L[i];
    }
  //start seq 3
  myGain[3] = 1;
  envTimer[3].start(seq[3].getLengthOfStep(0));
  sampFreq[3] = voxFreq * seq[3].getSampleOffOfStep(0);
  samp[3].setFreq(sampFreq[3]);
  // masterGains[3] = 100;

  // create seq 4
  int m5N[] = {4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3};
  int m5T[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  int m5L[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int l5 = sizeof(m5N)/sizeof(int);
  seq[4].length = l5;
    for (int i = 0; i < l5; i++){
      seq[4].notes[i] = m5N[i];
      seq[4].times[i] = m5T[i];
      seq[4].lengths[i] = m5L[i];
    }
  //start seq 4
  myGain[4] = 1;
  envTimer[4].start(seq[4].getLengthOfStep(0));
  sampFreq[4] = voxFreq * seq[4].getSampleOffOfStep(0);
  samp[4].setFreq(sampFreq[4]);

  // create seq 5
  int m6N[] = {7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1};
  int m6T[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  int m6L[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int l6 = sizeof(m6N)/sizeof(int);
  seq[5].length = l6;
    for (int i = 0; i < l6; i++){
      seq[5].notes[i] = m6N[i];
      seq[5].times[i] = m6T[i];
      seq[5].lengths[i] = m6L[i];
    }
  //start seq 5
  myGain[5] = 1;
  envTimer[5].start(seq[5].getLengthOfStep(0));
  sampFreq[5] = voxFreq * seq[5].getSampleOffOfStep(0);
  samp[5].setFreq(sampFreq[5]);
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switch65es
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  // play sequence using new class
  if(clockTimer.ready()){
    clockTimer.start(clockLen);

    for (int i = 0; i < NUM_SAMP; i++){
      pitchOffset[i] = !dipVals[i] * 4;
    }
    //seq #1
    seq[0].update(pitchOffset[0]);
    if(seq[0].ready){
      seq[0].ready = false;
      sampFreq[0] = (float)(voxFreq * seq[0].currSampleMul);
      samp[0].setFreq(sampFreq[0]*2);
      samp[0].start();
      envTimer[0].start(seq[0].currNoteLength);
      myGain[0] = 1;
    }
    //seq #2
    seq[1].update(pitchOffset[1]);
    if(seq[1].ready){
      seq[1].ready = false;
      sampFreq[1] = (float)(voxFreq * seq[1].currSampleMul);
      samp[1].setFreq(sampFreq[1]);
      samp[1].start();
      envTimer[1].start(seq[1].currNoteLength);
      myGain[1] = 1;
    }

    //seq #3
    seq[2].update(pitchOffset[2]);
    if(seq[2].ready){
      seq[2].ready = false;
      sampFreq[2] = (float)(voxFreq * seq[2].currSampleMul);
      samp[2].setFreq(sampFreq[2]);
      samp[2].start();
      envTimer[2].start(seq[2].currNoteLength);
      myGain[2] = 1;
    }

    //seq #4
    seq[3].update(pitchOffset[3]);
    if(seq[3].ready){
      seq[3].ready = false;
      sampFreq[3] = (float)(voxFreq * seq[3].currSampleMul);
      samp[3].setFreq(sampFreq[3]);
      samp[3].start();
      envTimer[3].start(seq[3].currNoteLength);
      myGain[3] = 1;
    }

      //seq #5
    seq[4].update(pitchOffset[4]);
    if(seq[4].ready){
      seq[4].ready = false;
      sampFreq[4] = (float)(voxFreq * seq[4].currSampleMul);
      samp[4].setFreq(sampFreq[4]);
      samp[4].start(irand(0, 45000));
      envTimer[4].start(seq[4].currNoteLength);
      myGain[4] = 1;
    }

    //seq #6
    seq[5].update(pitchOffset[5]);
    if(seq[5].ready){
      seq[5].ready = false;
      sampFreq[5] = (float)(voxFreq * seq[5].currSampleMul);
      samp[5].setFreq(sampFreq[5]);
      samp[5].start(irand(0, 45000));
      envTimer[5].start(seq[5].currNoteLength);
      myGain[5] = 1;
    }

  //handle glitching
  glitchFactor = map(potVals[0], 0, 4095, 0, 100);
  int maxLen = map(potVals[1], 0, 4095, 10, 2000);

  for (int i = 0; i < NUM_SAMP; i++){
    int randNum = irand(0, 100);
    if(randNum < glitchFactor){
      //perform  glitch
      randNum = randNum%3;
      switch(randNum){
        case 0: //octave up
          samp[i].setFreq(sampFreq[i] * 2);
          break;

        case 1: //octave down
          samp[i].setFreq(sampFreq[i] * 4);
          break;

        case 2: //on/off
          envTimer[i].start(irand(0, maxLen));
          myGain[i] = 1;
          samp[i].start(irand(0, 50000));
          break;
      }
    }
  }
  }



  //handle envelopes
  for (int i = 0; i < NUM_SAMP; i++){
    if(envTimer[i].ready()){
      myGain[i] = 0;
    }
  }
}


int updateAudio(){
  int sample = 0;
  for (int i = 0; i < NUM_SAMP; i++){
    sample += (samp[i].next() * myGain[i] * masterGains[i] * mute[i]);
  }
  return MonoOutput::fromAlmostNBit(17, sample);
}

 
void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
//    int myVal = mux.read(i);
    mux.read(i); //read once and throw away result (for reliability)
    mux.read(i);
    dipVals[dipPins[i]] = mux.read(i);
  }

  //Check if any switches changed position
  for (int i = 0; i < 8; i++){
//    Serial.print(muxVals[i]); Serial.print(" "); // uncomment if you want to print the current state of all DIP switches
    switch(i){
      case 0:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 1 up");
          } else {
            Serial.println("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
          } else {
            Serial.println("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
          } else {
            Serial.println("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 4 up");
          } else {
            Serial.println("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 5 up");
          } else {
            Serial.println("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 6 up");
          } else {
            Serial.println("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 7 up");
          } else {
            Serial.println("DIP 7 down");
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 8 up");
          } else {
            Serial.println("DIP 8 down");
          }
        }
        break;   
    }
    prevDipVals[i] = dipVals[i]; // update prevDipVals array
  }
}

void readTouch(){
  int pinVal = 0;
  for (int i = 0; i < 8; i++){
    pinVal = touchRead(touchPins[i]);  
    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal; 
    if (touchAvgs[i] < touchThreshold){
      touchVals[i] = 1; 
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]){
      if(touchVals[i]){
        if(mute[i] == 1){
          mute[i] = 0;
        } else {
          mute[i] = 1;
        }
      }
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            Serial.println("pad 0 pressed");
          } else { // pad 0 released
            Serial.println("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
          } else { // pad 1 released
            Serial.println("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
          } else { // pad 2 released
            Serial.println("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
          } else { // pad 3 released
            Serial.println("pad 3 released");
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
          } else { // pad 4 released
            Serial.println("pad 4 released");
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
          } else { // pad 5 released
            Serial.println("pad 5 released");
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
          } else { // pad 6 released
            Serial.println("pad 6 released");
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
          } else { // pad 7 released
            Serial.println("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
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