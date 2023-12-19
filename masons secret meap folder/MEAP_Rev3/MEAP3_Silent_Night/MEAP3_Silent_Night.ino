/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0

  //voice 3 = ChoirPad 92
      //atk=40
      //rel=63
  //voice 4 = TnklBell 113
  //voice 5 = cello 43
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
#include <ArduinoQueue.h>
#include <MIDI.h>

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#define NOTE_MARK_SIZE 12
#define RHYTHM_MARK_SIZE 5

#define SILVER_NOTE_SIZE 9
#define SILVER_RHYTHM_SIZE 4

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
int touchThreshold = 1000;

// variables for potentiometers
int potVals[] = {0, 0};

// for triggering the envelope
EventDelay noteDelay;
EventDelay noteDelaySilver;
int noteLength = 280;

// transition table

int transitionNotes[] = {67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, -1};
int silverNoteVals[] = {83, 84, 85, 86, 88, 90, 91, 93, 95};

float transitionWeights[NOTE_MARK_SIZE][NOTE_MARK_SIZE] = 
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //from G3
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from A3
  {0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2}, //from B3
  {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from C4
  {0, 0, 5, 1, 0, 5, 0, 0, 0, 0, 0, 0}, //from D4 
  {0, 0, 0, 0, 6, 2, 0, 2, 0, 0, 0, 0}, //from E4
  {0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0}, //from F#4 
  {0, 0, 0, 0, 2, 0, 2, 1, 0, 1, 0, 0}, //from G4 
  {0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 1, 0}, //from A4 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //from B4
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, //from C5 
  {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0} //from rest 
};

float transitionWeightsCum[NOTE_MARK_SIZE][NOTE_MARK_SIZE] = 
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from G3
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from A3
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from B3
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from C4
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from D4 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from E4
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from F#4 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from G4 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from A4 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from B4
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //from C5 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} //from rest 
};

float rhythmWeights[RHYTHM_MARK_SIZE][RHYTHM_MARK_SIZE] = {
  {2, 3, 2, 0, 1},
  {1, 0, 4, 0, 0},
  {3, 2, 2, 1, 0},
  {1, 0, 0, 0, 0},
  {1, 0, 0, 0, 0}
};

float rhythmWeightsCum[RHYTHM_MARK_SIZE][RHYTHM_MARK_SIZE] = {
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0}
};

float silverNotes[SILVER_NOTE_SIZE][SILVER_NOTE_SIZE] = {
  {0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 1, 0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 2, 2, 0, 0, 1, 0},
  {0, 0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 3, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 2, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 0}
};

float silverNotesCum[SILVER_NOTE_SIZE][SILVER_NOTE_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0}
};

float silverRhythms[SILVER_RHYTHM_SIZE][SILVER_RHYTHM_SIZE] = {
  {1, 1, 0, 0},
  {0, 1, 1, 1},
  {0, 1, 0, 0},
  {1, 0, 0, 0}
};

float silverRhythmsCum[SILVER_RHYTHM_SIZE][SILVER_RHYTHM_SIZE] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

int currNote = 4;
int currRhythm = 0;

int currNoteSilver = 3;
int currRhythmSilver = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

struct midiNote{
  int noteNum;
  int channel;
  unsigned long endTime;
};

ArduinoQueue<struct midiNote*> myNoteQueue(200);

int rhythmQueue[10] = {0}; //up to 10 notes in queue
int rhythmIndex = 0;
int rhythmLength = 3;

int rhythmQueueSilver[10] = {0}; //up to 10 notes in queue
int rhythmIndexSilver = 0;
int rhythmLengthSilver = 3;

int chordCounter = 0;
int chordLength = 10000;

EventDelay metro;
int metroCounter = 0;

unsigned long transitionStart = 30000;
unsigned long transitionEnd = 90000;
unsigned long fadeStart = 120000;
unsigned long fadeEnd = 150000;
unsigned long currTime = 0;


