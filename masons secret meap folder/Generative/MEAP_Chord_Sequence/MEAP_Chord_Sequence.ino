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
#include <tables/sin8192_int8.h> // loads sine wavetable
#include <tables/triangle_warm8192_int8.h>
#include <EventDelay.h>
#include <Ead.h>

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

Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc1(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc2(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc3(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc4(TRIANGLE_WARM8192_DATA);

Ead env1(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead env2(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead env3(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead env4(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain1 = 0;
int gain2 = 0;
int gain3 = 0;
int gain4 = 0;

int majScale[] = {0, 2, 4, 5, 7, 9, 11};
int scaleRoot = 60;
int chordRoot = 2;
int triad[] = {0, 2, 4};

EventDelay noteDelay;
int noteLength = 400;

int treeLevel = 4;

int arpCounter = 0;


void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);
  
  osc1.setFreq(mtof(scaleRoot + majScale[(triad[0] + chordRoot)%8]));
  osc2.setFreq(mtof(scaleRoot + majScale[(triad[1] + chordRoot)%8]));
  osc3.setFreq(mtof(scaleRoot + majScale[(triad[2] + chordRoot)%8]));
  osc4.setFreq(2 * mtof(scaleRoot + majScale[(triad[0] + chordRoot)%8]));

  env1.setAttack(10);
  env1.setDecay(1000);
  env2.setAttack(10);
  env2.setDecay(1000);
  env3.setAttack(10);
  env3.setDecay(1000);
  env4.setAttack(10);
  env4.setDecay(1000);

  noteDelay.start(noteLength); 

  randomSeed(10); 
  xorshiftSeed((long)random(1000));
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switches
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

//  setAtkDec(map(potVals[0], 0, 4095, 10, 2000), map(potVals[1], 0, 4095, 10, 2000));
  noteLength = map(potVals[1], 0, 4095, 50, 2000);
  setAtkDec(map(potVals[0], 0, 4095, 10, 2000), 2000);

  if(noteDelay.ready()){
    if(arpCounter == 4){
      arpCounter = 0;
      uint16_t myRandom = 0;
      int myMappedNum = 0;
  
      if(treeLevel != 0){
        treeLevel = treeLevel - 1;
      } else{
        myRandom = xorshift96();
        treeLevel = map(myRandom, 0, 65535, 0, 4);
      }
      switch(treeLevel){
        case 4:
          chordRoot = 2;
          break;
        case 3:
              myRandom = xorshift96();
              myMappedNum = map(myRandom, 0, 65535, 0, 3);
              if (myMappedNum == 0){
                chordRoot = 0;
              } else{
                chordRoot = 5;
              }
          break;
        case 2:
              myRandom = xorshift96();
              myMappedNum = map(myRandom, 0, 65535, 0, 2);
              if (myMappedNum == 0){
                chordRoot = 1;
              } else{
                chordRoot = 3;
              }
          break;
        case 1:
              myRandom = xorshift96();
              myMappedNum = map(myRandom, 0, 65535, 0, 3);
              if (myMappedNum == 0){
                chordRoot = 6;
              } else{
                chordRoot = 4;
              }
          break;
        case 0:
              myRandom = xorshift96();
              myMappedNum = map(myRandom, 0, 65535, 0, 3);
              Serial.println(myMappedNum);
              Serial.println(myMappedNum);
              Serial.println(myMappedNum);
              if (myMappedNum == 0){
                chordRoot = 5;
              } else{
                chordRoot = 0;
              }
          break;
      }
      Serial.print(treeLevel);
      Serial.print(" ");
      Serial.println(chordRoot+1);
      osc1.setFreq(mtof(scaleRoot + majScale[(triad[0] + chordRoot)%7]));
      osc2.setFreq(mtof(scaleRoot + majScale[(triad[1] + chordRoot)%7]));
      osc3.setFreq(mtof(scaleRoot + majScale[(triad[2] + chordRoot)%7]));
      osc4.setFreq(2 * mtof(scaleRoot + majScale[(triad[0] + chordRoot)%7]));
    } else {
    noteDelay.start(noteLength);
    switch(arpCounter){
      case 0:
        env1.start();
        break;
      case 1:
        env2.start();
        break;
      case 2:
        env3.start();
        break;
      case 3:
        env4.start();
    }
    arpCounter++;
  }
}

  gain1 = env1.next();
  gain2 = env2.next();
  gain3 = env3.next();
  gain4 = env4.next();
}


int updateAudio(){
  return MonoOutput::fromAlmostNBit(18, gain1 * osc1.next() + gain2 * osc2.next() + gain3 * osc3.next() + gain4 * osc4.next());
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
            env1.start();
            env2.start();
            env3.start();
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

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}

void setAtkDec(int atk, int dec){
  env1.setAttack(atk);
  env1.setDecay(dec);
  env2.setAttack(atk);
  env2.setDecay(dec);
  env3.setAttack(atk);
  env3.setDecay(dec);
  env4.setAttack(atk);
  env4.setDecay(dec);
}
