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
#include <EventDelay.h>
#include <ADSR.h>
#include "bell.h"
#include "bellroll.h"
#include "bowglo.h"
#include "clari.h"
#include "guit.h"
#include "juno.h"
#include "meltine.h"
#include "ocarina.h"
#include "junoPerc.h"

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#define NUM_SAMP_PADS 8
using namespace admux;

// variables for DIP switches
Mux mux(Pin(14, INPUT, PinType::Digital), Pinset(38, 45, 46));
int dipPins[] = {5, 6, 7, 4, 3, 0, 2, 1};
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {2, 4, 6, 8, 1, 3, 5, 7};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 500;

// variables for potentiometers
int potVals[] = {0, 0};

Sample <96359, AUDIO_RATE> sampPad[NUM_SAMP_PADS];
float sampPadFreq[NUM_SAMP_PADS];
float sampPadFreqDef;

ADSR <CONTROL_RATE, CONTROL_RATE> sampPadEnv[NUM_SAMP_PADS];
int sampPadGain[NUM_SAMP_PADS];

int majScale[] = {0, 2, 4, 5, 7, 9, 11, 12};

int entropy = 0;

int glitchOnOff[NUM_SAMP_PADS];
float glitchOct[NUM_SAMP_PADS];

int glitchLength = 35;
EventDelay glitchTimer;

void setup(){
  Serial.begin(115200); // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE); // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1); // sets up touch pads
  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function
  


  sampPadFreqDef = (float)bell_SAMPLERATE / (float) 96359;
  for(int i = 0; i < NUM_SAMP_PADS; i++){
    sampPad[i].setTable(bell_DATA);
    sampPad[i].setEnd(bell_NUM_CELLS);
    sampPadFreq[i] = sampPadFreqDef;
    sampPad[i].setFreq(sampPadFreq[i]);
    sampPad[i].setLoopingOn();
    sampPadGain[i] = 0;
    sampPadEnv[i].setAttackTime(3000);
    sampPadEnv[i].setSustainTime(30000);
    sampPadEnv[i].setReleaseTime(5000);
    sampPadEnv[i].setADLevels(255,255);
    glitchOnOff[i] = 1;
    glitchOct[i] = 1;
  }

  glitchTimer.start(glitchLength);
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switch65es
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  entropy = map(potVals[0], 0, 4095, 0, 100);

  int myRand = irand(0, 100);
  if(myRand < entropy){ //random octave glitching
    int ind = myRand % NUM_SAMP_PADS;
    if(sampPadEnv[ind].playing()){
      if(glitchOct[ind] == 1){
        glitchOct[ind] = 2;
      } else if (glitchOct[ind] == 2){
        glitchOct[ind] = 4;
      } else if (glitchOct[ind] == 4){
        glitchOct[ind] = 0.5;
      } else {
        glitchOct[ind] = 1;
      }
    
      sampPad[ind].setFreq(glitchOct[ind] * (float)(sampPadFreqDef * (float)pow(2, ((float)majScale[(int)ind])/12.0f)));
    }
  }

  for (int i = 0; i < NUM_SAMP_PADS ; i++){
    sampPadEnv[i].update();
  }

   for (int i = 0; i < NUM_SAMP_PADS; i++){
    sampPadGain[i] = sampPadEnv[i].next();
  }
}


AudioOutput_t updateAudio(){
  int sample = 0;

  for (int i = 0; i < NUM_SAMP_PADS; i++){
    sample += sampPad[i].next() * sampPadGain[i] * glitchOnOff[i];
  }
  return StereoOutput::fromAlmostNBit(19, sample, sample);
}

 

void readTouch(){
  int pinVal = 0;
  for (int i = 0; i < 8; i++){
    pinVal = touchRead(touchPins[i]);  
    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal; 
    if (touchAvgs[i] > touchThreshold){
      touchVals[i] = 1; 
    } else {
      touchVals[i] = 0;
    }
  
    if (touchVals[i] != prevTouchVals[i]){
      if(touchVals[i]){ // ANY PAD PRESSED
          glitchOnOff[i] = 1;
          glitchOct[i] = 1;
          sampPadEnv[i].noteOn();
          sampPad[i].start();
          sampPad[i].setFreq((float)(sampPadFreqDef * (float)pow(2, ((float)majScale[(int)i])/12.0f)));
      } else { // ANY PAD RELEASED
          sampPadEnv[i].noteOff();
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

void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
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
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(bell_DATA);
              sampPad[i].setEnd(bell_NUM_CELLS);
            }            
          } else {
            Serial.println("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(bowglo_DATA);
              sampPad[i].setEnd(bowglo_NUM_CELLS);
            }   
          } else {
            Serial.println("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(clari_DATA);
              sampPad[i].setEnd(clari_NUM_CELLS);
            }   
          } else {
            Serial.println("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(meltine_DATA);
              sampPad[i].setEnd(meltine_NUM_CELLS);
            }   
          } else {
            Serial.println("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 5 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(juno_DATA);
              sampPad[i].setEnd(juno_NUM_CELLS);
            }   
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

void readPots(){
  potVals[0] = 4095 - mozziAnalogRead(9);
  potVals[1] = 4095 - mozziAnalogRead(10);
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