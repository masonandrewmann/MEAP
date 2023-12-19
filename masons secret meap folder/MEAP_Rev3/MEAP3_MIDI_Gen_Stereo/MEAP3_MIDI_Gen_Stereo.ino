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
#include <MIDI.h>
#include <tables/sin8192_int8.h>
#include <Ead.h>
#include <ADSR.h>
#include <EventDelay.h>
#include <ArduinoQueue.h>


#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

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
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {2, 4, 6, 8, 1, 3, 5, 7};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 500;

// variables for potentiometers
int potVals[] = {0, 0};

Oscil <8192, AUDIO_RATE> osc1(SIN8192_DATA);;
Oscil <8192, AUDIO_RATE> osc2(SIN8192_DATA);;

EventDelay myDel;
int delTime = 12;

ADSR <AUDIO_RATE, AUDIO_RATE> env1a;

int gain1;

ADSR <AUDIO_RATE, AUDIO_RATE> env2a;
int gain2;

int numWaits1 = 0;
int numWaits2 = 0;

int majScale[] = {0, 2, 4, 5, 7, 9, 11}; // template for building major scale on top of 12TET
int scaleRoot = 48; // root of major scale

int seventh[] = {0, 2, 4, 6}; // template for triad on top of major scale
int chordRoot = 0; // root of triad

int octaveOffset = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

struct midiNote{
  int noteNum;
  int channel;
  unsigned long endTime;
};

ArduinoQueue<struct midiNote*> myNoteQueue(500);

int osc1Pan = 0;
int osc2Pan = 0;

void setup(){
  Serial.begin(115200); // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE); // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1); // sets up touch pads

  osc1.setFreq(mtof(scaleRoot + majScale[(seventh[0] + chordRoot)%7]));
  osc2.setFreq(mtof(scaleRoot + majScale[(seventh[1] + chordRoot)%7]));

  env1a.setADLevels(255,0);
  env2a.setADLevels(255,0);
  env1a.setTimes(10,100,0,0);
  env2a.setTimes(10,100,0,0);

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

  delTime = map(potVals[0], 0, 4095, 300, 2);
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
      int myNoteNum = irand(0, 7);
      //calculate mapping values
      int envVal = map(myNoteNum, 0, 7, 1000, 50);
      //apply envelope
      env1a.setTimes(10, envVal, 0, 0);
      //apply frequency
      int myOctave = 0;
      while(myNoteNum > 3){
        myNoteNum -= 4;
        myOctave++;
      }
      myNoteNum = 12 + scaleRoot + majScale[(seventh[myNoteNum] + chordRoot)%7] + 12 * myOctave + 12 * octaveOffset;
      osc1.setFreq(mtof(myNoteNum));
      env1a.noteOn();
      osc1Pan = irand(0, 1);

      createMIDINote(constrainMIDI(myNoteNum), 100, 1, envVal);
    }

    if (numWaits2 <= 0){
      numWaits2 = irand(8, 16);
      if (delTime < 4){
        numWaits2 = 1;
      }
      if(touchVals[6]){
        numWaits2 = 6;
      } else if (touchVals[7]){
        numWaits2 = 12;
      }
      //generate random note
      int myNoteNum = irand(0, 23); // switch max to 23
      //calculate mapping values
      int envVal = map(myNoteNum, 0, 23, 3500, 500);
      //apply envelope
      env2a.setTimes(10, envVal, 0, 0);
      //apply frequency
      int myOctave = 0;
      while(myNoteNum > 3){
        myNoteNum -= 4;
        myOctave++;
      }
      myNoteNum = scaleRoot + majScale[(seventh[myNoteNum] + chordRoot)%7] + 12 * myOctave - 12 + 12 * octaveOffset;
      osc2.setFreq(mtof(myNoteNum));
      env2a.noteOn();
      osc2Pan = irand(0, 1);

      createMIDINote(constrainMIDI(myNoteNum), 100, 2, envVal);
    }
  }

  if(!myNoteQueue.isEmpty()){
    struct midiNote* myNote = myNoteQueue.getHead();
    if(millis() > myNote->endTime){
      MIDI.sendNoteOff(myNote->noteNum, 100, myNote->channel);
      free(myNote);

      myNoteQueue.dequeue();
    }
  }
}


AudioOutput_t updateAudio(){
  // return MonoOutput::fromAlmostNBit(17, osc1.next() * gain1 + osc2.next() * gain2);
  int l_samp = 0;
  int r_samp = 0;
  env1a.update();
  env2a.update();

  if(osc1Pan == 0){
    l_samp += osc1.next() * env1a.next();
  } else{
    r_samp += osc1.next() * env1a.next();
  }

    if(osc2Pan == 0){
    l_samp += osc2.next() * env2a.next();
  } else{
    r_samp += osc2.next() * env2a.next();
  }
  return StereoOutput::fromAlmostNBit(17, l_samp, r_samp);
}

void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++){
    mux.read(i); //read once and throw away result (for reliability)
    dipVals[dipPins[i]] = mux.read(i);
  }

  //Check if any switches changed position
  for (int i = 0; i < 8; i++){
    switch(i){
      case 0:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 1 up");
          } else {
            debugln("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 2 up");
          } else {
            debugln("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 3 up");
          } else {
            debugln("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 4 up");
          } else {
            debugln("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 5 up");
          } else {
            debugln("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 6 up");
          } else {
            debugln("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            debugln("DIP 7 up");
          } else {
            debugln("DIP 7 down");
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
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
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            debugln("pad 0 pressed");
            // chordRoot = 0;
            MIDI.sendProgramChange(12, 1); //piano
            MIDI.sendProgramChange(1, 2); //marimba
          } else { // pad 0 released
            debugln("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            debugln("pad 1 pressed");
            MIDI.sendProgramChange(92, 1); //bowed pad
            MIDI.sendProgramChange(89, 2); //warmpad
            // chordRoot = 5;
          } else { // pad 1 released
            debugln("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            debugln("pad 2 pressed");
            // chordRoot = 3;
            MIDI.sendProgramChange(52, 1); //ooh
            MIDI.sendProgramChange(53, 2); //ahh
          } else { // pad 2 released
            debugln("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            debugln("pad 3 pressed");
            // chordRoot = 4;
            MIDI.sendProgramChange(5, 1); //ooh
            MIDI.sendProgramChange(89, 2); // square
            // MIDI.sendControlChange(, , 2);//cutoff -29
            // MIDI.sendControlChange(73, 16, 2);//atk 16
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
          if(touchVals[i]){ // pad 6 pressed
            debugln("pad 6 pressed");
          } else { // pad 6 released
            debugln("pad 6 released");
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            debugln("pad 7 pressed");
          } else { // pad 7 released
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

void createMIDINote(int noteNum, int velocity, int channel, int length){
  struct midiNote* myMidiNote = (struct midiNote*)malloc(sizeof(struct midiNote));
  myMidiNote->noteNum = noteNum;
  myMidiNote->channel = channel;
  myMidiNote->endTime = millis() + length;
  myNoteQueue.enqueue(myMidiNote);
  MIDI.sendNoteOn(noteNum, velocity, channel);
}

int constrainMIDI(int noteNum){
  while(noteNum > 127){
    noteNum -= 12;
  }
  while(noteNum < 0){
    noteNum += 12;
  }
  return noteNum;
}