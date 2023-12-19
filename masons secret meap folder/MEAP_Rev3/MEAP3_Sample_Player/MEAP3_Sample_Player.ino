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

// variables for samples
//Sample <bell_NUM_CELLS, AUDIO_RATE> bellSamp(bell_DATA);
//Sample <bellroll_NUM_CELLS, AUDIO_RATE> bellrollSamp(bellroll_DATA);
//Sample <bowglo_NUM_CELLS, AUDIO_RATE> bowgloSamp(bowglo_DATA);
//Sample <clari_NUM_CELLS, AUDIO_RATE> clariSamp(clari_DATA);
//Sample <guit_NUM_CELLS, AUDIO_RATE> guitSamp(guit_DATA);
//Sample <juno_NUM_CELLS, AUDIO_RATE> junoSamp(juno_DATA);
//Sample <meltine_NUM_CELLS, AUDIO_RATE> meltineSamp(meltine_DATA);
//Sample <ocarina_NUM_CELLS, AUDIO_RATE> ocarinaSamp(ocarina_DATA);
Sample <96359, AUDIO_RATE> bellSamp(bell_DATA);
Sample <96359, AUDIO_RATE> bellrollSamp(bellroll_DATA);
Sample <96359, AUDIO_RATE> bowgloSamp(bowglo_DATA);
Sample <96359, AUDIO_RATE> clariSamp(clari_DATA);
//Sample <96359, AUDIO_RATE> guitSamp(guit_DATA);
Sample <96359, AUDIO_RATE> guitSamp(junoPerc_DATA);
Sample <96359, AUDIO_RATE> junoSamp(juno_DATA);
Sample <96359, AUDIO_RATE> meltineSamp(meltine_DATA);
Sample <96359, AUDIO_RATE> ocarinaSamp(ocarina_DATA);


int dipPressed = 0; // MAY NEED TO ADJUST THIS TO ACCOUNT FOR STARTING STATES OF DIP SWITCHES



float padPressNum = -1;

int majScale[] = {0, 2, 4, 5, 7, 9, 11, 12};

int rootOffset = 0;

float bellFreq, bellrollFreq, bowgloFreq, clariFreq, guitFreq, junoFreq, meltineFreq, ocarinaFreq;

int gain[] = {0, 0, 0, 0, 0, 0, 0, 0};

int entropy = 0;

int glitchOnOff[] = {1, 1, 1, 1, 1, 1, 1, 1};
float glitchOct[] = {1, 1, 1, 1, 1, 1, 1, 1};

int glitchLength = 25;
EventDelay glitchTimer;

uint16_t myRandom = 0; // variable for raw random number
int myMappedNum = 0; // variable for mapped random number

ADSR <CONTROL_RATE, CONTROL_RATE> envelopes[8];

