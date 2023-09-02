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
#include "harp_c3.h"

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

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

// variables for potentiometers
int potVals[] = {0, 0};

// variables for sample
Sample <harp_c3_NUM_CELLS, AUDIO_RATE> aSample(harp_c3_DATA);
float aSampleFreq;




void setup(){
  startMozzi();
  Serial.begin(115200);
  aSampleFreq =  (float)harp_c3_SAMPLERATE / (float) harp_c3_NUM_CELLS;
  aSample.setFreq(aSampleFreq); // play at the speed it was recorded
}


void loop(){
  audioHook();
}


void updateControl(){
  readTouch();
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSample.next());
}

//
//void readTouch(){
//  int pinVal = 0;
//  for (int i = 0; i < 8; i++){
//    pinVal = touchRead(touchPins[i]);   
//    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal; 
//    if (touchAvgs[i] < touchThreshold){
//      touchVals[i] = 1; 
//    } else {
//      touchVals[i] = 0;
//    }
//    if (touchVals[i] != prevTouchVals[i]){
//      switch(i){
//        case 0:
//          if(touchVals[i]){ // pad 0 pressed
//            Serial.println("0");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 0/12)));
//          } else { // pad 0 released
//
//          }
//          break;
//        case 1:
//          if(touchVals[i]){ // pad 1 pressed
//            Serial.println("1");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 2/12)));
//          } else { // pad 1 released
//
//          }
//          break;
//        case 2:
//          if(touchVals[i]){ // pad 2 pressed
//            Serial.println("2");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 4/12)));
//          } else { // pad 2 released
//
//          }
//          break;
//        case 3:
//          if(touchVals[i]){ // pad 3 pressed
//            Serial.println("3");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 5/12)));
//          } else { // pad 3 released
//
//          }
//          break;
//        case 4:
//          if(touchVals[i]){ // pad 4 pressed
//            Serial.println("4");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE /harp_c3_NUM_CELLS ) * pow(2, 7/12)));
//          } else { // pad 4 released
//
//          }
//          break;
//        case 5:
//          if(touchVals[i]){ // pad 5 pressed
//            Serial.println("5");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE /harp_c3_NUM_CELLS ) * pow(2, 9/12)));
//          } else { // pad 5 released
//
//          }
//          break;
//        case 6:
//          if(touchVals[i]){ // pad 6 pressed
//            Serial.println("6");
//            aSample.start();
//            aSample.setFreq((int)((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 11/12)));
//          } else { // pad 6 released
//
//          }
//          break;
//        case 7:
//          if(touchVals[i]){ // pad 7 pressed
//            Serial.println("7");
//            aSample.start();
//            aSample.setFreq((int) ((harp_c3_SAMPLERATE / harp_c3_NUM_CELLS ) * pow(2, 12/12)));
//          } else { // pad 7 released
//
//          }
//          break;
//      }
//    }
//    prevTouchVals[i] = touchVals[i];
//  }
//}


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
            aSample.setFreq((float)(aSampleFreq * pow(2, 11.0/12.0)));
            aSample.start();
          } else { // pad 6 released

          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
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
  potVals[0] = mozziAnalogRead(0);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(1);
}
