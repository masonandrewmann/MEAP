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
#include <EventDelay.h>
#include <Mux.h>
#include <tables/triangle_warm8192_int8.h>
#include <EventDelay.h>
#include <Ead.h>

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

Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc1(TRIANGLE_WARM8192_DATA);

Ead env1(CONTROL_RATE); // resolution will be CONTROL_RATE
int gain1 = 0;

// for triggering the envelope
EventDelay noteDelay;
int noteLength = 400;

// transition table

int transitionNotes[] = {60, 62, 64, 65, 67, 69, 70, 72};

float transitionWeights[8][8] = 
{
  {3, 2, 0, 1, 1, 0, 0, 1}, //from C 
  {2, 0, 0, 0, 0, 0, 1, 0}, //from D
  {2, 0, 0, 0, 0, 0, 1, 0}, //from E 
  {1, 0, 2, 0, 1, 0, 0, 0}, //from f 
  {0, 0, 0, 2, 0, 0, 0, 0}, //from G 
  {0, 0, 0, 2, 0, 0, 0, 0}, //from A 
  {0, 0, 0, 0, 0, 1, 1, 0}, //from Bb 
  {0, 0, 0, 0, 1, 0, 0, 0}, //from C^ 
};

float transitionWeightsCum[8][8] = 
{
  {0, 0, 0, 0, 0, 0, 0, 0}, //from C 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from D
  {0, 0, 0, 0, 0, 0, 0, 0}, //from E 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from f 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from G 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from A 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from Bb 
  {0, 0, 0, 0, 0, 0, 0, 0}, //from C^ 
};

int currNote = 0;

// variables for potentiometers
int potVals[] = {0, 0};

void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi();
  
  osc1.setFreq(mtof(transitionNotes[currNote])); //set frequency of sine oscillator

  env1.setAttack(10);
  env1.setDecay(300);

  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sums[] = {0, 0, 0, 0, 0, 0, 0, 0};

  // CALCULATING SUMS
  for (int row = 0; row < 8; row++){ 
    for (int col = 0; col < 8; col++){
      sums[row] += transitionWeights[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < 8; row++){
    for (int col = 0; col < 8; col++){
      transitionWeights[row][col] /= sums[row];
    }
  }

   // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < 8; row++){
    for (int col = 0; col < 8; col++){ // compute cumulative probability density function
      for(int i = 0; i <= col; i++){
        transitionWeightsCum[row][col] += transitionWeights[row][i];
      }
    }
  }
  noteDelay.start(noteLength);

  for (int row = 0; row < 8; row++){
    for (int col = 0; col < 8; col++){
      Serial.print(transitionWeightsCum[row][col]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
}


void loop(){
  audioHook();
}


void updateControl(){
  if (noteDelay.ready()){
    noteDelay.start(noteLength);

    float R = frand();
    for(int i = 0; i < 8; i++){
      if(transitionWeightsCum[currNote][i] >= R){
        currNote = i;
        break;
      }
    }
    osc1.setFreq(mtof(transitionNotes[currNote]));
    env1.start();
    Serial.println(currNote);
  }

  gain1 = env1.next();
}


int updateAudio(){
  return MonoOutput::from16Bit(osc1.next() * gain1);
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
