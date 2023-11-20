/*
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

int myNotes[] = {72, 76, 79, 74, 77, 81}; //C4, E4, G4, D4, F4, A4
float myWeights[] = {2, 2, 2, 1, 1, 1};
float myWeightsCum[] = {0, 0, 0, 0, 0, 0};

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi(CONTROL_RATE);

  //set oscillators to pitches of first chord (iii chord on c major scale)


  noteDelay.start(noteLength); //start first note timer

  randomSeed(10); // initializes random number generator
  xorshiftSeed((long)random(1000));

  env1.setAttack(10);
  env1.setDecay(300);
  
  osc1.setFreq(mtof(72)); // start on C4
  env1.start();

  
  //SETTING UP PROBABILITY DENSITY FUNCTION
  int sum = 0;
  for (int i = 0; i < 6; i++){ // determine sum of all probabilities
      sum += myWeights[i];
  }
  
  for (int i = 0; i < 6; i++){ // normalize so probabilities all add up to one
      myWeights[i] /= sum;
  }
  
  for (int i = 0; i < 6; i++){ // compute cumulative probability density function
    for(int j = 0; j <= i; j++){
      myWeightsCum[i] += myWeights[j];
    }
  }
}


void loop(){
  audioHook();
}


void updateControl(){
  readPots(); // reads potentiometers

  if (noteDelay.ready()){ // ready to move to next note
    noteDelay.start(noteLength);

    float R = frand();
    int myNote = 0;
    for (int i = 0; i < 6; i++){
      if(myWeightsCum[i] >= R){
        myNote = myNotes[i];
        break;
      }
    }
    osc1.setFreq(mtof(myNote));
    env1.start();
    Serial.println(myNote);
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
