/*
  Press touch pads 0-5 to toggle sequences on/off
  Pad #6 begins a density ramp of rhythmic vocal chops that lasts 10x as long as pad is held
  Pad #7 cuts out rhythmic vocals
  Pot #0 controls how often glitches occur (entropy)
  Pot #1 controls length of envelopes allowed in glitches (cw = long notes, ccw = short notes)

  DIP 1-5 choose sample ,, bell, bowglo, clari, meltine, juno

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
#include "vox.h"
#include <MIDI.h>
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

#define NUM_SAMP 6
#define NUM_SAMP_PADS 8

#define DEBUG 1 // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

using namespace admux;


// variables for DIP switches
Mux mux(Pin(14, INPUT, PinType::Digital), Pinset(38, 45, 46));
int dipPins[] = {5, 6, 7, 4, 3, 0, 2, 1};
int dipVals[] = {1, 1, 1, 1, 1, 1, 1, 1};
int prevDipVals[] = {1, 1, 1, 1, 1, 1, 1, 1};

// variables for capacitive touch pads
int touchPins[] = {2, 4, 6, 8, 1, 3, 5, 7};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 500;

// variables for potentiometers
int potVals[] = {0, 0};

// variables for samples
Sample <vox_NUM_CELLS, AUDIO_RATE> samp[NUM_SAMP];
float sampFreq[NUM_SAMP];

Sample <vox_NUM_CELLS, AUDIO_RATE> sampR[NUM_SAMP];
float sampRFreq[NUM_SAMP];
int sampRGain[NUM_SAMP];
unsigned long sampREndTime[NUM_SAMP];
int panVal[NUM_SAMP] = {0, 1, 0, 1, 0, 1}; // L R L R L R
int rhythmSampIndex = 0;
int quarterCounter = 2;
int rhythmCounter = 0;
int rSteps[] = {0, 0, 0, 0, 2, 4, 7, 7, 7, 9, 12, 12, 12};


float voxFreq = 0;

EventDelay sTimer(CONTROL_RATE);
int sTime = 140;

EventDelay envTimer[NUM_SAMP];
int noteLen = 50;
int myGain[NUM_SAMP];
int mute[NUM_SAMP];
int pitchOffset[NUM_SAMP];

int masterGains[NUM_SAMP];

int majScale[] = {0, 2, 4, 5, 7, 9, 11}; // template for building major scale on top of 12TET
int scaleRoot = 48; // root of major scale

int seventh[] = {0, 2, 4, 6}; // template for triad on top of major scale
int chordRoot = 0; // root of triad

//PATTERNS
class Sequence {       // The class
  public:             // Access specifier
    int notes[50];
    int times[50];
    int lengths[50];
    int index = 0;
    int length = 0;
    int beatCounter = 0;

    float currSampleMul = 0;
    int currMidiOffset = 0;
    int currNoteLength = 0;
    bool ready = false;

    void update(int offset){
      beatCounter++;
      if (beatCounter >= times[index]){
        beatCounter = 0;
        index++;
        if(index >= length){
          index = 0;
        }
        //set freq
        currMidiOffset = getMidiOfStep(index);
        currSampleMul = getSampleOffOfStepOffset(index, offset);
        currNoteLength = getLengthOfStep(index);
        ready = true;
      }
    }

  float scaleDegtoSemi(int scaleDeg)
  {
        int scaleOct = 0;
        while(scaleDeg < 0){ // bring up to octave
          scaleDeg += 7;
          scaleOct -= 1;
        }
        while(scaleDeg > 6){ // bring down to octave
          scaleDeg -= 7;
          scaleOct += 1;
        }
        float mySemi = majScale[scaleDeg] + 12 * scaleOct;
        return mySemi;
  }

  int getMidiOfStep(int stepNum){
    return scaleDegtoSemi(notes[stepNum]);
  }

  float getSampleOffOfStep(int stepNum){
    return pow(2, (float)getMidiOfStep(stepNum) / 12.0);
  }

  float getSampleOffOfStepOffset(int stepNum, int offset){
    return pow(2, (float)(scaleDegtoSemi(notes[stepNum] + offset)) / 12.0);
  }

  int getLengthOfStep(int stepNum){
    return lengths[stepNum] * sTime;
  }
};

Sequence seq[NUM_SAMP];


int glitchFactor = 0;


//intensity ramp stuff
unsigned long iRampEnd = 0;
unsigned long iRampStart = 0;
bool iRampActive = false;

// ---------- SAMPLE KBD VARIABLES --------------

Sample <96359, AUDIO_RATE> sampPad[NUM_SAMP_PADS];
float sampPadFreq[NUM_SAMP_PADS];
float sampPadFreqDef;

ADSR <CONTROL_RATE, CONTROL_RATE> sampPadEnv[NUM_SAMP_PADS];
int sampPadGain[NUM_SAMP_PADS];

int entropy = 0;

int glitchOnOff[NUM_SAMP_PADS];
float glitchOct[NUM_SAMP_PADS];

int glitchLength = 35;
EventDelay glitchTimer;


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);


void setup(){
  Serial.begin(115200); // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE); // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1); // sets up touch pads
  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function

  //setup all the voice objects
  voxFreq = (float)vox_SAMPLERATE / (float) vox_NUM_CELLS;
  for (int i = 0; i < NUM_SAMP; i++){
    samp[i].setTable(vox_DATA);
    sampFreq[i] = (float)(voxFreq * pow(2, (float)(2*i)/12.0));
    samp[i].setFreq(sampFreq[i]);
    sampR[i].setTable(vox_DATA);
    sampR[i].setLoopingOn();
    sampRFreq[i] = (float)(voxFreq);
    sampR[i].setFreq(sampRFreq[i]);
    sampRGain[i] = 0;
    myGain[i] = 0;
    masterGains[i] = 255;
    mute[i] = 0;
    pitchOffset[i] = 0;
  }

  //start other MEAP
  MIDI.sendNoteOn(126, 100, 1);
  MIDI.sendNoteOn(127, 100, 1);
  MIDI.sendNoteOn(127, 100, 1);

  // create seq 0
  int m1N[] = {-5, -1, 0, 0, 2, -1}; // E3 B3 C4 C4 E4 B3 = -5 -1 0 0 2 -1
  int m1T[] = {12, 4, 22, 9, 1, 12}; // 0 12 16 38 47 48
  int m1L[] = {12, 4, 18, 9, 1, 12}; // 12 4 18 9 1 12
  int l1 = sizeof(m1N)/sizeof(int);
  seq[0].length = l1;
    for (int i = 0; i < l1; i++){
      seq[0].notes[i] = m1N[i];
      seq[0].times[i] = m1T[i];
      seq[0].lengths[i] = m1L[i];
    }
  //start seq 0
  myGain[0] = 1;
  envTimer[0].start(seq[0].getLengthOfStep(0));
  sampFreq[0] = voxFreq * seq[0].getSampleOffOfStep(0);
  samp[0].setFreq(sampFreq[0]*2);
  // masterGains[0] = 200;

  // create seq 1
  int m2N[] = {-3, -3, -3, -1, -3};
  int m2T[] = {4, 4, 4, 2, 2};
  int m2L[] = {2, 2, 2, 2, 2};
  int l2 = sizeof(m2N)/sizeof(int);
  seq[1].length = l2;
    for (int i = 0; i < l2; i++){
      seq[1].notes[i] = m2N[i];
      seq[1].times[i] = m2T[i];
      seq[1].lengths[i] = m2L[i];
    }
  //start seq 1
  myGain[1] = 1;
  envTimer[1].start(seq[1].getLengthOfStep(0));
  sampFreq[1] = voxFreq * seq[1].getSampleOffOfStep(0);
  samp[1].setFreq(sampFreq[1]);

  // create seq 2
  int m3N[] = {-5, -7, -5, -7, -5, -7, -5, -6, -7, -6, -7, -6, -7, -5};
  int m3T[] = {2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};
  int m3L[] = {2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};
  int l3 = sizeof(m3N)/sizeof(int);
  seq[2].length = l3;
    for (int i = 0; i < l3; i++){
      seq[2].notes[i] = m3N[i];
      seq[2].times[i] = m3T[i];
      seq[2].lengths[i] = m3L[i];
    }
  //start seq 2
  myGain[2] = 1;
  envTimer[2].start(seq[2].getLengthOfStep(0));
  sampFreq[2] = voxFreq * seq[2].getSampleOffOfStep(0);
  samp[2].setFreq(sampFreq[2]);

  // create seq 3
  int m4N[] = {11, 9, 7};
  int m4T[] = {1, 1, 1};
  int m4L[] = {1, 1, 1};
  int l4 = sizeof(m4N)/sizeof(int);
  seq[3].length = l4;
    for (int i = 0; i < l4; i++){
      seq[3].notes[i] = m4N[i];
      seq[3].times[i] = m4T[i];
      seq[3].lengths[i] = m4L[i];
    }
  //start seq 3
  myGain[3] = 1;
  envTimer[3].start(seq[3].getLengthOfStep(0));
  sampFreq[3] = voxFreq * seq[3].getSampleOffOfStep(0);
  samp[3].setFreq(sampFreq[3]);
  // masterGains[3] = 100;

  // create seq 4
  int m5N[] = {4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3};
  int m5T[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  int m5L[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int l5 = sizeof(m5N)/sizeof(int);
  seq[4].length = l5;
    for (int i = 0; i < l5; i++){
      seq[4].notes[i] = m5N[i];
      seq[4].times[i] = m5T[i];
      seq[4].lengths[i] = m5L[i];
    }
  //start seq 4
  myGain[4] = 1;
  envTimer[4].start(seq[4].getLengthOfStep(0));
  sampFreq[4] = voxFreq * seq[4].getSampleOffOfStep(0);
  samp[4].setFreq(sampFreq[4]);

  // create seq 5
  int m6N[] = {7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1};
  int m6T[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  int m6L[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int l6 = sizeof(m6N)/sizeof(int);
  seq[5].length = l6;
    for (int i = 0; i < l6; i++){
      seq[5].notes[i] = m6N[i];
      seq[5].times[i] = m6T[i];
      seq[5].lengths[i] = m6L[i];
    }
  //start seq 5
  myGain[5] = 1;
  envTimer[5].start(seq[5].getLengthOfStep(0));
  sampFreq[5] = voxFreq * seq[5].getSampleOffOfStep(0);
  samp[5].setFreq(sampFreq[5]);

  // sTimer.start(sTime);
  sTimer.start(0);

  sampleKbdSetup();
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switch65es
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers
  sampleKbdLoop();

  if(sTimer.ready()){
    sTimer.start(sTime);
    MIDI.sendNoteOn(127, 100, 1);

    // ------------ RHYTHMIC VOCAL BEGIN -------------
    quarterCounter += 1;
    unsigned long myCurrTime = millis();
    if(quarterCounter >= 1){
      quarterCounter = 0;
      if(iRampActive){
        // if(myCurrTime > (iRampEnd + 10000)){
        //   iRampActive = false;
        // }
        //interpolating the ramp
        int density;
        if (myCurrTime < iRampEnd){
          density = map(myCurrTime, iRampStart, iRampEnd, 0, 90);
        } else{
          density = 90;
        }
        debugln(density);
        int randNum = irand(0, 90);
        if(density < 70 && density > 30){
          randNum = max(randNum, (int)irand(0, 90));
        }
        if(randNum < density){
          sampRGain[rhythmSampIndex] = 255;
          sampR[rhythmSampIndex].start(1000);
          float fMul = pow(2, (float)rSteps[irand(0, 12)] / 12.0);
          sampRFreq[rhythmSampIndex] = voxFreq * fMul;
          sampR[rhythmSampIndex].setFreq(sampRFreq[rhythmSampIndex]);
          if(density >= 90){
            int myEnd = myCurrTime + irand(5, 800);
            myEnd = min(myEnd, (int)(myCurrTime + irand(5, 800)));
            myEnd = min(myEnd, (int)(myCurrTime + irand(5, 800)));
            myEnd = min(myEnd, (int)(myCurrTime + irand(5, 800)));
            sampREndTime[rhythmSampIndex++] = myEnd;

          } else {
            sampREndTime[rhythmSampIndex++] = myCurrTime + irand(map(density, 0, 90, 2000, 5), map(density, 0, 90, 5000, 100));
          }
          if(rhythmSampIndex >= NUM_SAMP){
            rhythmSampIndex = 0;
          }
        }
      }
    }

    for (int i = 0; i < NUM_SAMP; i++){
      if(myCurrTime > sampREndTime[i]){
        sampRGain[i] = 0;
      }
    }
    // ------------ RHYTHMIC VOCAL END -------------

    // ------------ VOCAL SEQUENCES BEGIN -------------
    for (int i = 0; i < NUM_SAMP; i++){
      pitchOffset[i] = !dipVals[i] * 4;
    }
    //seq #1
    seq[0].update(pitchOffset[0]);
    if(seq[0].ready){
      seq[0].ready = false;
      sampFreq[0] = (float)(voxFreq * seq[0].currSampleMul);
      samp[0].setFreq(sampFreq[0]*2);
      samp[0].start();
      envTimer[0].start(seq[0].currNoteLength);
      myGain[0] = 1;
    }
    //seq #2
    seq[1].update(pitchOffset[1]);
    if(seq[1].ready){
      seq[1].ready = false;
      sampFreq[1] = (float)(voxFreq * seq[1].currSampleMul);
      samp[1].setFreq(sampFreq[1]);
      samp[1].start();
      envTimer[1].start(seq[1].currNoteLength);
      myGain[1] = 1;
    }

    //seq #3
    seq[2].update(pitchOffset[2]);
    if(seq[2].ready){
      seq[2].ready = false;
      sampFreq[2] = (float)(voxFreq * seq[2].currSampleMul);
      samp[2].setFreq(sampFreq[2]);
      samp[2].start();
      envTimer[2].start(seq[2].currNoteLength);
      myGain[2] = 1;
    }

    //seq #4
    seq[3].update(pitchOffset[3]);
    if(seq[3].ready){
      seq[3].ready = false;
      sampFreq[3] = (float)(voxFreq * seq[3].currSampleMul);
      samp[3].setFreq(sampFreq[3]);
      samp[3].start();
      envTimer[3].start(seq[3].currNoteLength);
      myGain[3] = 1;
    }

      //seq #5
    seq[4].update(pitchOffset[4]);
    if(seq[4].ready){
      seq[4].ready = false;
      sampFreq[4] = (float)(voxFreq * seq[4].currSampleMul);
      samp[4].setFreq(sampFreq[4]);
      samp[4].start(irand(0, 45000));
      envTimer[4].start(seq[4].currNoteLength);
      myGain[4] = 1;
    }

    //seq #6
    seq[5].update(pitchOffset[5]);
    if(seq[5].ready){
      seq[5].ready = false;
      sampFreq[5] = (float)(voxFreq * seq[5].currSampleMul);
      samp[5].setFreq(sampFreq[5]);
      samp[5].start(irand(0, 45000));
      envTimer[5].start(seq[5].currNoteLength);
      myGain[5] = 1;
    }

  // ------------ VOCAL SEQUENCES END -------------

  // ------------ VOCAL SEQUENCES GLITCHING BEGIN-------------
  glitchFactor = map(potVals[0], 0, 4095, 0, 100);
  int maxLen = map(potVals[1], 0, 4095, 10, 2000);

  for (int i = 0; i < NUM_SAMP; i++){
    int randNum = irand(0, 100);
    if(randNum < glitchFactor){
      //perform  glitch
      randNum = randNum%3;
      switch(randNum){
        case 0: //octave up
          samp[i].setFreq(sampFreq[i] * 2);
          break;

        case 1: //2 octave up
          samp[i].setFreq(sampFreq[i] * 4);
          break;

        case 2: //on/off
          envTimer[i].start(irand(0, maxLen));
          myGain[i] = 1;
          samp[i].start(irand(0, 50000));
          break;
      }
    }
  }
  }
  // ------------ VOCAL SEQUENCES GLITCHING END-------------


  // ------------ HANDLE ENVELOPES BEGIN -------------
  for (int i = 0; i < NUM_SAMP; i++){
    if(envTimer[i].ready()){
      myGain[i] = 0;
    }
  }
  // ------------ HANDLE ENVELOPES END -------------
}


AudioOutput_t updateAudio(){
  int lSamp = 0;
  int rSamp = 0;
  int pattSamp = 0;
  for (int i = 0; i < NUM_SAMP; i++){
    pattSamp += (samp[i].next() * myGain[i] * masterGains[i] * mute[i]);
    int myImmediate = sampR[i].next() * sampRGain[i];
    lSamp += myImmediate * (1.3-panVal[i]);
    rSamp += myImmediate * (panVal[i] + 0.3);
  }

  lSamp += pattSamp >> 1;
  rSamp += pattSamp >> 1;

  int kbdSamp = 0;
  for (int i = 0; i < NUM_SAMP_PADS; i++){
    kbdSamp += sampPad[i].next() * sampPadGain[i] * glitchOnOff[i];
  }

  lSamp += kbdSamp >> 3;
  rSamp += kbdSamp >> 3;

  return StereoOutput::fromAlmostNBit(19, lSamp, rSamp);
}

 
void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
    mux.read(i);
    dipVals[dipPins[i]] = !mux.read(i);
  }

  //Check if any switches changed position
  for (int i = 0; i < 8; i++){
    switch(i){
      case 0:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 1 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(bell_DATA);
              sampPad[i].setEnd(bell_NUM_CELLS);
            }
          } else {
            debugln("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 2 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(bowglo_DATA);
              sampPad[i].setEnd(bowglo_NUM_CELLS);
            }   
          } else {
            debugln("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 3 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(clari_DATA);
              sampPad[i].setEnd(clari_NUM_CELLS);
            }   
          } else {
            debugln("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 4 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(meltine_DATA);
              sampPad[i].setEnd(meltine_NUM_CELLS);
            }   
          } else {
            debugln("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 5 up");
            for (int i = 0; i < NUM_SAMP_PADS; i++){
              sampPad[i].setTable(juno_DATA);
              sampPad[i].setEnd(juno_NUM_CELLS);
            }
          } else {
            debugln("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 6 up");
          } else {
            debugln("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 7 up");
            sTime = 280;
          } else {
            debugln("DIP 7 down");
            sTime = 140;
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(dipVals[i]){
            debugln("DIP 8 up");
          } else {
            debugln("DIP 8 down");
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
    if (touchAvgs[i] > touchThreshold){
      touchVals[i] = 1; 
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]){
      if(touchVals[i]){ // ANY PAD PRESSED
        if(dipVals[7]){ //dip 8 is up, play keys
          glitchOnOff[i] = 1;
          glitchOct[i] = 1;
          sampPadEnv[i].noteOn();
          sampPad[i].start();
          sampPad[i].setFreq((float)(sampPadFreqDef * (float)pow(2, ((float)majScale[(int)i])/12.0f)));
        } else { // dip 8 is down, toggle patterns
          if(mute[i] == 1){
            mute[i] = 0;
          } else { 
            mute[i] = 1;
          }
        }

      } else { //ANY PAD RELEASED
        if(dipVals[7]){//dip 8 is up, play keys
          sampPadEnv[i].noteOff();
        }
      }
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            debugln("pad 0 pressed");
          } else { // pad 0 released
            debugln("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            debugln("pad 1 pressed");
          } else { // pad 1 released
            debugln("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            debugln("pad 2 pressed");
          } else { // pad 2 released
            debugln("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            debugln("pad 3 pressed");
          } else { // pad 3 released
            debugln("pad 3 released");
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            debugln("pad 4 pressed");
          } else { // pad 4 released
            debugln("pad 4 released");
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            debugln("pad 5 pressed");
          } else { // pad 5 released
            debugln("pad 5 released");
          } 
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed starts intensity ramp for 10x as long as pressed
            debugln("pad 6 pressed");
            if(!dipVals[7]){ //dip 8 down, start intensity ramp
              iRampStart = millis();
            }
          } else { // pad 6 released
            debugln("pad 6 released");
            if(!dipVals[7]){ //dip 8 down, start intensity ramp
              unsigned long eep = millis();
              iRampEnd = eep + (eep-iRampStart) * 10;
              iRampActive = true;
              debug(iRampStart);
              debug(" ");
              debug(iRampEnd);
              debug(" ");
              debugln(iRampEnd-iRampStart);
            }
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            debugln("pad 7 pressed");
            if(!dipVals[7]){ //dip 8 down, stop vocal chopping
              iRampActive = false;
            }
          } else { // pad 7 release  d
            debugln("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
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

void sampleKbdSetup(){
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

void sampleKbdLoop(){

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