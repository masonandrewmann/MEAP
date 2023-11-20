/*
  Example that generates baroque-style chord progressions in the key of C major

  Pot #1 Control attack time of envelope

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
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

Ead env1(CONTROL_RATE); // resolution will be CONTROL_RATE

// envelope values
int gain1 = 0;

EventDelay noteDelay;
int noteLength = 400; // number of milliseconds between notes of arpeggio

int sequence[12];
int seqInd = 0;
int seqLen = 0;
int restLen = 0;
int lastPitch = 0;

int firstNote = 1;

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  //set oscillators to pitches of first chord (iii chord on c major scale)


  noteDelay.start(noteLength); //start first note timer

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));

  for (int i = 0; i < 12; i++){
    sequence[i] = 0;
  }
  env1.setAttack(10);
  env1.setDecay(300);
  
  fillSequence();
  osc1.setFreq(mtof(sequence[seqInd]));
  env1.start();
}


void loop(){
  audioHook();
}


void updateControl(){
  readPots(); // reads potentiometers

  if (noteDelay.ready()){ // ready to move to next note
    noteDelay.start(noteLength);
    seqInd++;
    if(seqInd < seqLen){
      osc1.setFreq(mtof(sequence[seqInd]));
      env1.start();
    } else if (seqInd < (seqLen + restLen)){
      //rest
    } else{
      seqInd = 0;
      fillSequence();
      osc1.setFreq(mtof(sequence[seqInd]));
      env1.start();
      
    }
  }

  gain1 = env1.next();
}


int updateAudio(){
  return MonoOutput::fromAlmostNBit(16, gain1 * osc1.next());
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}

void fillSequence(){
  seqLen = irand(11, 12);
  int candidate = 0; // candidate note
  int interval = 0; // interval from last note
  for (int i = 0; i < seqLen; i++){
    if(firstNote){ // first note has no restrictions
      candidate = irand(72, 91);
      firstNote = 0;
    } else if (i == 0){ // first note of each sequence can be a P5 up from last note of prev seq
      do{
        candidate = irand(72, 91);
        interval = candidate - lastPitch;
      } while (interval > 7 || interval < 0); // repeat so long as conditions not met
    } else if (i == 1){ // second note jumps down by up to a P5
      do{
        candidate = irand(72, 91);
        interval = candidate - lastPitch;
      } while (interval > 0 || interval < -7); // repeat so long as conditions not met
    } else { // remaining notes within minor third up or down
      do{
        candidate = irand(72, 91);
        interval = candidate - lastPitch;
      } while (interval > 3 || interval < -3); // repeat so long as conditions not met
    }
    lastPitch = candidate;
    sequence[i] = candidate;
  }
  restLen = irand(3, 4);
}

long irand(long howsmall, long howbig){
  howbig++;
  if (howsmall >= howbig){
    return howsmall;
  }
  long diff = howbig - howsmall;
  return (xorshift96() % diff) + howsmall;
}
