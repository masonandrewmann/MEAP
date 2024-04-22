/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <Meap.h> // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/sin8192_int8.h>
#include <ArduinoQueue.h>

#define CONTROL_RATE 128   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports


#define DEBUG 1 // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

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

struct midiNote{
  int noteNum;
  int channel;
  unsigned long endTime;
};

ArduinoQueue<struct midiNote*> myNoteQueue(500);

int osc1Pan = 0;
int osc2Pan = 0;

void setup(){
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

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
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  delTime = map(meap.pot_vals[0], 0, 4095, 300, 2);
  octaveOffset = map(meap.pot_vals[1], 0, 4095, -3, 3);

  if(myDel.ready()){
    numWaits1--;
    numWaits2--;

    myDel.start(delTime);
    if (numWaits1 <= 0){
      numWaits1 = meap.irand(2, 8);
      if(meap.touch_vals[4]){
        numWaits1 = 1;
      } else if (meap.touch_vals[5]){
        numWaits1 = 4;
      }
      //generate random note
      int myNoteNum = meap.irand(0, 7);
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
      osc1Pan = meap.irand(0, 1);

      createMIDINote(constrainMIDI(myNoteNum), 100, 1, envVal);
    }

    if (numWaits2 <= 0){
      numWaits2 = meap.irand(8, 16);
      if (delTime < 4){
        numWaits2 = 1;
      }
      if(meap.touch_vals[6]){
        numWaits2 = 6;
      } else if (meap.touch_vals[7]){
        numWaits2 = 12;
      }
      //generate random note
      int myNoteNum = meap.irand(0, 23); // switch max to 23
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
      osc2Pan = meap.irand(0, 1);

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


/** User defined function called whenever a touch pad is pressed or released
  @param number is the number of the pad that was pressed or released: 0-7
  @param pressed is true if the pad was pressed and false if the pad was released
	*/
void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released
  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
            MIDI.sendProgramChange(12, 1); //piano
            MIDI.sendProgramChange(1, 2); //marimba
      } else {        // Pad 1 released
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
            MIDI.sendProgramChange(92, 1); //bowed pad
            MIDI.sendProgramChange(89, 2); //warmpad
      } else {        // Pad 2 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
            MIDI.sendProgramChange(52, 1); //ooh
            MIDI.sendProgramChange(53, 2); //ahh
      } else {        // Pad 3 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
            MIDI.sendProgramChange(5, 1); //ooh
            MIDI.sendProgramChange(89, 2); // square
      } else {        // Pad 4 released
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
      } else {        // Pad 5 released
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
      } else {        // Pad 6 released
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
      } else {        // Pad 7 released
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
      } else {        // Pad 8 released
      }
      break;
  }
}

/** User defined function called whenever a DIP switch is toggled up or down
  @param number is the number of the switch that was toggled up or down: 0-7
  @param up is true if the switch was toggled up, and false if the switch was toggled down
	*/
void Meap::updateDip(int number, bool up) {
  if (up) {  // Any DIP up

  } else {  //Any DIP down
  }
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up

      } else {   // DIP 1 down
      }
      break;
    case 1:
      if (up) {  // DIP 2 up

      } else {   // DIP 2 down
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
      } else {   // DIP 3 down
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
      } else {   // DIP 4 down
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
      } else {   // DIP 5 down
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
      } else {   // DIP 6 down
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
      } else {   // DIP 7 down
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
      } else {   // DIP 8 down
      }
      break;
  }
}
