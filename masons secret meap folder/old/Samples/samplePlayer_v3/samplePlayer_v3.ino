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

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));

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
    if(touchVals[ind]){
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

  
}


int updateAudio(){
//  return MonoOutput::fromAlmostNBit(8, bellSamp.next());
  return MonoOutput::fromAlmostNBit(10, 
  bellSamp.next() * gain[0] * glitchOnOff[0] +
  bellrollSamp.next() * gain[1] * glitchOnOff[1]  + 
  bowgloSamp.next() * gain[2] * glitchOnOff[2]  + 
  clariSamp.next() * gain[3] * glitchOnOff[3]  + 
  guitSamp.next() * gain[4] * glitchOnOff[4]  + 
  junoSamp.next() * gain[5] * glitchOnOff[5]  + 
  meltineSamp.next() * gain[6] * glitchOnOff[6]  + 
  ocarinaSamp.next() * gain[7] * glitchOnOff[7] );
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
      glitchOnOff[i] = 1;
      glitchOct[i] = 1;
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            Serial.println("pad 0 pressed");
              gain[0] = 1;
              bellSamp.start();
            if (dipPressed <= 0){
            } else{
              bellSamp.setFreq(glitchOct[i] * (float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 0 released
            Serial.println("pad 0 released");
            gain[0] = 0;
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
              gain[1] = 1;
              bellrollSamp.start();
            if (dipPressed <= 0){
            } else{
              bellrollSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 1 released
            Serial.println("pad 1 released");
              gain[1] = 0;
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
              gain[2] = 1;
              bowgloSamp.start();
            if (dipPressed <= 0){
            } else{
              bowgloSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 2 released
            Serial.println("pad 2 released");
              gain[2] = 0;
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
              gain[3] = 1;
              clariSamp.start();
            if (dipPressed <= 0){
            } else{
              clariSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 3 released
            Serial.println("pad 3 released");
              gain[3] = 0;
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
              gain[4] = 1;
              guitSamp.start();
            if (dipPressed <= 0){
            } else{
              guitSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 4 released
            Serial.println("pad 4 released");
              gain[4] = 0;
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
              gain[5] = 1;
              junoSamp.start();
            if (dipPressed <= 0){
            } else{
              junoSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 5 released
            Serial.println("pad 5 released");
              gain[5] = 0;
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
              gain[6] = 1;
              meltineSamp.start();
            if (dipPressed <= 0){
            } else{
              meltineSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 6 released
            Serial.println("pad 6 released");
              gain[6] = 0;
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
              gain[7] = 1;
              ocarinaSamp.start();
            if (dipPressed <= 0){
            } else{
              ocarinaSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)i] + rootOffset)/12.0f)));
            }
          } else { // pad 7 released
            Serial.println("pad 7 released");
              gain[7] = 0;
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }

//  if (padPressNum != -1 && dipPressed >= 1){
//    for (int i = 0; i < 8; i++){
//      if (dipVals[i] == 0){
//        switch(i){
//          case(0):
//            bellSamp.setFreq((float)(bellFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            bellSamp.start();
//            break;
//          case(1):
//            bellrollSamp.setFreq((float)(bellrollFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            bellrollSamp.start();
//            break;
//          case(2):
//            bowgloSamp.setFreq((float)(bowgloFreq * (float)pow(2, ((float)majScale[(int)padPressNum]-12 + rootOffset)/12.0f)));
//            bowgloSamp.start();
//            break;
//          case(3):
//            clariSamp.setFreq((float)(clariFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            clariSamp.start();
//            break;
//          case(4):
//            guitSamp.setFreq((float)(guitFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            guitSamp.start();
//            break;
//          case(5):
//            junoSamp.setFreq((float)(junoFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            junoSamp.start();
//            break;
//          case(6):
//            meltineSamp.setFreq((float)(meltineFreq * (float)pow(2, ((float)majScale[(int)padPressNum] + rootOffset)/12.0f)));
//            meltineSamp.start();
//            break;
//          case(7):
//            ocarinaSamp.setFreq((float)(ocarinaFreq * (float)pow(2, ((float)majScale[(int)padPressNum]-12 + rootOffset)/12.0f)));
//            ocarinaSamp.start();
//            break;
//
//        }
//      }
//    }
//    // calc the freq using padPressNum 
//    // determine the sample needed using dip switches
//    // set the freq
//    // trigger the note
//    padPressNum = -1;
//  }
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
            bellSamp.setTable(bellroll_DATA);
            bellrollSamp.setTable(bellroll_DATA);
            bowgloSamp.setTable(bellroll_DATA);
            clariSamp.setTable(bellroll_DATA);
            guitSamp.setTable(bellroll_DATA);
            junoSamp.setTable(bellroll_DATA);
            meltineSamp.setTable(bellroll_DATA);
            ocarinaSamp.setTable(bellroll_DATA);

            bellSamp.setEnd(bellroll_NUM_CELLS);
            bellrollSamp.setEnd(bellroll_NUM_CELLS);
            bowgloSamp.setEnd(bellroll_NUM_CELLS);
            clariSamp.setEnd(bellroll_NUM_CELLS);
            guitSamp.setEnd(bellroll_NUM_CELLS);
            junoSamp.setEnd(bellroll_NUM_CELLS);
            meltineSamp.setEnd(bellroll_NUM_CELLS);
            ocarinaSamp.setEnd(bellroll_NUM_CELLS);
            dipPressed++;
          } else {
            Serial.println("DIP 2 down");
            dipPressed--;
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
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);

  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
  rootOffset = map(potVals[1], 0, 4092, 0, 11);
//  Serial.println(myMap);

}