void setup(){
  Serial.begin(115200);
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);
  touchSetCycles(1, 1); // sets up touch pads
  randomSeed(10); // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000)); // uses arduino random number generator to seed the Mozzi random function

  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sums[NOTE_MARK_SIZE] = {0};

  // CALCULATING SUMS
  for (int row = 0; row < NOTE_MARK_SIZE; row++){ 
    for (int col = 0; col < NOTE_MARK_SIZE; col++){
      sums[row] += transitionWeights[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < NOTE_MARK_SIZE; row++){
    for (int col = 0; col < NOTE_MARK_SIZE; col++){
      transitionWeights[row][col] /= sums[row];
    }
  }

   // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < NOTE_MARK_SIZE; row++){
    for (int col = 0; col < NOTE_MARK_SIZE; col++){ // compute cumulative probability density function
      for(int i = 0; i <= col; i++){
        transitionWeightsCum[row][col] += transitionWeights[row][i];
      }
    }
  }

  // ------CALCULATING RHYTHM TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsR[RHYTHM_MARK_SIZE] = {0};

  // CALCULATING SUMS
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++){ 
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++){
      sumsR[row] += rhythmWeights[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++){
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++){
      rhythmWeights[row][col] /= sumsR[row];
    }
  }

   // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++){
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++){ // compute cumulative probability density function
      for(int i = 0; i <= col; i++){
        rhythmWeightsCum[row][col] += rhythmWeights[row][i];
      }
    }
  }

  // ------CALCULATING SILVER RHYTHM TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsSr[SILVER_RHYTHM_SIZE] = {0};

  // CALCULATING SUMS
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++){ 
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++){
      sumsSr[row] += silverRhythms[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++){
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++){
      silverRhythms[row][col] /= sumsSr[row];
    }
  }

   // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++){
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++){ // compute cumulative probability density function
      for(int i = 0; i <= col; i++){
        silverRhythmsCum[row][col] += silverRhythms[row][i];
      }
    }
  }

  // ------CALCULATING SILVER NOTE TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsSn[SILVER_NOTE_SIZE] = {0};

  // CALCULATING SUMS
  for (int row = 0; row < SILVER_NOTE_SIZE; row++){ 
    for (int col = 0; col < SILVER_NOTE_SIZE; col++){
      sumsSn[row] += silverNotes[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < SILVER_NOTE_SIZE; row++){
    for (int col = 0; col < SILVER_NOTE_SIZE; col++){
      silverNotes[row][col] /= sumsSn[row];
    }
  }

   // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < SILVER_NOTE_SIZE; row++){
    for (int col = 0; col < SILVER_NOTE_SIZE; col++){ // compute cumulative probability density function
      for(int i = 0; i <= col; i++){
        silverNotesCum[row][col] += silverNotes[row][i];
      }
    }
  }

//PRINT THEM ALL
  for (int row = 0; row < NOTE_MARK_SIZE; row++){
    for (int col = 0; col < NOTE_MARK_SIZE; col++){
      Serial.print(transitionWeightsCum[row][col]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  Serial.println("------");
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++){
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++){
      Serial.print(rhythmWeightsCum[row][col]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.println("------");
  for (int row = 0; row < SILVER_NOTE_SIZE; row++){
    for (int col = 0; col < SILVER_NOTE_SIZE; col++){
      Serial.print(silverNotesCum[row][col]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  Serial.println("------");
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++){
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++){
      Serial.print(silverRhythmsCum[row][col]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  //first rhythm is 0 (dotted quarter, eighth, quarter)
  rhythmQueue[0] = 3*noteLength;
  rhythmQueue[1] = noteLength;
  rhythmQueue[2] = 2*noteLength;
  rhythmIndex = 0;
  rhythmLength = 3;
  noteDelay.start(rhythmQueue[rhythmIndex]);

  //silver bells setup
  rhythmQueueSilver[0] = 2 * noteLength;
  rhythmQueueSilver[1] = 2 * noteLength;
  rhythmQueueSilver[2] = noteLength;
  rhythmQueueSilver[3] = noteLength;
  rhythmLengthSilver = 4;
  rhythmIndexSilver = 0;
  noteDelaySilver.start(rhythmQueueSilver[rhythmIndexSilver]);



  // trigger first note
  createMIDINote(transitionNotes[currNote], 100, 4, 1000);

  // createMIDINote(55, 100, 3, chordLength);
  // createMIDINote(66, 100, 3, chordLength);
  // createMIDINote(71, 100, 3, chordLength);
  // createMIDINote(74, 100, 3, chordLength);
  chordCounter = irand(5, 20);

  metro.start(noteLength);
  //voice 3 = ChoirPad 92
      //atk=40
      //rel=63
  //voice 4 = TnklBell 113
  //voice 5 = cello 43
  MIDI.sendProgramChange(91, 3);
  MIDI.sendProgramChange(112, 4);
  MIDI.sendProgramChange(42, 5);
}



void loop(){
  audioHook();
}


void updateControl(){
  currTime = millis();
  if(currTime < fadeEnd){
    if(metro.ready()){ // cello background
      metro.start(noteLength);
      metroCounter++;
      if(metroCounter >=2){
        metroCounter = 0;
      }
        switch(metroCounter){
          case 0:
            createMIDINote(43, 70, 2, chordLength);
            break;
          case 1:
            createMIDINote(50, 70, 2, chordLength);
            break;
        }
    }
  }


  if (noteDelay.ready()){ // silent night
    //move to next pitch
    float R = frand();
    for(int i = 0; i < NOTE_MARK_SIZE; i++){
      if(transitionWeightsCum[currNote][i] >= R){
        currNote = i;
        break;
      }
    }

    chordCounter--;
    if(chordCounter <= 0){
      chordCounter = irand(5, 10);
      int randNum = irand(0, 1);
      if(randNum == 0){
        // createMIDINote(55-12, 100, 3, chordLength);
        // createMIDINote(66-12, 100, 3, chordLength);
        // createMIDINote(71-12, 100, 3, chordLength);
        // createMIDINote(74-12, 100, 3, chordLength);
      } else{
        // createMIDINote(48-12, 100, 3, chordLength);
        // createMIDINote(67-12, 100, 3, chordLength);
        // createMIDINote(71-12, 100, 3, chordLength);
        // createMIDINote(76-12, 100, 3, chordLength);
      }

    }

    currTime = millis();
    if(transitionNotes[currNote] != -1){
      // trigger next note
      if(currTime < transitionStart){
        createMIDINote(transitionNotes[currNote], 90, 4, 1000);
      } else if (currTime > transitionEnd){
        //do nothing
      } else{ //mid transition
        int randNum = irand(0, 100);
        if(randNum < map(currTime, transitionStart, transitionEnd, 100, 0)){
          createMIDINote(transitionNotes[currNote], 90, 4, 1000);
        }
      }
    }

    rhythmIndex++;
    if(rhythmIndex >= rhythmLength){ //reached end of current rhythm
      rhythmIndex = 0;
      currRhythm = nextRhythm(currRhythm);
      Serial.println(currRhythm);
      switch(currRhythm){
        case 0: // dq . e . q
          rhythmQueue[0] = 3 * noteLength;
          rhythmQueue[1] = noteLength;
          rhythmQueue[2] = 2 * noteLength;
          rhythmLength = 3;
          break;
        case 1: // dh
          rhythmQueue[0] = 6 * noteLength;
          rhythmLength = 1;
          break;
        case 2: // h . q
          rhythmQueue[0] = 4 * noteLength;
          rhythmQueue[1] = 2 * noteLength;
          rhythmLength = 2;
          break;
        case 3: // q . q . q
          rhythmQueue[0] = 2 * noteLength;
          rhythmQueue[1] = 2 * noteLength;
          rhythmQueue[2] = 2 * noteLength;
          rhythmLength = 3;
          break;
        case 4: // dh _ dh
          rhythmQueue[0] = 12 * noteLength;
          rhythmLength = 1;
          break;
      }
    }
    noteDelay.start(rhythmQueue[rhythmIndex]);
  }

  if (noteDelaySilver.ready()){ // silver bells
    //move to next pitch
    float R = frand();
    for(int i = 0; i < SILVER_NOTE_SIZE; i++){
      if(silverNotesCum[currNoteSilver][i] >= R){
        currNoteSilver = i;
        break;
      }
    }
    currTime = millis();
    if(silverNoteVals[currNoteSilver] != -1){
      if(currTime < transitionStart || currTime > fadeEnd){
        //do nothing
      } else if (currTime > transitionEnd && currTime < fadeStart){
        createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000); //play on channel 6 for now
      } else if (currTime > transitionStart && currTime < transitionEnd){ //mid transition
        int randNum = irand(0, 100);
        if(randNum < map(currTime, transitionStart, transitionEnd, 0, 100)){
          createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000); //play on channel 6 for now
        }
      } else if (currTime > fadeStart && currTime < fadeEnd){
          int randNum = irand(0, 100);
          if(randNum < map(currTime, fadeStart, fadeEnd, 100, 0)){
            createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000); //play on channel 6 for now
          }
      }
    }


    rhythmIndexSilver++;
    if(rhythmIndexSilver >= rhythmLengthSilver){ //reached end of current rhythm
      rhythmIndexSilver = 0;
      currRhythmSilver = nextRhythmSilver(currRhythmSilver);
      Serial.println(currRhythmSilver);
      switch(currRhythmSilver){
        case 0: // q . q . e . e
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 2 * noteLength;
          rhythmQueueSilver[2] = noteLength;
          rhythmQueueSilver[3] = noteLength;
          rhythmLengthSilver = 4;
          break;
        case 1: // q . q . q
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 2 * noteLength;
          rhythmQueueSilver[2] = 2 * noteLength;
          rhythmLengthSilver = 3;
          break;
        case 2: // h . e . e
          rhythmQueueSilver[0] = 4 * noteLength;
          rhythmQueueSilver[1] = noteLength;
          rhythmQueueSilver[2] = noteLength;
          rhythmLengthSilver = 3;
          break;
        case 3: // q . h
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 4 * noteLength;
          rhythmLengthSilver = 2;
          break;
      }
    }
    noteDelaySilver.start(rhythmQueueSilver[rhythmIndexSilver]);
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
  return StereoOutput::from16Bit(0, 0);
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

int nextRhythm(int currRhythm){
      //move to next pitch
  int nextRhythm = 0;
  float R = frand();
    for(int i = 0; i < RHYTHM_MARK_SIZE; i++){
      if(transitionWeightsCum[currRhythm][i] >= R){
        nextRhythm = i;
        break;
      }
    }
  return nextRhythm;
}

int nextRhythmSilver(int currRhythm){
      //move to next pitch
  int nextRhythmSilver = 0;
  float R = frand();
    for(int i = 0; i < SILVER_RHYTHM_SIZE; i++){
      if(silverRhythmsCum[currRhythmSilver][i] >= R){
        nextRhythmSilver = i;
        break;
      }
    }
  return nextRhythmSilver;
}