void setup(){
  Serial.begin(115200); // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE); // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1); // sets up touch pads
  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function
  

  //sample setup
  bellFreq = (float)bell_SAMPLERATE / (float) 96359;
  bellrollFreq = (float)bellroll_SAMPLERATE / (float) 96359;
  bowgloFreq = (float)bowglo_SAMPLERATE / (float) 96359;
  clariFreq = (float)clari_SAMPLERATE / (float) 96359;
//  guitFreq = (float)guit_SAMPLERATE / (float) 96359;
  junoFreq = (float)juno_SAMPLERATE / (float) 96359;
  meltineFreq = (float)meltine_SAMPLERATE / (float) 96359;
  ocarinaFreq = (float)ocarina_SAMPLERATE / (float) 96359;

  bellSamp.setFreq(bellFreq * 2.0f); 
  bellrollSamp.setFreq(bellrollFreq); 
  bowgloSamp.setFreq(bowgloFreq); 
  clariSamp.setFreq(clariFreq); 
  guitSamp.setFreq(guitFreq); 
  junoSamp.setFreq(junoFreq); 
  meltineSamp.setFreq(meltineFreq); 
  ocarinaSamp.setFreq(ocarinaFreq); 

  bellSamp.setLoopingOn();
  bellrollSamp.setLoopingOn();
  bowgloSamp.setLoopingOn();
  clariSamp.setLoopingOn();
  guitSamp.setLoopingOn();
  junoSamp.setLoopingOn();
  meltineSamp.setLoopingOn();
  ocarinaSamp.setLoopingOn();

  bellSamp.setEnd(bell_NUM_CELLS);
  bellrollSamp.setEnd(bellroll_NUM_CELLS);
  bowgloSamp.setEnd(bowglo_NUM_CELLS);
  clariSamp.setEnd(clari_NUM_CELLS);
//  guitSamp.setEnd(guit_NUM_CELLS);
  guitSamp.setEnd(junoPerc_NUM_CELLS);
  junoSamp.setEnd(juno_NUM_CELLS);
  meltineSamp.setEnd(meltine_NUM_CELLS);
  ocarinaSamp.setEnd(ocarina_NUM_CELLS);

//Sample <bell_NUM_CELLS, AUDIO_RATE> bellSamp(bell_DATA);
//Sample <bellroll_NUM_CELLS, AUDIO_RATE> bellrollSamp(bellroll_DATA);
//Sample <bowglo_NUM_CELLS, AUDIO_RATE> bowgloSamp(bowglo_DATA);
//Sample <clari_NUM_CELLS, AUDIO_RATE> clariSamp(clari_DATA);
//Sample <guit_NUM_CELLS, AUDIO_RATE> guitSamp(guit_DATA);
//Sample <juno_NUM_CELLS, AUDIO_RATE> junoSamp(juno_DATA);
//Sample <meltine_NUM_CELLS, AUDIO_RATE> meltineSamp(meltine_DATA);
//Sample <ocarina_NUM_CELLS, AUDIO_RATE> ocarinaSamp(ocarina_DATA);

  glitchTimer.start(glitchLength);

  for(int i = 0; i < 8; i++){
    envelopes[i].setADLevels(255,255);
//    envelopes[i].setTimes(500, 10, 10, 500);
    envelopes[i].setAttackTime(3000);
    envelopes[i].setSustainTime(30000);
    envelopes[i].setReleaseTime(3000);
    gain[i] = 0;
  }
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switch65es
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  entropy = map(potVals[0], 0, 4095, 0, 100);

  myRandom = xorshift96();
  myMappedNum = myRandom % 100;

//  if(myMappedNum < (entropy)/2){
//    int ind = myMappedNum % 8;
//    if(glitchOnOff[ind] == 1){
//      glitchOnOff[ind] = 0;
//    } else{
//      glitchOnOff[ind] = 1;
//    }
//  }

  myRandom = xorshift96();
  myMappedNum = myRandom % 100;

  if(myMappedNum < entropy){ //random octave glitching
    int ind = myMappedNum % 8;
    if(envelopes[ind].playing()){
      if(glitchOct[ind] == 1){
        glitchOct[ind] = 2;
      } else if (glitchOct[ind] == 2){
        glitchOct[ind] = 4;
      } else if (glitchOct[ind] == 4){
        glitchOct[ind] = 0.5;
      } else {
        glitchOct[ind] = 1;
      }
    
      switch(ind){
        case 0:
          bellSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 1:
          bellrollSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 2:
          bowgloSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 3:
          clariSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 4:
          guitSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 5:
          junoSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 6:
          meltineSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
        case 7:
          ocarinaSamp.setFreq(glitchOct[ind] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)ind] + rootOffset)/12.0f)));
          break;
      }
    }

    
  }

  for (int i = 0; i < 8; i++){
    envelopes[i].update();
  }

   for (int i = 0; i < 8; i++){
    gain[i] = envelopes[i].next();
  }
}


