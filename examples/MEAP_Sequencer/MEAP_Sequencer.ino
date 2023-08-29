/*
  Example plays through sequence on loop. Sequence is 
  defined using variables mySeqNotes and mySeqLens

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <tables/sin8192_int8.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

// variables for playing sequence
uint8_t mySeqNotes[] = {88, 86, 78, 80, 85, 83, 74, 76, 83, 81, 73, 76, 81}; // notes referred to by MIDI note number
int mySeqLens[] = {250, 250, 500, 500, 250, 250, 500, 500, 250, 250, 500, 500, 1500}; // length of notes in milliseconds
int seqInd = 0;
int seqLen = 13;
unsigned long seqTimer = 250;

//oscillator definitions
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> mySine(SIN8192_DATA);


void setup(){
  startMozzi();
  mySine.setFreq(mtof(myNoteSequence[0]));
}


void loop(){
  audioHook();
}


void updateControl(){
  if(millis() > seqTimer){ // check if current note needs to end
    seqInd = (seqInd + 1) % seqLen; // move to next step of sequence
    mySine.setFreq(mtof(mySeqNotes[seqInd])); // set oscillator frequeency to current pitch
    seqTimer = seqTimer + mySeqLens[seqInd]; // set ending time of current note
  }
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(mySine.next());
}
