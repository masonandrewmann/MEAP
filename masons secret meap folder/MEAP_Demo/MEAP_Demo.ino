/*
  Example for basic monophonic sine wave synthesizer played using
  capacitive touch pads. Plays a C major scale starting on C4.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <ADSR.h>
#include <Oscil.h>
#include "kick.h"
#include "snare.h"
#include "hat.h"
#include "harp_c3.h"
#include "cmaj7.h"
#include <tables/cos8192_int8.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

#define SAMPLE_FREQ 16384

using namespace admux;

// variables for DIP switches
Mux mux(Pin(5, INPUT, PinType::Digital), Pinset(16, 17, 18));
int dipPins[] = {6, 5, 7, 4, 3, 0, 1, 2};
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

int notes[] = {72, 79, 81, 84, 72};




// variables for potentiometers
int potVals[] = {0, 0};

Oscil <COS8192_NUM_CELLS, AUDIO_RATE> sine1(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> sine2(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> sine3(COS8192_DATA);

Oscil <COS8192_NUM_CELLS, AUDIO_RATE> mel1(COS8192_DATA);

ADSR <CONTROL_RATE, CONTROL_RATE> env1;
ADSR <CONTROL_RATE, CONTROL_RATE> env2;
ADSR <CONTROL_RATE, CONTROL_RATE> env3;

ADSR <CONTROL_RATE, CONTROL_RATE> melEnv;

// variables for sample
Sample <kick_NUM_CELLS, AUDIO_RATE> kick(kick_DATA);
Sample <snare_NUM_CELLS, AUDIO_RATE> snare(snare_DATA);
Sample <hat_NUM_CELLS, AUDIO_RATE> hat(hat_DATA);
float kickFreq;
float snareFreq;
float hatFreq;

int gain1 = 1;
int gain2 = 1;
int gain3 = 1;

int melGain = 1;

int kickTrigs[] = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
int snareTrigs[] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
int hatTrigs[] = {0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0};
int seqInd = -1;

int seqTrigs[] = {72, 0, 0, 0, 81, 0, 79, 0, 0, 0, 72, 0, 0, 0, 0, 0};

//int cmaj7
Sample <harp_c3_NUM_CELLS, AUDIO_RATE> aSample(harp_c3_DATA);
float aSampleFreq;

//int cmaj7
Sample <myChord_NUM_CELLS, AUDIO_RATE> chord1(myChord_DATA);
float chord1Freq;
Sample <myChord_NUM_CELLS, AUDIO_RATE> chord2(myChord_DATA);
float chord2Freq;


float tempo = 100;

unsigned long nextTime = 0;

float stepLen = 0;

int cmaj7[] = {60, 71, 76};
int fmaj7[] = {65, 69, 76};



void setup(){
  startMozzi();
  Serial.begin(115200);
  
  kickFreq =  (float)kick_SAMPLERATE / (float) kick_NUM_CELLS;
  kick.setFreq(kickFreq); // play at the speed it was recorded

  snareFreq =  (float)snare_SAMPLERATE / (float) snare_NUM_CELLS;
  snare.setFreq(snareFreq); // play at the speed it was recorded

  hatFreq =  (float)hat_SAMPLERATE / (float) hat_NUM_CELLS;
  hat.setFreq(kickFreq); // play at the speed it was recorded

  stepLen = 60000.0/tempo;
  stepLen = stepLen / 4.0;
//  stepLen = 250;

  nextTime += stepLen;

  sine1.setFreq(mtof(60));
  sine2.setFreq(mtof(64));
  sine3.setFreq(mtof(67));

  mel1.setFreq(mtof(72));

  env1.setADLevels(255,230);
  env2.setADLevels(255,230);
  env3.setADLevels(255,230);

  melEnv.setADLevels(255,230);

  env1.setTimes(200,100,400,400);
  env2.setTimes(200,100,400,400);
  env3.setTimes(200,100,400,400);

  melEnv.setTimes(10,20,50,200);

  aSampleFreq =  (float)harp_c3_SAMPLERATE / (float) harp_c3_NUM_CELLS;
  aSample.setFreq(aSampleFreq); // play at the speed it was recorded
  aSample.rangeWholeSample();

  chord1Freq =  (float)myChord_SAMPLERATE / (float) myChord_NUM_CELLS * 4;
  chord1.setFreq(chord1Freq); // play at the speed it was recorded

  chord2Freq =  (float)myChord_SAMPLERATE / (float) myChord_NUM_CELLS * 4 * pow(2, 5.0/12.0);
  chord2.setFreq(chord2Freq); // play at the speed it was recorded
  
}


void loop(){
  audioHook();
}


void updateControl(){
  readTouch();
  readPots();

  if (millis() > nextTime){
    seqInd = (seqInd + 1) % 8;

    int chordRand = rand(100);
    if(chordRand > map(potVals[0], 0, 4095, 110, 10)) {
      int c1Mul = 1;
      if(chordRand % 2 == 1){
        c1Mul = 2;
      }
      chord1.setFreq(chord1Freq * c1Mul); // play at the speed it was recorded
      chord1.start();
      int len1 = rand(myChord_NUM_CELLS);
      len1 = min(len1, rand(myChord_NUM_CELLS));
      len1 = min(len1, rand(myChord_NUM_CELLS));
      len1 = min(len1, rand(myChord_NUM_CELLS));
      chord1.setEnd(len1);
    }

    chordRand = rand(100);
    if(chordRand > map(potVals[1], 0, 4095, 110, 10)) {
      int c2Mul = 1;
      if(chordRand % 2 == 1){
        c2Mul = 2;
      }
      chord2.setFreq(chord2Freq * c2Mul); // play at the speed it was recorded
      chord2.start();
      int len2 = rand(myChord_NUM_CELLS);
      len2 = min(len2, rand(myChord_NUM_CELLS));
      len2 = min(len2, rand(myChord_NUM_CELLS));
      len2 = min(len2, rand(myChord_NUM_CELLS));
      chord2.setEnd(len2);
    }

    if (seqInd == 0){
      int randVal;

      randVal = rand(100);

      //random melody
      for (int i = 0; i < 16; i++){
        seqTrigs[i] = 0;
      }
      int numNotes = rand(6);
      for (int i = 0; i < numNotes; i++){
        int rand1 = rand(16);
        int rand2 = rand(16);
        rand1 = min(rand1, rand2);
        int oct = rand(4);
        if (oct==1){
          oct = 12;
        } else if(oct == 2) {
          oct = -12;
        } else if (oct == 3){
          oct = 24;
        } else {
          oct = 0;
        }
        seqTrigs[rand1] = notes[i] + oct;
      }

      if(randVal > 30){
        int r = rand(1000)-rand(1000);
        unsigned int new_value = abs(r);

        env1.setTimes(new_value, new_value, new_value, new_value);
        env2.setTimes(new_value, new_value, new_value, new_value);
        env3.setTimes(new_value, new_value, new_value, new_value);
        
        if(randVal%2 == 0){
          sine1.setFreq(mtof(cmaj7[0] - rand(2) * 12));
          sine2.setFreq(mtof(cmaj7[1]- rand(2) * 12));
          sine3.setFreq(mtof(cmaj7[2]- rand(2) * 12));
        } else{
          sine1.setFreq(mtof(fmaj7[0]- rand(2) * 12));
          sine2.setFreq(mtof(fmaj7[1]- rand(2) * 12));
          sine3.setFreq(mtof(fmaj7[2]- rand(2) * 12));
        }
        env1.noteOn();
        env2.noteOn();
        env3.noteOn();

        
      }

      

      for (int i = 0; i < 16; i++){
        randVal = rand(100);
        if(randVal > 50){
          hatTrigs[i] = 1;
        } else {
          hatTrigs[i] = 0;
        }
      }

      for (int i = 0; i < 8; i += 2){
        randVal = rand(100);
        if(randVal > 60){
          snareTrigs[i] = 1;
        } else {
          snareTrigs[i] = 0;
        }
      }

      randVal = rand(100);
      if (randVal > 75){
        kickTrigs[14] = 1;
      } else {
        kickTrigs[14] = 0;
      }
      
    }
    
    if(kickTrigs[seqInd] == 1){
      kick.start();
    }
    
    if(snareTrigs[seqInd] == 1){
      snare.setFreq(snareFreq + rand(200)); // play at the speed it was recorded
      snare.start();
    }

    if(hatTrigs[seqInd] == 1){
      hat.setFreq(kickFreq + rand(200)); // play at the speed it was recorded
      hat.start();
    }

    if(seqTrigs[seqInd] != 0){
      mel1.setFreq(mtof(seqTrigs[seqInd]));
      melEnv.noteOn();
    }

    nextTime += stepLen;
  }

  env1.update();
  gain1 = env1.next();

  env2.update();
  gain2 = env2.next();

  env3.update();
  gain3 = env3.next();

  melEnv.update();
  melGain = melEnv.next();
}


AudioOutput_t updateAudio(){
  int s1 = (gain1 * sine1.next())>>4;
  int s2 = (gain2 * sine2.next())>>4;
  int s3 = (gain3 * sine3.next())>>4;
  int m1 = (melGain * mel1.next())>>3;
  return MonoOutput::fromAlmostNBit(16, kick.next()*256 + snare.next()*256 + hat.next()*256 + s1 + s2 + s3 + m1 + aSample.next()*64 + chord1.next() * 100 + chord2.next() * 100);
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
            aSample.setFreq((float)aSampleFreq);
            aSample.start();
          } else { // pad 0 released

          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            aSample.setFreq((float)(aSampleFreq * pow(2, 2.0/12.0)));
            aSample.start();
          } else { // pad 1 released

          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            aSample.setFreq((float)(aSampleFreq * pow(2, 4.0/12.0)));
            aSample.start();
          } else { // pad 2 released

          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            aSample.setFreq((float)(aSampleFreq * pow(5, 2.0/12.0)));
            aSample.start();
          } else { // pad 3 released

          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            aSample.setFreq((float)(aSampleFreq * pow(2, 7.0/12.0)));
            aSample.start();
          } else { // pad 4 released

          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            aSample.setFreq((float)(aSampleFreq * pow(2, 9.0/12.0)));
            aSample.start();
          } else { // pad 5 released

          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
//            chord1.start();
            aSample.setFreq((float)(aSampleFreq * pow(2, 11.0/12.0)));
            aSample.start();
          } else { // pad 6 released

          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
//            chord2.start();
            aSample.setFreq((float)(aSampleFreq * pow(2, 12.0/12.0)));
            aSample.start();
          } else { // pad 7 released

          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(36);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(39);
}