AudioOutput_t updateAudio(){
  int sample = bellSamp.next() * gain[0] * glitchOnOff[0] +
  bellrollSamp.next() * gain[1] * glitchOnOff[1]  + 
  bowgloSamp.next() * gain[2] * glitchOnOff[2]  + 
  clariSamp.next() * gain[3] * glitchOnOff[3]  + 
  guitSamp.next() * gain[4] * glitchOnOff[4]  + 
  junoSamp.next() * gain[5] * glitchOnOff[5]  + 
  meltineSamp.next() * gain[6] * glitchOnOff[6]  + 
  ocarinaSamp.next() * gain[7] * glitchOnOff[7];
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
      glitchOnOff[i] = 1;
      glitchOct[i] = 1;
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            Serial.println("pad 0 pressed");
              envelopes[i].noteOn();
              bellSamp.start();
            if (false){
            } else{
              bellSamp.setFreq(glitchOct[i] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 0 released
            Serial.println("pad 0 released");
            envelopes[i].noteOff();
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
              envelopes[i].noteOn();
              bellrollSamp.start();
            if (false){
            } else{
              bellrollSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 1 released
            Serial.println("pad 1 released");
              envelopes[i].noteOff();
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
              envelopes[i].noteOn();
              bowgloSamp.start();
            if (false){
            } else{
              bowgloSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 2 released
            Serial.println("pad 2 released");
              envelopes[i].noteOff();
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
              envelopes[i].noteOn();
              clariSamp.start();
            if (false){
            } else{
              clariSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 3 released
            Serial.println("pad 3 released");
              envelopes[i].noteOff();
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
              envelopes[i].noteOn();
              guitSamp.start();
            if (false){
            } else{
              guitSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 4 released
            Serial.println("pad 4 released");
              envelopes[i].noteOff();
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
              envelopes[i].noteOn();
              junoSamp.start();
            if (false){
            } else{
              junoSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 5 released
            Serial.println("pad 5 released");
              envelopes[i].noteOff();
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
              envelopes[i].noteOn();
              meltineSamp.start();
            if (false){
            } else{
              meltineSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 6 released
            Serial.println("pad 6 released");
              envelopes[i].noteOff();
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
            Serial.println(dipPressed);
              envelopes[i].noteOn();
              ocarinaSamp.start();
            if (false){
            } else{
              ocarinaSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 7 released
            Serial.println("pad 7 released");
              envelopes[i].noteOff();
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
            dipPressed++;
            bellSamp.setTable(bell_DATA);
            bellrollSamp.setTable(bell_DATA);
            bowgloSamp.setTable(bell_DATA);
            clariSamp.setTable(bell_DATA);
            guitSamp.setTable(bell_DATA);
            junoSamp.setTable(bell_DATA);
            meltineSamp.setTable(bell_DATA);
            ocarinaSamp.setTable(bell_DATA);

            bellSamp.setEnd(bell_NUM_CELLS);
            bellrollSamp.setEnd(bell_NUM_CELLS);
            bowgloSamp.setEnd(bell_NUM_CELLS);
            clariSamp.setEnd(bell_NUM_CELLS);
            guitSamp.setEnd(bell_NUM_CELLS);
            junoSamp.setEnd(bell_NUM_CELLS);
            meltineSamp.setEnd(bell_NUM_CELLS);
            ocarinaSamp.setEnd(bell_NUM_CELLS);
            
            Serial.println(dipPressed);
          } else {
            Serial.println("DIP 1 down");
            dipPressed--;
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
              for (int i = 0; i < 8; i++){
                envelopes[i].setAttackTime(20);
              }
//            bellSamp.setTable(bellroll_DATA);
//            bellrollSamp.setTable(bellroll_DATA);
//            bowgloSamp.setTable(bellroll_DATA);
//            clariSamp.setTable(bellroll_DATA);
//            guitSamp.setTable(bellroll_DATA);
//            junoSamp.setTable(bellroll_DATA);
//            meltineSamp.setTable(bellroll_DATA);
//            ocarinaSamp.setTable(bellroll_DATA);
//
//            bellSamp.setEnd(bellroll_NUM_CELLS);
//            bellrollSamp.setEnd(bellroll_NUM_CELLS);
//            bowgloSamp.setEnd(bellroll_NUM_CELLS);
//            clariSamp.setEnd(bellroll_NUM_CELLS);
//            guitSamp.setEnd(bellroll_NUM_CELLS);
//            junoSamp.setEnd(bellroll_NUM_CELLS);
//            meltineSamp.setEnd(bellroll_NUM_CELLS);
//            ocarinaSamp.setEnd(bellroll_NUM_CELLS);
//            dipPressed++;
          } else {
            Serial.println("DIP 2 down");
              for (int i = 0; i < 8; i++){
                envelopes[i].setAttackTime(3000);
              }
//            dipPressed--;
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
            bellSamp.setTable(bowglo_DATA);
            bellrollSamp.setTable(bowglo_DATA);
            bowgloSamp.setTable(bowglo_DATA);
            clariSamp.setTable(bowglo_DATA);
            guitSamp.setTable(bowglo_DATA);
            junoSamp.setTable(bowglo_DATA);
            meltineSamp.setTable(bowglo_DATA);
            ocarinaSamp.setTable(bowglo_DATA);

            bellSamp.setEnd(bowglo_NUM_CELLS);
            bellrollSamp.setEnd(bowglo_NUM_CELLS);
            bowgloSamp.setEnd(bowglo_NUM_CELLS);
            clariSamp.setEnd(bowglo_NUM_CELLS);
            guitSamp.setEnd(bowglo_NUM_CELLS);
            junoSamp.setEnd(bowglo_NUM_CELLS);
            meltineSamp.setEnd(bowglo_NUM_CELLS);
            ocarinaSamp.setEnd(bowglo_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 3 down");
            dipPressed--;
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 4 up");
            bellSamp.setTable(clari_DATA);
            bellrollSamp.setTable(clari_DATA);
            bowgloSamp.setTable(clari_DATA);
            clariSamp.setTable(clari_DATA);
            guitSamp.setTable(clari_DATA);
            junoSamp.setTable(clari_DATA);
            meltineSamp.setTable(clari_DATA);
            ocarinaSamp.setTable(clari_DATA);

            bellSamp.setEnd(clari_NUM_CELLS);
            bellrollSamp.setEnd(clari_NUM_CELLS);
            bowgloSamp.setEnd(clari_NUM_CELLS);
            clariSamp.setEnd(clari_NUM_CELLS);
            guitSamp.setEnd(clari_NUM_CELLS);
            junoSamp.setEnd(clari_NUM_CELLS);
            meltineSamp.setEnd(clari_NUM_CELLS);
            ocarinaSamp.setEnd(clari_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 4 down");
            dipPressed--;
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 5 up");
//            bellSamp.setTable(guit_DATA);
//            bellrollSamp.setTable(guit_DATA);
//            bowgloSamp.setTable(guit_DATA);
//            clariSamp.setTable(guit_DATA);
//            guitSamp.setTable(guit_DATA);
//            junoSamp.setTable(guit_DATA);
//            meltineSamp.setTable(guit_DATA);
//            ocarinaSamp.setTable(guit_DATA);
//
//            bellSamp.setEnd(guit_NUM_CELLS);
//            bellrollSamp.setEnd(guit_NUM_CELLS);
//            bowgloSamp.setEnd(guit_NUM_CELLS);
//            clariSamp.setEnd(guit_NUM_CELLS);
//            guitSamp.setEnd(guit_NUM_CELLS);
//            junoSamp.setEnd(guit_NUM_CELLS);
//            meltineSamp.setEnd(guit_NUM_CELLS);
//            ocarinaSamp.setEnd(guit_NUM_CELLS);
            bellSamp.setTable(junoPerc_DATA);
            bellrollSamp.setTable(junoPerc_DATA);
            bowgloSamp.setTable(junoPerc_DATA);
            clariSamp.setTable(junoPerc_DATA);
            guitSamp.setTable(junoPerc_DATA);
            junoSamp.setTable(junoPerc_DATA);
            meltineSamp.setTable(junoPerc_DATA);
            ocarinaSamp.setTable(junoPerc_DATA);

            bellSamp.setEnd(junoPerc_NUM_CELLS);
            bellrollSamp.setEnd(junoPerc_NUM_CELLS);
            bowgloSamp.setEnd(junoPerc_NUM_CELLS);
            clariSamp.setEnd(junoPerc_NUM_CELLS);
            guitSamp.setEnd(junoPerc_NUM_CELLS);
            junoSamp.setEnd(junoPerc_NUM_CELLS);
            meltineSamp.setEnd(junoPerc_NUM_CELLS);
            ocarinaSamp.setEnd(junoPerc_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 5 down");
            dipPressed--;
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 6 up");
            bellSamp.setTable(juno_DATA);
            bellrollSamp.setTable(juno_DATA);
            bowgloSamp.setTable(juno_DATA);
            clariSamp.setTable(juno_DATA);
            guitSamp.setTable(juno_DATA);
            junoSamp.setTable(juno_DATA);
            meltineSamp.setTable(juno_DATA);
            ocarinaSamp.setTable(juno_DATA);

            bellSamp.setEnd(juno_NUM_CELLS);
            bellrollSamp.setEnd(juno_NUM_CELLS);
            bowgloSamp.setEnd(juno_NUM_CELLS);
            clariSamp.setEnd(juno_NUM_CELLS);
            guitSamp.setEnd(juno_NUM_CELLS);
            junoSamp.setEnd(juno_NUM_CELLS);
            meltineSamp.setEnd(juno_NUM_CELLS);
            ocarinaSamp.setEnd(juno_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 6 down");
            dipPressed--;
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 7 up");
            bellSamp.setTable(meltine_DATA);
            bellrollSamp.setTable(meltine_DATA);
            bowgloSamp.setTable(meltine_DATA);
            clariSamp.setTable(meltine_DATA);
            guitSamp.setTable(meltine_DATA);
            junoSamp.setTable(meltine_DATA);
            meltineSamp.setTable(meltine_DATA);
            ocarinaSamp.setTable(meltine_DATA);

            bellSamp.setEnd(meltine_NUM_CELLS);
            bellrollSamp.setEnd(meltine_NUM_CELLS);
            bowgloSamp.setEnd(meltine_NUM_CELLS);
            clariSamp.setEnd(meltine_NUM_CELLS);
            guitSamp.setEnd(meltine_NUM_CELLS);
            junoSamp.setEnd(meltine_NUM_CELLS);
            meltineSamp.setEnd(meltine_NUM_CELLS);
            ocarinaSamp.setEnd(meltine_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 7 down");
            dipPressed--;
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 8 up");
            bellSamp.setTable(ocarina_DATA);
            bellrollSamp.setTable(ocarina_DATA);
            bowgloSamp.setTable(ocarina_DATA);
            clariSamp.setTable(ocarina_DATA);
            guitSamp.setTable(ocarina_DATA);
            junoSamp.setTable(ocarina_DATA);
            meltineSamp.setTable(ocarina_DATA);
            ocarinaSamp.setTable(ocarina_DATA);

            bellSamp.setEnd(ocarina_NUM_CELLS);
            bellrollSamp.setEnd(ocarina_NUM_CELLS);
            bowgloSamp.setEnd(ocarina_NUM_CELLS);
            clariSamp.setEnd(ocarina_NUM_CELLS);
            guitSamp.setEnd(ocarina_NUM_CELLS);
            junoSamp.setEnd(ocarina_NUM_CELLS);
            meltineSamp.setEnd(ocarina_NUM_CELLS);
            ocarinaSamp.setEnd(ocarina_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 8 down");
            dipPressed--;
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
