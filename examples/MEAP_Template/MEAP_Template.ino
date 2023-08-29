/*
  Provides a basic blank template for building MEAP programs
  Includes definitions of read functions but doesn't use them.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

// variables for DIP switches
Mux mux(Pin(5, INPUT, PinType::Digital), Pinset(16, 17, 18));
int dipPins[] = {2, 1, 0, 3, 4, 7, 5, 6};
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {4, 13, 27, 33, 2, 15, 14, 32};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

// variables for potentiometers
int potVals[] = {0, 0};



void setup(){
  startMozzi();
}


void loop(){
  audioHook();
}


void updateControl(){
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(0); 
}

void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
    mux.read(i); //read once and throw away result (for reliability)
    dipVals[dipPins[i]] = mux.read(i);
  }

  //Check if any switches changed position
  for (int i = 0; i < 8; i++){
    switch(i){
      case 0:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 1 up
            
          } else { // DIP 1 down
            
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 2 up

          } else { // DIP 2 down

          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 3 up

          } else { // DIP 3 down

          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 4 up

          } else { // DIP 4 down

          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 5 up

          } else { // DIP 5 down

          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 6 up

          } else { // DIP 6 down

          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 7 up

          } else { // DIP 7 down

          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){ // DIP 8 up

          } else { // DIP 8 down

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
    touchRead(touchPins[i]);  
    pinVal = touchRead(touchPins[i]);   
    if (pinVal < threshold){
      touchVals[i] = 1; 
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]){
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed

          } else { // pad 0 released

          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed

          } else { // pad 1 released

          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed

          } else { // pad 2 released

          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed

          } else { // pad 3 released

          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed

          } else { // pad 4 released

          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed

          } else { // pad 5 released

          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed

          } else { // pad 6 released

          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed

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
