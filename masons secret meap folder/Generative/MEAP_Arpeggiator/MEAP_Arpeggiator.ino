/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.


  MODES:
    chromatic
    diatonic
    triad
    seventh

    knobs control
      arp speed
      num of notes added
      
  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <EventDelay.h>
#include <SPI.h>
#include <Ead.h>
#include <tables/sin8192_int8.h> // loads sine wavetable

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

#define NUM_OSC 8

#define DIR_EN 0

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

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> sine1(SIN8192_DATA);

Ead env1(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain1 = 0;


Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> sines[NUM_OSC] = Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> (SIN8192_DATA);
Ead envs[NUM_OSC](CONTROL_RATE);
int gains[NUM_OSC];
EventDelay timers[NUM_OSC];
int arpCounters[NUM_OSC];
int currNotes[NUM_OSC];
int arpLengths[NUM_OSC];
int arpIntervals[NUM_OSC];
int currOsc = 0;
int prevNote = 0;
int arpDir = 1;


EventDelay arpTimer;
int arpLength = 200;
int arpCounter = 0;
int arpNotes = 4;
int arpInterval = 2;

int majScale[] = {0, 2, 4, 5, 7, 9, 11, 12};
int scaleRoot = 60; // root of major scale
//int triad[] = {0, 2, 4};
//int seventh[] = {0, 2, 4, 6};
int chordRoot = 0;
int currNote = 0;


void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);
  sine1.setFreq(440); //set frequency of sine oscillator
  env1.set(10, 2000);

  for (int i = 0; i < NUM_OSC; i++){
    sines[i].setFreq(440);
    envs[i].set(10, 2000);
    arpCounters[i] = 0;
    gains[i] = 0;
    currNotes[i] = 0;
    arpLengths[i] = 0;
    arpIntervals[i] = 2;
  }
}


void loop(){
  audioHook();
}


void updateControl(){
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers
  readDip();

  arpNotes = map(potVals[0], 0, 4095, 1, 12);
  arpLength = map(potVals[1], 0, 4095, 600, 30);

  // PLAY NEXT ARP NOTE
  for (int i = 0; i < NUM_OSC; i++){
    if((arpCounters[i] > 0) && timers[i].ready()){
      arpCounters[i]--;
      currNotes[i] += arpIntervals[i] * arpDir;
      int myNote = currNotes[i];
      int myOctave = 0;
      if(myNote > 0){
        while(myNote > 7){
          myNote -= 7;
          myOctave += 12;
        }
      } else {
        while(myNote < 0){
          myNote += 7;
          myOctave -= 12;
        }
      }

      sines[i].setFreq(mtof(scaleRoot + majScale[myNote] + myOctave));
      envs[i].start();
      timers[i].start(arpLengths[i]);
    }
  }

  // CALCULATE GAINS
  for (int i = 0; i < NUM_OSC; i++){
    gains[i] = envs[i].next();
  }
}


int updateAudio(){
  int mySample = 0;
  for (int i = 0; i < NUM_OSC; i++){
    mySample += sines[i].next() * gains[i];
  }
//  return MonoOutput::fromAlmostNBit(16, sine1.next() * gain1);
  return MonoOutput::fromAlmostNBit(18, mySample);
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
            currOsc = (currOsc + 1) % NUM_OSC;
            envs[currOsc].start();
            currNotes[currOsc] = i;
            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
            arpCounters[currOsc] = arpNotes;
            arpLengths[currOsc] = arpLength;
            timers[currOsc].start(arpLengths[currOsc]);
            arpIntervals[currOsc] = arpInterval;
            if(DIR_EN){
              if (prevNote <= i){
                arpDir = 1;
              } else {
                arpDir = -1;
              }
              prevNote = i;
            }
      }
//      switch(i){
//        case 0:
//          if(touchVals[i]){ // pad 0 pressed
//            Serial.println("pad 0 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 0;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
////            env1.start();
////            currNote = 0;
////            sine1.setFreq(mtof(scaleRoot + majScale[currNote]));
////            arpCounter = arpNotes;
////            arpTimer.start(arpLength);
//          } else { // pad 0 released
//            Serial.println("pad 0 released");
//          }
//          break;
//        case 1:
//          if(touchVals[i]){ // pad 1 pressed
//            Serial.println("pad 1 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 1;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 1 released
//            Serial.println("pad 1 released");
//          }
//          break;
//        case 2:
//          if(touchVals[i]){ // pad 2 pressed
//            Serial.println("pad 2 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 2;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 2 released
//            Serial.println("pad 2 released");
//          }
//          break;
//        case 3:
//          if(touchVals[i]){ // pad 3 pressed
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 3;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 3 released
//            Serial.println("pad 3 released");
//          }
//          break;
//        case 4:
//          if(touchVals[i]){ // pad 4 pressed
//            Serial.println("pad 4 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 4;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 4 released
//            Serial.println("pad 4 released");
//          }
//          break;
//        case 5:
//          if(touchVals[i]){ // pad 5 pressed
//            Serial.println("pad 5 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 5;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 5 released
//            Serial.println("pad 5 released");
//          }
//          break;
//        case 6:
//          if(touchVals[i]){ // pad 6 pressed
//            Serial.println("pad 6 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 6;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 6 released
//            Serial.println("pad 6 released");
//          }
//          break;
//        case 7:
//          if(touchVals[i]){ // pad 7 pressed
//            Serial.println("pad 7 pressed");
//            currOsc = (currOsc + 1) % NUM_OSC;
//            envs[currOsc].start();
//            currNotes[currOsc] = 7;
//            sines[currOsc].setFreq(mtof(scaleRoot + majScale[currNotes[currOsc]]));
//            arpCounters[currOsc] = arpNotes;
//            arpLengths[currOsc] = arpLength;
//            timers[currOsc].start(arpLengths[currOsc]);
//            arpIntervals[currOsc] = arpInterval;
//            if (prevNote >= i){
//              arpDir = 1;
//            } else {
//              arpDir = -1;
//            }
//            prevNote = i;
//          } else { // pad 7 released
//            Serial.println("pad 7 released");
//          }
//          break;
//      }
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
            arpInterval = 1;
          } else {
            Serial.println("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
            arpInterval = 2;
          } else {
            Serial.println("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
            arpInterval = 3;
          } else {
            Serial.println("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 4 up");
            arpInterval = 4;
          } else {
            Serial.println("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 5 up");
            arpInterval = 5;
          } else {
            Serial.println("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 6 up");
            arpInterval = 6;
          } else {
            Serial.println("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 7 up");
            arpInterval = 7;
          } else {
            Serial.println("DIP 7 down");
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 8 up");
            arpInterval = 8;
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
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}
