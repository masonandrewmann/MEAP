/*
  Example plays through sequence on loop. Sequence is 
  defined using variables mySeqNotes and mySeqLens

  Holding Touch #0 will transpose up by a major second
  Holding Touch #1 will play the sequence at double speed
  Holding Touch #2 will reverse the order of every pair of notes
  Holding Touch #3 will randomly mute about 1/3 of the notes
  Holding Touch #4 will play the pattern backwards
  Holding Touch #5 will play the sequence at half speed
  Holding Touch #6 will play every other note twice as fast
  Holding Touch #7 will randomly play notes twice as fast
  
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <tables/sin8192_int8.h>
#include <EventDelay.h>
#include <Ead.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

// variables for playing sequence
int mySeqNotes[] = {48, 52, 55, 59, 62, 65, 69, 72}; // notes referred to by MIDI note number
int mySeqLens[] = {250, 250, 250, 250, 250, 250, 250, 250}; // length of notes in milliseconds
int seqInd = 0;
int seqLen = 8;

int seqInc = 1;

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;


int gain;

int rootOffset = 0;
float lenMul = 1;

//oscillator definitions
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> mySine(SIN8192_DATA);
Ead myEnv(CONTROL_RATE); // resolution will be CONTROL_RATE


EventDelay noteDelay;

// variables for potentiometers
int potVals[] = {0, 0};

void setup(){
  startMozzi(CONTROL_RATE);
  pinMode(34, INPUT);
  mySine.setFreq(mtof(mySeqNotes[0]));
  noteDelay.start(mySeqLens[0]); // 2 second countdown
  myEnv.setAttack(10);
  myEnv.setDecay(500);

  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function
}


void loop(){
  audioHook();
}


void updateControl(){
    readPots(); // reads potentiometers
    readTouch(); // reads capacitive touch pads


  if(noteDelay.ready()){
    seqInd = (seqInd + seqInc); // move to next step of sequence
    if (seqInd > (seqLen - 1)){
      seqInd = 0;
    } else if (seqInd < 0){
      seqInd = seqLen - 1;
    }

    if(touchVals[7]){
      uint16_t myRandom = xorshift96(); 
      int myMappedNum = map(myRandom, 0, 65535, 0, 10);
      if (myMappedNum < 3){
        noteDelay.start(mySeqLens[seqInd] * lenMul / 2); // set ending time of current note
      } else{
        noteDelay.start(mySeqLens[seqInd] * lenMul); // set ending time of current note
      }
    } else {
      noteDelay.start(mySeqLens[seqInd] * lenMul); // set ending time of current note
    }
    if(touchVals[3]){ // if pad 3 is pressed, randomly skip notes
      uint16_t myRandom = xorshift96(); 
      int myMappedNum = map(myRandom, 0, 65535, 0, 10);
      if (myMappedNum < 4){
        
      } else{
        myEnv.start();
        mySine.setFreq(mtof(mySeqNotes[seqInd] + rootOffset)); // set oscillator frequency to current pitch

      }
    } else {
      myEnv.start();
      mySine.setFreq(mtof(mySeqNotes[seqInd] + rootOffset)); // set oscillator frequency to current pitch
    }
  }
  
  int atkTime = map(potVals[0], 0, 4095, 10, 1000);
  int decTime = map(potVals[1], 0, 4095, 10, 1000);
  myEnv.setAttack(atkTime);
  myEnv.setDecay(decTime);
  gain = myEnv.next();
}


AudioOutput_t updateAudio(){
  return MonoOutput::fromAlmostNBit(16,mySine.next()*gain);
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
            rootOffset = 5;
          } else { // pad 0 released
            Serial.println("pad 0 released");
            rootOffset = 0;
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
            lenMul = 0.5;
          } else { // pad 1 released
            Serial.println("pad 1 released");
            lenMul = 1;
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
            for( int i = 0; i < 8; i += 2){
              int temp = mySeqNotes[i];
              mySeqNotes[i] = mySeqNotes[i+1];
              mySeqNotes[i+1] = temp;
            }
          } else { // pad 2 released
            Serial.println("pad 2 released");
            for( int i = 0; i < 8; i += 2){
              int temp = mySeqNotes[i];
              mySeqNotes[i] = mySeqNotes[i+1];
              mySeqNotes[i+1] = temp;
            }
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
            seqInc = -1;
          } else { // pad 4 released
            Serial.println("pad 4 released");
            seqInc = 1;
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
            lenMul = 2;
          } else { // pad 5 released
            Serial.println("pad 5 released");
            lenMul = 1;
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
            for (int i = 0; i < 8; i += 2){
              mySeqLens[i] = 125;
            }

          } else { // pad 6 released
            Serial.println("pad 6 released");
            for (int i = 0; i < 8; i += 2){
              mySeqLens[i] = 250;
            }
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
