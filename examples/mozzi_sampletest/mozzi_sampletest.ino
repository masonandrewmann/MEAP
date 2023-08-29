#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <ADSR.h>
#include <Sample.h>
#include "cmaj7.h"
#include "cmin7.h"
#include <tables/sin8192_int8.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

Mux mux(Pin(5, INPUT, PinType::Digital), Pinset(16, 17, 18));
byte muxPinTable[] = {2, 1, 0, 3, 4, 7, 5, 6};
byte muxVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
byte prevMuxVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> mySine(SIN8192_DATA);
ADSR <AUDIO_RATE, AUDIO_RATE> envelope;
// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <cmaj7_NUM_CELLS, AUDIO_RATE> aSample(cmaj7_DATA);
Sample <cmin7_NUM_CELLS, AUDIO_RATE> bSample(cmin7_DATA);

unsigned int duration, attack, decay, sustain, release_ms;


//int touchPins[] = {4, 2, 15, 13, 14, 27, 33, 32};
int touchPins[] = {4, 13, 27, 33, 2, 15, 14, 32};

int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

int threshold = 20;

void setup() {
  Serial.begin(115200);
  startMozzi();
  mySine.setFreq(440); //set frequency of Oscillator

  attack = 100;
  decay = 100;
  sustain = 100;
  release_ms = 500;
  byte attackLevel = 200;
  byte decayLevel = 150;
  envelope.setADLevels(attackLevel, decayLevel);
  envelope.setTimes(attack, decay, sustain, release_ms);

  aSample.setFreq((float) cmaj7_SAMPLERATE / (float) cmaj7_NUM_CELLS); // play at the speed it was recorded
  bSample.setFreq((float) cmin7_SAMPLERATE / (float) cmin7_NUM_CELLS); // play at the speed it was recorded
}


void loop() {
  audioHook();
}

void updateControl(){
  updateTouch();
}

AudioOutput_t updateAudio(){
  envelope.update();
//  return MonoOutput::from16Bit((int) (envelope.next() * mySine.next()));
  return MonoOutput::fromAlmostNBit(9, (int) aSample.next() + (int) bSample.next());
}
  
void updateTouch(){
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
              aSample.start();
              float sr = ((float) cmaj7_SAMPLERATE / (float) cmaj7_NUM_CELLS) * pow(2.0, 0.0/12.0);
              aSample.setFreq(sr);
//            Serial.println("pad 0 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
              bSample.start();
              float sr = ((float) cmin7_SAMPLERATE / (float) cmin7_NUM_CELLS) * pow(2.0, 2.0/12.0);
              bSample.setFreq(sr);
//            Serial.println("pad 1 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
              bSample.start();
              float sr = ((float) cmin7_SAMPLERATE / (float) cmin7_NUM_CELLS) * pow(2.0, 4.0/12.0);
              bSample.setFreq(sr);
//            Serial.println("pad 2 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
              aSample.start();
              float sr = ((float) cmaj7_SAMPLERATE / (float) cmaj7_NUM_CELLS) * pow(2.0, 5.0/12.0);
              aSample.setFreq(sr);
//            Serial.println("pad 3 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
              aSample.start();
              float sr = ((float) cmaj7_SAMPLERATE / (float) cmaj7_NUM_CELLS) * pow(2.0, 7.0/12.0);
              aSample.setFreq(sr);
//            Serial.println("pad 4 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
              bSample.start();
              float sr = ((float) cmin7_SAMPLERATE / (float) cmin7_NUM_CELLS) * pow(2.0, 9.0/12.0);
              bSample.setFreq(sr);
//            Serial.println("pad 5 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
              bSample.start();
              float sr = ((float) cmin7_SAMPLERATE / (float) cmin7_NUM_CELLS) * pow(2.0, 11.0/12.0);
              bSample.setFreq(sr);
//            Serial.println("pad 6 pressed");
          } else { // pad 0 released
            
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
              aSample.start();
              float sr = ((float) cmaj7_SAMPLERATE / (float) cmaj7_NUM_CELLS) * pow(2.0, 12.0/12.0);
              aSample.setFreq(sr);
//            Serial.println("pad 7 pressed");
          } else { // pad 0 released
            
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}
