/*
  Demonstrates manipulation of samples

  Pot #1 controls entropy
  Pot #2 control tempo

  Touch pads control chord root on a major scale908

  DIP 1-3 toggle between vocal and bell samples

  Toggling DIP 8 up drops everthing down an octave
  
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <Sample.h>
#include <EventDelay.h>
#include "vocal_samp.h"
#include "bellroll.h"

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

// variables for potentiometers
int potVals[] = {0, 0};

// variables for sample
Sample <(long)vocal_samp_NUM_CELLS, AUDIO_RATE> mySample(vocal_samp_DATA);
Sample <(long)vocal_samp_NUM_CELLS, AUDIO_RATE> sample2(vocal_samp_DATA);
Sample <(long)vocal_samp_NUM_CELLS, AUDIO_RATE> sample3(vocal_samp_DATA);

//Sample <(long)bellroll_NUM_CELLS, AUDIO_RATE> mySample(bellroll_DATA);
//Sample <(long)bellroll_NUM_CELLS, AUDIO_RATE> sample2(bellroll_DATA);
//Sample <(long)bellroll_NUM_CELLS, AUDIO_RATE> sample3(bellroll_DATA);

float mySampleFreq;
float sample2Freq;
float sample3Freq;

int noteNum = 0;
int note2Num = 0;
int note3Num = 0;

int sampStart = 0;
int sampLen = 2000;

int samp2Start = 0;
int samp2Len = 2000;

int samp3Start = 0;
int samp3Len = 2000;

EventDelay noteDelay;
int noteLength = 400;

EventDelay noteTwo;
int note2Length = 800;

EventDelay noteThree;
int note3Length = 1600;

uint16_t myRandom = 0; // variable for raw random number
int myMappedNum = 0; // variable for mapped random number

int entropy = 100;

int majScale[] = {0, 2, 4, 5, 7, 9, 11, 12};
int triad[] = {0, 2, 4};
int seventh[] = {0, 2, 4, 6};
int chordRoot = 0;

float octave = 1;

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi();

  mySampleFreq = (float)vocal_samp_SAMPLERATE / (float) (long)vocal_samp_NUM_CELLS;
//  mySampleFreq = (float)bellroll_SAMPLERATE / (float) (long)bellroll_NUM_CELLS;
  
  mySample.setFreq(mySampleFreq);
  sample2.setFreq(mySampleFreq);
  sample3.setFreq(mySampleFreq);
  

  noteDelay.start(noteLength); //starts first note timer
  noteTwo.start(note2Length);
  noteThree.start(note2Length);

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switches
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  entropy = map(potVals[0], 0, 4095, 0, 100);

  noteLength = map(potVals[1], 0, 4095, 500, 50);
  note2Length = noteLength * 2;
  note3Length = noteLength * 4;


  if (noteDelay.ready()){

  // ------- RANDOMIZING SAMPLE PLAYBACK --------
    myRandom = xorshift96();
    myMappedNum = map(myRandom, 0, 65535, 0, 100);
    if( myMappedNum < entropy){
      //generate random pitch
      myRandom = xorshift96();
//      myMappedNum = map(myRandom, 0, 65535, 50, 200);
//      float myFreqMul = myMappedNum / 100.0;
      noteNum = map(myRandom, 0, 65535, 0, 4);
      
      //generate random start point
      myRandom = xorshift96();
      sampStart = map(myRandom, 0, 65535, 0, 20000);
  
      //generate random length
      myRandom = xorshift96();
      int rand1 = map(myRandom, 0, 65535, 500, 4000);

      myRandom = xorshift96();
      int rand2 = map(myRandom, 0, 65535, 500, 4000);

      sampLen = max(rand1, rand2);
    }

    mySample.setFreq(octave * mySampleFreq * (float)pow(2, (float)majScale[(chordRoot + seventh[noteNum])%7]/12.0));
    mySample.setStart(sampStart);
    mySample.setEnd(sampStart + sampLen);
    mySample.start();

    //-------- RANDOMIZING NOTE LENGTH -----------

    myRandom = xorshift96();
    myMappedNum = map(myRandom, 0, 65535, 0, 100);
    if( myMappedNum < entropy){
      myRandom = xorshift96();
      myMappedNum = map(myRandom, 0, 65535, 0, 4);
      switch(myMappedNum){
        case 0:
          noteDelay.start(noteLength / 2);
        case 1: 
          noteDelay.start(noteLength / 2);
        case 2:
          noteDelay.start(noteLength);
        case 3:
          noteDelay.start(noteLength);  
        default:
          noteDelay.start(noteLength);      
      }
    } else {
      noteDelay.start(noteLength);
    }
//    noteDelay.start(noteLength);
  }

  if (noteTwo.ready()){
  // ------- RANDOMIZING SAMPLE PLAYBACK --------
    myRandom = xorshift96();
    myMappedNum = map(myRandom, 0, 65535, 0, 100);
    if( myMappedNum < entropy){
      
      //generate random pitch
      myRandom = xorshift96();
      note2Num = map(myRandom, 0, 65535, 0, 4);
      
      //generate random start point
      myRandom = xorshift96();
      samp2Start = map(myRandom, 0, 65535, 0, 20000);
  
      //generate random length
      myRandom = xorshift96();
      int rand1 = map(myRandom, 0, 65535, 1000, 6000);

      myRandom = xorshift96();
      int rand2 = map(myRandom, 0, 65535, 1000, 6000);

      samp2Len = max(rand1, rand2) * 2;
    }

    sample2.setFreq(octave * 0.5f * mySampleFreq * (float)pow(2, (float)majScale[(chordRoot + seventh[note2Num])%7]/12.0));
    sample2.setStart(sampStart);
    sample2.setEnd(sampStart + sampLen);
    sample2.start();
    //-------- RANDOMIZING NOTE LENGTH -----------

    myRandom = xorshift96();
    myMappedNum = map(myRandom, 0, 65535, 0, 100);
    if( myMappedNum < entropy){
      myRandom = xorshift96();
      myMappedNum = map(myRandom, 0, 65535, 0, 4);
      switch(myMappedNum){
        case 0:
          noteTwo.start(note2Length / 2);
        case 1: 
          noteTwo.start(note2Length / 2);
        case 2:
          noteTwo.start(note2Length);
        case 3:
          noteTwo.start(note2Length);   
        default:
          noteTwo.start(note2Length);   
      }
    } else {
      noteTwo.start(noteLength);
    }
//    noteTwo.start(note2Length);
  }


  if (noteThree.ready()){
  // ------- RANDOMIZING SAMPLE PLAYBACK --------
    myRandom = xorshift96();
    myMappedNum = map(myRandom, 0, 65535, 0, 100);
    if( myMappedNum < entropy){
      
      //generate random pitch
      myRandom = xorshift96();
      note3Num = map(myRandom, 0, 65535, 0, 4);
      
      //generate random start point
      myRandom = xorshift96();
      samp3Start = map(myRandom, 0, 65535, 0, 5000);
  
      //generate random length
      myRandom = xorshift96();
      int rand1 = map(myRandom, 0, 65535, 1000, 12000);

      myRandom = xorshift96();
      int rand2 = map(myRandom, 0, 65535, 1000, 12000);

      samp3Len = max(rand1, rand2) * 2;
    }

    sample3.setFreq(octave * 0.25f * mySampleFreq * (float)pow(2, (float)majScale[(chordRoot + seventh[note3Num])%7]/12.0));
    sample3.setStart(sampStart);
    sample3.setEnd(sampStart + sampLen);
    sample3.start();

    noteThree.start(note3Length);
  }
}


int updateAudio(){
  return MonoOutput::fromAlmostNBit(9, mySample.next() + sample2.next() + sample3.next());
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
            sample3.setTable(bellroll_DATA);
          } else {
            Serial.println("DIP 1 down");
            sample3.setTable(vocal_samp_DATA);
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
            sample2.setTable(bellroll_DATA);
          } else {
            Serial.println("DIP 2 down");
            sample2.setTable(vocal_samp_DATA);
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
            mySample.setTable(bellroll_DATA);
          } else {
            Serial.println("DIP 3 down");
            mySample.setTable(vocal_samp_DATA);
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
            octave = 0.5;
          } else {
            Serial.println("DIP 8 down");
            octave = 1;
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
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            Serial.println("pad 0 pressed");
            chordRoot = 0;
            
          } else { // pad 0 released
            Serial.println("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
            chordRoot = 1;
          } else { // pad 1 released
            Serial.println("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
            chordRoot = 2;
          } else { // pad 2 released
            Serial.println("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
            chordRoot = 3;
          } else { // pad 3 released
            Serial.println("pad 3 released");
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
            chordRoot = 4;
          } else { // pad 4 released
            Serial.println("pad 4 released");
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
            chordRoot = 5;
          } else { // pad 5 released
            Serial.println("pad 5 released");
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
            chordRoot = 6;
          } else { // pad 6 released
            Serial.println("pad 6 released");
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
            chordRoot = 7;
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
