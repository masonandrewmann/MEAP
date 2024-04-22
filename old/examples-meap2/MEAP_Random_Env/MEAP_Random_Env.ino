/*
  Example that plays random notes with random envelopes
  
  Potentiometer 1 controls the maximum length of the envelopes release phase
  Potentiometer 2 controls the maximum length of the envelopes release phase

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <Mux.h>
#include <tables/sin8192_int8.h> // loads sine wavetable

#define CONTROL_RATE 128 // faster than usual to help smooth CONTROL_RATE adsr interpolation (next())


using namespace admux;

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> aOscil(SIN8192_DATA);

// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, CONTROL_RATE> envelope;

byte gain;
unsigned int duration, attack, decay, sustain, release_ms;

// variables for potentiometers
int potVals[] = {0, 0};


void setup(){
  Serial.begin(115200);
  randSeed(); // fresh random
  startMozzi(CONTROL_RATE);
  aOscil.setFreq(440); //set frequency of sine oscillator
}


void loop(){
  audioHook();
}


void updateControl(){
    readPots(); // reads potentiometers
    Serial.println(potVals[0]);
  if(noteDelay.ready()){

      // choose envelope levels
      byte attack_level = rand(128)+127;
      byte decay_level = rand(255);
      envelope.setADLevels(attack_level,decay_level);

      // generate a random new adsr parameter value in milliseconds
      int r1 = rand((int)map(potVals[0], 0, 4095, 10, 1000));
      int r2 = rand((int)map(potVals[1], 0, 4095, 10, 1000));
//      unsigned int new_value = abs(r);

     // randomly choose one of the adsr parameters and set the new value
     switch (rand(4)){
       case 0:
       attack = r1;
       break;

       case 1:
       decay = r1;
       break;

       case 2:
       sustain = r2;
       break;

       case 3:
       release_ms = r2;
       break;
     }
     envelope.setTimes(attack,decay,sustain,release_ms);
     envelope.noteOn();

     byte midi_note = rand(107)+20;
     aOscil.setFreq((int)mtof(midi_note));

     noteDelay.start(attack+decay+sustain+release_ms);

   }
  envelope.update();
  gain = envelope.next(); // this is where it's different to an audio rate envelope
}


int updateAudio(){

  return MonoOutput::from16Bit((int) (gain * aOscil.next()));
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(36);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(39);
}
