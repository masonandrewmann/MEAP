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
#include <tables/sin8192_int8.h>
#include <Ead.h>
#include <EventDelay.h>

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

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

Oscil <8192, AUDIO_RATE> osc1(SIN8192_DATA);;
Oscil <8192, AUDIO_RATE> osc2(SIN8192_DATA);;

EventDelay myDel;
int delTime = 12;

Ead env1(CONTROL_RATE);
int gain1;

Ead env2(CONTROL_RATE);
int gain2;

int numWaits1 = 0;
int numWaits2 = 0;

int majScale[] = {0, 2, 4, 5, 7, 9, 11}; // template for building major scale on top of 12TET
int scaleRoot = 48; // root of major scale

int seventh[] = {0, 2, 4, 6}; // template for triad on top of major scale
int chordRoot = 0; // root of triad

int octaveOffset = 0;


void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);
  osc1.setFreq(mtof(scaleRoot + majScale[(seventh[0] + chordRoot)%7]));
  osc2.setFreq(mtof(scaleRoot + majScale[(seventh[1] + chordRoot)%7]));
  env1.setAttack(10);
  env1.setDecay(100);
  env2.setAttack(10);
  env2.setDecay(100);

  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switches
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  delTime = map(potVals[0], 0, 4095, 2, 1000);
  octaveOffset = map(potVals[1], 0, 4095, -3, 3);

  if(myDel.ready()){
    numWaits1--;
    numWaits2--;

    myDel.start(delTime);
    if (numWaits1 <= 0){
      numWaits1 = irand(2, 8);
      if(touchVals[4]){
        numWaits1 = 1;
      } else if (touchVals[5]){
        numWaits1 = 4;
      }
      //generate random note
      int myNoteNum = irand(0, 23);
      //calculate mapping values
      int envVal = map(myNoteNum, 0, 23, 1000, 50);
      //apply envelope
      env1.setDecay(envVal);
      //apply frequency
      int myOctave = 0;
      while(myNoteNum > 3){
        myNoteNum -= 4;
        myOctave++;
      }
      osc1.setFreq(mtof(12 + scaleRoot + majScale[(seventh[myNoteNum] + chordRoot)%7] + 12 * myOctave + 12 * octaveOffset));
      env1.start();
    }


    if (numWaits2 <= 0){
      numWaits2 = irand(8, 16);
      if (delTime < 10){
        numWaits2 = 1;
      }
      if(touchVals[6]){
        numWaits2 = 6;
      } else if (touchVals[7]){
        numWaits2 = 12;
      }
      //generate random note
      int myNoteNum = irand(0, 11);
      //calculate mapping values
      int envVal = map(myNoteNum, 0, 11, 3500, 500);
      //apply envelope
      env2.setDecay(envVal);
      //apply frequency
      int myOctave = 0;
      while(myNoteNum > 3){
        myNoteNum -= 4;
        myOctave++;
      }
      osc2.setFreq(mtof(scaleRoot + majScale[(seventh[myNoteNum] + chordRoot)%7] + 12 * myOctave - 12 + 12 * octaveOffset));
      env2.start();
    }
  }

  gain1 = env1.next();
  gain2 = env2.next();
}


int updateAudio(){
  return MonoOutput::fromAlmostNBit(17, osc1.next() * gain1 + osc2.next() * gain2);
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
            chordRoot = 5;
          } else { // pad 1 released
            Serial.println("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
            chordRoot = 3;
          } else { // pad 2 released
            Serial.println("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
            chordRoot = 4;
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
