/*

  DIP 1, 2, 3 enable pad, tubular bells and melody respectively

  DIP 8 enables random MIDI gen
  pot 0 controls density of midi gen
  pot 1 controls central pitch of midi gen




  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Mux.h>
#include <SPI.h>
#include <tables/sin8192_int8.h>  // loads sine wavetable
#include <EventDelay.h>
#include <MIDI.h>
#include <ADSR.h>
#include <LinkedList.h>

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable


#define CHORD_CHANNEL 5 // 3 for MU15
#define CHORD_OFFSET -12
#define MELODY_CHANNEL 1 // 8 for MU15

#define DEBUG 1  // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define NOTE_MARK_SIZE 12
#define RHYTHM_MARK_SIZE 5

#define SILVER_NOTE_SIZE 9
#define SILVER_RHYTHM_SIZE 4

#define NUM_CLOUDS 4

float frand();
long irand(long howsmall, long howbig);

using namespace admux;

// variables for DIP switches
Mux mux(Pin(14, INPUT, PinType::Digital), Pinset(38, 45, 46));
int dipPins[] = { 5, 6, 7, 4, 3, 0, 2, 1 };
int dipVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int prevDipVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// variables for capacitive touch pads
int touchPins[] = { 2, 4, 6, 8, 1, 3, 5, 7 };
int touchAvgs[] = { 100, 100, 100, 100, 100, 100, 100, 100 };
int touchVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int prevTouchVals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int touchThreshold = 500;

// variables for potentiometers
int potVals[] = { 0, 0 };

int treeLevel = 2;

int noteCounter = 32;

int chordLength = 4000;

bool specialChordFlag = false;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

class MidiNote {
public:
  int noteNum;
  int channel;
  unsigned long endTime;

  // MidiNote(int _note_num, int _channel, int _end_time){
  //   noteNum = _note_num;
  //   channel = _channel;
  //   endTime = _end_time;
  // }
};

class MidiCloud{
  public:
    unsigned long start_time = 0;
    unsigned long end_time = 1000;
    int channel = 1;
    int duration = 1000;
    int start_density = 50;
    int end_density = 50;
    int start_pitch_min = 48;
    int start_pitch_max = 72;
    int end_pitch_min = 48;
    int end_pitch_max = 72;
    int start_length = 3;
    int end_length = 3;
    bool active = false;
    // add smth for timbre
    // add smth for set of notes chosen from

  void setTimes(unsigned long _start_time, unsigned long _end_time){
    start_time = _start_time;
    end_time = _end_time;
  }

  void setChannel(int _channel_num){
    channel = _channel_num;
  }

  // Sets densities (in num notes per second)
  void setDensities(int _start_density, int _end_density){
    start_density = _start_density;
    end_density = _end_density;
  }

  // Sets range of MIDI notes
  void setPitchRange(int _start_min, int _start_max, int _end_min, int _end_max){
    start_pitch_min = _start_min;
    start_pitch_max = _start_max;
    end_pitch_min = _end_min;
    end_pitch_max = _end_max;
  }

  // Sets length of MIDI notes
  void setLengths(int _start_length, int _end_length){
    start_length = _start_length;
    end_length = _end_length;
  }

  // Begins a MIDI cloud with specified duration
  void start(int _duration){
    duration = _duration;
    active = true;
    start_time = millis();
    end_time = start_time + _duration;
  }

  // Checks if a note should be triggered on this step.
  // Returns a MidiNote object if yes, returns NULL if no
  // Don't forget to free the returned MidiNote object after using
  MidiNote* next(){
    //interpolate the values 
    if (active){
      int curr_time = millis();
      float curr_density = map(curr_time, start_time, end_time, start_density, end_density);
      float thresh = curr_density / (float)CONTROL_RATE;

      if(curr_time > end_time){
        active = false;
      }

      if(frand() < thresh){ // play a note
        int curr_pitch_min = map(curr_time, start_time, end_time, start_pitch_min, end_pitch_min);
        int curr_pitch_max = map(curr_time, start_time, end_time, start_pitch_max, end_pitch_max);
        int curr_length = map(curr_time, start_time, end_time, start_length, end_length);

        int my_pitch = irand(curr_pitch_min, curr_pitch_max);
        MidiNote* this_note = new MidiNote();
        //constrain pitch to c major pentatonic scale .. UNHARDCODE THIS LATER
        int pmd = my_pitch % 12;
        while(pmd != 0 && pmd != 2 && pmd != 4 && pmd != 7 && pmd != 9){
          my_pitch++;
          pmd = my_pitch % 12;
        }
        this_note->noteNum = my_pitch;
        this_note->channel = channel; //hardcoded for now FIX LATER
        this_note->endTime = curr_length; // I'm passing note length not end time in this arg, THIS IS BAD AND CONFUSING
        return this_note;
      } else{
        return NULL;
      }
    } else {
      return NULL;
    }
  }
};

LinkedList<MidiNote *> myNoteQueue = LinkedList<MidiNote *>();

int currNoteSilver2 = 3;

int bellCounter = 0;

int musicBoxNotes[] = { 84, 83, 76, 79, 81, 79 };
int musicBoxIndex = -1;
int musicBoxCounter = 0;

bool stepTrig = false;
bool pattStart = false;

// SILVER BELLS + SILENT NIGHT MARKOV VARIABLES

// for triggering the envelope
EventDelay noteDelay;
EventDelay noteDelaySilver;
int noteLength = 280;

// transition table

int transitionNotes[] = { 67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, -1 };
int silverNoteVals[] = { 83, 84, 85, 86, 88, 90, 91, 93, 95 };

float transitionWeights[NOTE_MARK_SIZE][NOTE_MARK_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  //from G3
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from A3
  { 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2 },  //from B3
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from C4
  { 0, 0, 5, 1, 0, 5, 0, 0, 0, 0, 0, 0 },  //from D4
  { 0, 0, 0, 0, 6, 2, 0, 2, 0, 0, 0, 0 },  //from E4
  { 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0 },  //from F#4
  { 0, 0, 0, 0, 2, 0, 2, 1, 0, 1, 0, 0 },  //from G4
  { 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 1, 0 },  //from A4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  //from B4
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },  //from C5
  { 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0 }   //from rest
};

float transitionWeightsCum[NOTE_MARK_SIZE][NOTE_MARK_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from G3
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from A3
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from B3
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from C4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from D4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from E4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from F#4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from G4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from A4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from B4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //from C5
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }   //from rest
};

float rhythmWeights[RHYTHM_MARK_SIZE][RHYTHM_MARK_SIZE] = {
  { 2, 3, 2, 0, 1 },
  { 1, 0, 4, 0, 0 },
  { 3, 2, 2, 1, 0 },
  { 1, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0 }
};

float rhythmWeightsCum[RHYTHM_MARK_SIZE][RHYTHM_MARK_SIZE] = {
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

float silverNotes[SILVER_NOTE_SIZE][SILVER_NOTE_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 2, 1, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 2, 2, 0, 0, 1, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 3, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 2, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0 }
};

float silverNotesCum[SILVER_NOTE_SIZE][SILVER_NOTE_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

float silverRhythms[SILVER_RHYTHM_SIZE][SILVER_RHYTHM_SIZE] = {
  { 1, 1, 0, 0 },
  { 0, 1, 1, 1 },
  { 0, 1, 0, 0 },
  { 1, 0, 0, 0 }
};

float silverRhythmsCum[SILVER_RHYTHM_SIZE][SILVER_RHYTHM_SIZE] = {
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 }
};

int currNote = 4;
int currRhythm = 0;

int currNoteSilver = 3;
int currRhythmSilver = 0;


int rhythmQueue[10] = { 0 };  //up to 10 notes in queue
int rhythmIndex = 0;
int rhythmLength = 3;

int rhythmQueueSilver[10] = { 0 };  //up to 10 notes in queue
int rhythmIndexSilver = 0;
int rhythmLengthSilver = 3;

EventDelay metro;
int metroCounter = 0;

unsigned long transitionStart = 30000;
unsigned long transitionEnd = 90000;
unsigned long fadeStart = 120000;
unsigned long fadeEnd = 150000;
unsigned long currTime = 0;

//---------- MIDI GEN VARIABLES ---------

Oscil<8192, AUDIO_RATE> osc1(SIN8192_DATA);
;
Oscil<8192, AUDIO_RATE> osc2(SIN8192_DATA);
;

EventDelay myDel;
int delTime = 12;

ADSR<AUDIO_RATE, AUDIO_RATE> env1a;
int gain1;
ADSR<AUDIO_RATE, AUDIO_RATE> env2a;
int gain2;

int numWaits1 = 0;
int numWaits2 = 0;

int majScale[] = { 0, 2, 4, 5, 7, 9, 11 };  // template for building major scale on top of 12TET
int scaleRoot = 48;                         // root of major scale

int seventh[] = { 0, 2, 4, 6 };  // template for triad on top of major scale
int chordRoot = 0;               // root of triad

int octaveOffset = 0;

int osc1Pan = 0;
int osc2Pan = 0;

// MIDI CLOUD VARIABLES

MidiCloud my_midi_cloud[NUM_CLOUDS];
EventDelay cloud_delay[NUM_CLOUDS];

unsigned long clouds_start_time = 0;
unsigned long clouds_end_time = 1000;
int clouds_dir = 1;
bool clouds_active = false;
int cloud_velocity = 100;

bool markov_init = true;


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  touchSetCycles(1, 1);                      // sets up touch pads
  randomSeed(10);                            // gives random seed to arduino function (the number 10 is arbitrary, it generates a true random num behind the scenes)
  xorshiftSeed((long)random(1000));          // uses arduino random number generator to seed the Mozzi random function
  MIDI.turnThruOff();

  // markovSetup();
  // genSetup();
  initializeMU15();
  midiCloudSetup();
}


void loop() {
  audioHook();
}


void updateControl() {
  readDip();    // reads DIP switches
  readTouch();  // reads capacitive touch pads
  readPots();   // reads potentiometers
  readMidi();
  if(!markov_init){
    // markovLoop();
    if (dipVals[7]) {
      genLoop();
    }
  }
  
  if(clouds_active){
    midiCloudLoop();
  }




  if (pattStart && stepTrig) {
    if(markov_init){
      // markovSetup();
      genSetup();
      markov_init = false;
    }
    stepTrig = false;
    //bell handling
    bellCounter--;
    if (bellCounter <= 0) {
      //move to next pitch
      float R = frand();
      for (int i = 0; i < SILVER_NOTE_SIZE; i++) {
        if (silverNotesCum[currNoteSilver2][i] >= R) {
          currNoteSilver2 = i;
          break;
        }
      }
      if (silverNoteVals[currNoteSilver2] != -1) {
        if (dipVals[1]) {
          createMIDINote(silverNoteVals[currNoteSilver2] - 12, 100, 7, 1000);  //play on channel 6 for now
        }
      }
      int bellDel = irand(0, 1);
      switch (bellDel) {
        case 0:
          bellCounter = 64;
          // break;
        case 1:
          bellCounter = 128;
          // break;
      }
    }

    //music box handling
    musicBoxCounter--;
    if (musicBoxCounter <= 0) {
      musicBoxIndex++;
      if (musicBoxIndex > 5) {
        musicBoxIndex = 0;
      }
      if (dipVals[2]) {
        createMIDINote(musicBoxNotes[musicBoxIndex]-12, 70, MELODY_CHANNEL, 500);
        createMIDINote(musicBoxNotes[musicBoxIndex] - 12-12, 90, MELODY_CHANNEL, 500);
      }

      // debugln(musicBoxIndex);
      musicBoxCounter = 2;
    }

    // chord handling
    noteCounter--;
    if (noteCounter <= 0) {
      treeLevel = treeLevel - 1;
      if (treeLevel < 0) {
        treeLevel = 2;
      }
      int chordRand = 0;
      switch (treeLevel) {
        case 2:  // FIRST CHORD
          chordRand = irand(0, 2);
          if (dipVals[0]) {
            switch (chordRand) {
              case 0:
                createMIDINote(64 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(67 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                break;
              case 1:
                createMIDINote(60 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(64 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(67 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                break;
              case 2:
                createMIDINote(64 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(67 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(71 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                break;
            }
          }
          noteCounter = 32;
          break;

        case 1:  // SECOND CHORD
          chordRand = irand(0, 2);
          if (dipVals[0]) {
            switch (chordRand) {
              case 0:
                createMIDINote(67 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(71 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                break;
              case 1:
                createMIDINote(67 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(71 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(74 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                break;
              case 2:
                createMIDINote(69 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                createMIDINote(72 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                specialChordFlag = true;
                break;
            }
          }

          noteCounter = 32;
          break;

        case 0:  // LAST CHORD
          if (specialChordFlag) {
            specialChordFlag = false;
            if (dipVals[0]) {
              createMIDINote(55 - 12 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
              createMIDINote(55 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
              createMIDINote(59 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
              createMIDINote(62 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
            }
          } else {
            chordRand = irand(0, 2);
            if (dipVals[0]) {
              switch (chordRand) {
                case 0:
                  createMIDINote(57 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  createMIDINote(60 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  break;
                case 1:
                  createMIDINote(57 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  createMIDINote(60 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  createMIDINote(64 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  break;
                case 2:
                  createMIDINote(53 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  createMIDINote(57 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  createMIDINote(60 + CHORD_OFFSET, 100, CHORD_CHANNEL, chordLength);
                  break;
              }
            }
          }
          noteCounter = 64;
          break;
      }
    }
  }

  //freeing finished midi notes
  int myQueueSize = myNoteQueue.size();
  debugln(myQueueSize);
  if (myQueueSize > 0) {
    MidiNote *myMidiNote;
    unsigned long myTime = millis();
    do{
      for (int i = 0; i < myQueueSize; i++) {
        myMidiNote = myNoteQueue.get(i);
        if (myTime > myMidiNote->endTime) {
          MIDI.sendNoteOff(myMidiNote->noteNum, 100, myMidiNote->channel);
          delete (myNoteQueue.get(i));
          myNoteQueue.remove(i);
          break;
        }
      }
    } while (myNoteQueue.size() > 500);
  }
}


AudioOutput_t updateAudio() {
  // return MonoOutput::fromAlmostNBit(17, osc1.next() * gain1 + osc2.next() * gain2);
  int l_samp = 0;
  int r_samp = 0;
  env1a.update();
  env2a.update();

  if (osc1Pan == 0) {
    l_samp += osc1.next() * env1a.next();
  } else {
    r_samp += osc1.next() * env1a.next();
  }

  if (osc2Pan == 0) {
    l_samp += osc2.next() * env2a.next();
  } else {
    r_samp += osc2.next() * env2a.next();
  }
  return StereoOutput::fromAlmostNBit(17, l_samp, r_samp);
}

void readDip() {
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
    mux.read(i);  //read once and throw away result (for reliability)
    dipVals[dipPins[i]] = !mux.read(i);
  }
  //Check if any switches changed position
  for (int i = 0; i < 8; i++) {
    switch (i) {
      case 0:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 1 up");
          } else {
            debugln("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 2 up");
          } else {
            debugln("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 3 up");
          } else {
            debugln("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 4 up");
          } else {
            debugln("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 5 up");
          } else {
            debugln("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 6 up");
          } else {
            debugln("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 7 up");
            //begin the midi clouds
            midiCloudSetup();
            clouds_start_time = millis();
            clouds_end_time = clouds_start_time + 30000;
            clouds_active = true;
          } else {
            debugln("DIP 7 down");
            clouds_active = false;
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]) {
          if (dipVals[i]) {
            debugln("DIP 8 up");
          } else {
            debugln("DIP 8 down");
          }
        }
        break;
    }
    prevDipVals[i] = dipVals[i];  // update prevDipVals array
  }
}

void readTouch() {
  int pinVal = 0;
  for (int i = 0; i < 8; i++) {
    pinVal = touchRead(touchPins[i]);
    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal;
    if (touchAvgs[i] > touchThreshold) {
      touchVals[i] = 1;
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]) {
      switch (i) {
        case 0:
          if (touchVals[i]) {  // pad 0 pressed
            debugln("pad 0 pressed");
            // chordRoot = 0;
            MIDI.sendProgramChange(12, 1);  //piano
            MIDI.sendProgramChange(1, 2);   //marimba
          } else {                          // pad 0 released
            debugln("pad 0 released");
          }
          break;
        case 1:
          if (touchVals[i]) {  // pad 1 pressed
            debugln("pad 1 pressed");
            MIDI.sendProgramChange(92, 1);  //bowed pad
            MIDI.sendProgramChange(89, 2);  //warmpad
            // chordRoot = 5;
          } else {  // pad 1 released
            debugln("pad 1 released");
          }
          break;
        case 2:
          if (touchVals[i]) {  // pad 2 pressed
            debugln("pad 2 pressed");
            // chordRoot = 3;
            MIDI.sendProgramChange(52, 1);  //ooh
            MIDI.sendProgramChange(53, 2);  //ahh
          } else {                          // pad 2 released
            debugln("pad 2 released");
          }
          break;
        case 3:
          if (touchVals[i]) {  // pad 3 pressed
            debugln("pad 3 pressed");
            // chordRoot = 4;
            MIDI.sendProgramChange(5, 1);   //ooh
            MIDI.sendProgramChange(89, 2);  // square
            // MIDI.sendControlChange(, , 2);//cutoff -29
            // MIDI.sendControlChange(73, 16, 2);//atk 16
          } else {  // pad 3 released
            debugln("pad 3 released");
          }
          break;
        case 4:
          if (touchVals[i]) {  // pad 4 pressed
            debugln("pad 4 pressed");
          } else {  // pad 4 released
            debugln("pad 4 released");
          }
          break;
        case 5:
          if (touchVals[i]) {  // pad 5 pressed
            debugln("pad 5 pressed");
          } else {  // pad 5 released
            debugln("pad 5 released");
          }
          break;
        case 6:
          if (touchVals[i]) {  // pad 6 pressed
            debugln("pad 6 pressed");
          } else {  // pad 6 released
            debugln("pad 6 released");
          }
          break;
        case 7:
          if (touchVals[i]) {  // pad 7 pressed
            debugln("pad 7 pressed");
          } else {  // pad 7 released
            debugln("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void readPots() {
  potVals[0] = 4095 - mozziAnalogRead(9);
  potVals[1] = 4095 - mozziAnalogRead(10);
}

long irand(long howsmall, long howbig) {  // generates a random integer between howsmall and howbig (inclusive of both numbers)
  howbig++;
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return (xorshift96() % diff) + howsmall;
}

float frand()  // generates a random float between 0 and 1 with 4 decimals of precision
{
  return (xorshift96() % 10000 / 10000.f);
}

void createMIDINote(int noteNum, int velocity, int channel, int length) {
  MidiNote *myMidiNote = new MidiNote();
  myMidiNote->noteNum = noteNum;
  myMidiNote->channel = channel;
  myMidiNote->endTime = millis() + length;
  myNoteQueue.add(myMidiNote);
  MIDI.sendNoteOn(noteNum, velocity, channel);
}

void readMidi() {
  if (MIDI.read())  // Is there a MIDI message incoming ?
  {
    switch (MIDI.getType())  // Get the type of the message we caught
    {
      case midi::NoteOn:  // If it is a Program Change,
        // debug("note: ");
        // debug(MIDI.getData1());
        // debug(" velocity: ");
        // debugln(MIDI.getData2());
        if (MIDI.getData1() == 127) {
          // debugln("trig");
          stepTrig = true;
        } else if (MIDI.getData1() == 126) {
          pattStart = true;
        }
        break;
      // See the online reference for other message types
      default:
        break;
    }
  }
}

int nextRhythm(int currRhythm) {
  //move to next pitch
  int nextRhythm = 0;
  float R = frand();
  for (int i = 0; i < RHYTHM_MARK_SIZE; i++) {
    if (transitionWeightsCum[currRhythm][i] >= R) {
      nextRhythm = i;
      break;
    }
  }
  return nextRhythm;
}

int nextRhythmSilver(int currRhythm) {
  //move to next pitch
  int nextRhythmSilver = 0;
  float R = frand();
  for (int i = 0; i < SILVER_RHYTHM_SIZE; i++) {
    if (silverRhythmsCum[currRhythmSilver][i] >= R) {
      nextRhythmSilver = i;
      break;
    }
  }
  return nextRhythmSilver;
}

int constrainMIDI(int noteNum) {
  while (noteNum > 127) {
    noteNum -= 12;
  }
  while (noteNum < 0) {
    noteNum += 12;
  }
  return noteNum;
}

void markovSetup() {
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sums[NOTE_MARK_SIZE] = { 0 };

  // CALCULATING SUMS
  for (int row = 0; row < NOTE_MARK_SIZE; row++) {
    for (int col = 0; col < NOTE_MARK_SIZE; col++) {
      sums[row] += transitionWeights[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < NOTE_MARK_SIZE; row++) {
    for (int col = 0; col < NOTE_MARK_SIZE; col++) {
      transitionWeights[row][col] /= sums[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < NOTE_MARK_SIZE; row++) {
    for (int col = 0; col < NOTE_MARK_SIZE; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        transitionWeightsCum[row][col] += transitionWeights[row][i];
      }
    }
  }

  // ------CALCULATING RHYTHM TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsR[RHYTHM_MARK_SIZE] = { 0 };

  // CALCULATING SUMS
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++) {
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++) {
      sumsR[row] += rhythmWeights[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++) {
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++) {
      rhythmWeights[row][col] /= sumsR[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < RHYTHM_MARK_SIZE; row++) {
    for (int col = 0; col < RHYTHM_MARK_SIZE; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        rhythmWeightsCum[row][col] += rhythmWeights[row][i];
      }
    }
  }

  // ------CALCULATING SILVER RHYTHM TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsSr[SILVER_RHYTHM_SIZE] = { 0 };

  // CALCULATING SUMS
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++) {
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++) {
      sumsSr[row] += silverRhythms[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++) {
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++) {
      silverRhythms[row][col] /= sumsSr[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < SILVER_RHYTHM_SIZE; row++) {
    for (int col = 0; col < SILVER_RHYTHM_SIZE; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        silverRhythmsCum[row][col] += silverRhythms[row][i];
      }
    }
  }

  // ------CALCULATING SILVER NOTE TRANSITION TABLE-------
  // SETTING UP PROBABILITY DENSITY FUNCTION
  int sumsSn[SILVER_NOTE_SIZE] = { 0 };

  // CALCULATING SUMS
  for (int row = 0; row < SILVER_NOTE_SIZE; row++) {
    for (int col = 0; col < SILVER_NOTE_SIZE; col++) {
      sumsSn[row] += silverNotes[row][col];
    }
  }

  // NORMALIZING PDF
  for (int row = 0; row < SILVER_NOTE_SIZE; row++) {
    for (int col = 0; col < SILVER_NOTE_SIZE; col++) {
      silverNotes[row][col] /= sumsSn[row];
    }
  }

  // CALCULATING CUMULATIVE PDF
  for (int row = 0; row < SILVER_NOTE_SIZE; row++) {
    for (int col = 0; col < SILVER_NOTE_SIZE; col++) {  // compute cumulative probability density function
      for (int i = 0; i <= col; i++) {
        silverNotesCum[row][col] += silverNotes[row][i];
      }
    }
  }

  //first rhythm is 0 (dotted quarter, eighth, quarter)
  rhythmQueue[0] = 3 * noteLength;
  rhythmQueue[1] = noteLength;
  rhythmQueue[2] = 2 * noteLength;
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

  metro.start(noteLength);
}

void markovLoop() {
  currTime = millis();
  if (currTime < fadeEnd) {
    if (metro.ready()) {  // cello background
      metro.start(noteLength);
      metroCounter++;
      if (metroCounter >= 2) {
        metroCounter = 0;
      }
      switch (metroCounter) {
        case 0:
          createMIDINote(43, 80, 12, 1000);
          break;
        case 1:
          createMIDINote(50, 80, 12, 1000);
          break;
      }
    }
  }


  if (noteDelay.ready()) {  // silent night
    //move to next pitch
    float R = frand();
    for (int i = 0; i < NOTE_MARK_SIZE; i++) {
      if (transitionWeightsCum[currNote][i] >= R) {
        currNote = i;
        break;
      }
    }

    currTime = millis();
    if (transitionNotes[currNote] != -1) {
      // trigger next note
      if (currTime < transitionStart) {
        createMIDINote(transitionNotes[currNote], 90, 4, 1000);
      } else if (currTime > transitionEnd) {
        //do nothing
      } else {  //mid transition
        int randNum = irand(0, 100);
        if (randNum < map(currTime, transitionStart, transitionEnd, 100, 0)) {
          createMIDINote(transitionNotes[currNote], 90, 4, 1000);
        }
      }
    }

    rhythmIndex++;
    if (rhythmIndex >= rhythmLength) {  //reached end of current rhythm
      rhythmIndex = 0;
      currRhythm = nextRhythm(currRhythm);
      switch (currRhythm) {
        case 0:  // dq . e . q
          rhythmQueue[0] = 3 * noteLength;
          rhythmQueue[1] = noteLength;
          rhythmQueue[2] = 2 * noteLength;
          rhythmLength = 3;
          break;
        case 1:  // dh
          rhythmQueue[0] = 6 * noteLength;
          rhythmLength = 1;
          break;
        case 2:  // h . q
          rhythmQueue[0] = 4 * noteLength;
          rhythmQueue[1] = 2 * noteLength;
          rhythmLength = 2;
          break;
        case 3:  // q . q . q
          rhythmQueue[0] = 2 * noteLength;
          rhythmQueue[1] = 2 * noteLength;
          rhythmQueue[2] = 2 * noteLength;
          rhythmLength = 3;
          break;
        case 4:  // dh _ dh
          rhythmQueue[0] = 12 * noteLength;
          rhythmLength = 1;
          break;
      }
    }
    noteDelay.start(rhythmQueue[rhythmIndex]);
  }

  if (noteDelaySilver.ready()) {  // silver bells
    //move to next pitch
    float R = frand();
    for (int i = 0; i < SILVER_NOTE_SIZE; i++) {
      if (silverNotesCum[currNoteSilver][i] >= R) {
        currNoteSilver = i;
        break;
      }
    }
    currTime = millis();
    if (silverNoteVals[currNoteSilver] != -1) {
      if (currTime < transitionStart || currTime > fadeEnd) {
        //do nothing
      } else if (currTime > transitionEnd && currTime < fadeStart) {
        createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000);       //play on channel 6 for now
      } else if (currTime > transitionStart && currTime < transitionEnd) {  //mid transition
        int randNum = irand(0, 100);
        if (randNum < map(currTime, transitionStart, transitionEnd, 0, 100)) {
          createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000);  //play on channel 6 for now
        }
      } else if (currTime > fadeStart && currTime < fadeEnd) {
        int randNum = irand(0, 100);
        if (randNum < map(currTime, fadeStart, fadeEnd, 100, 0)) {
          createMIDINote(silverNoteVals[currNoteSilver], 115, 6, 1000);  //play on channel 6 for now
        }
      }
    }


    rhythmIndexSilver++;
    if (rhythmIndexSilver >= rhythmLengthSilver) {  //reached end of current rhythm
      rhythmIndexSilver = 0;
      currRhythmSilver = nextRhythmSilver(currRhythmSilver);
      switch (currRhythmSilver) {
        case 0:  // q . q . e . e
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 2 * noteLength;
          rhythmQueueSilver[2] = noteLength;
          rhythmQueueSilver[3] = noteLength;
          rhythmLengthSilver = 4;
          break;
        case 1:  // q . q . q
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 2 * noteLength;
          rhythmQueueSilver[2] = 2 * noteLength;
          rhythmLengthSilver = 3;
          break;
        case 2:  // h . e . e
          rhythmQueueSilver[0] = 4 * noteLength;
          rhythmQueueSilver[1] = noteLength;
          rhythmQueueSilver[2] = noteLength;
          rhythmLengthSilver = 3;
          break;
        case 3:  // q . h
          rhythmQueueSilver[0] = 2 * noteLength;
          rhythmQueueSilver[1] = 4 * noteLength;
          rhythmLengthSilver = 2;
          break;
      }
    }
    noteDelaySilver.start(rhythmQueueSilver[rhythmIndexSilver]);
  }
}


void genSetup() {
  osc1.setFreq(mtof(scaleRoot + majScale[(seventh[0] + chordRoot) % 7]));
  osc2.setFreq(mtof(scaleRoot + majScale[(seventh[1] + chordRoot) % 7]));
  env1a.setADLevels(255, 0);
  env2a.setADLevels(255, 0);
  env1a.setTimes(10, 100, 0, 0);
  env2a.setTimes(10, 100, 0, 0);
}

void genLoop() {

  delTime = map(potVals[0], 0, 4095, 300, 2);
  octaveOffset = map(potVals[1], 0, 4095, -3, 3);

  if (myDel.ready()) {
    numWaits1--;
    numWaits2--;

    myDel.start(delTime);
    if (numWaits1 <= 0) {
      numWaits1 = irand(2, 8);
      if (touchVals[4]) {
        numWaits1 = 1;
      } else if (touchVals[5]) {
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
      while (myNoteNum > 3) {
        myNoteNum -= 4;
        myOctave++;
      }
      myNoteNum = 12 + scaleRoot + majScale[(seventh[myNoteNum] + chordRoot) % 7] + 12 * myOctave + 12 * octaveOffset;
      osc1.setFreq(mtof(myNoteNum));
      env1a.noteOn();
      osc1Pan = irand(0, 1);

      createMIDINote(constrainMIDI(myNoteNum), 100, 1, envVal);
    }

    if (numWaits2 <= 0) {
      numWaits2 = irand(8, 16);
      if (delTime < 4) {
        numWaits2 = 1;
      }
      if (touchVals[6]) {
        numWaits2 = 6;
      } else if (touchVals[7]) {
        numWaits2 = 12;
      }
      //generate random note
      int myNoteNum = irand(0, 23);  // switch max to 23
      //calculate mapping values
      int envVal = map(myNoteNum, 0, 23, 3500, 500);
      //apply envelope
      env2a.setTimes(10, envVal, 0, 0);
      //apply frequency
      int myOctave = 0;
      while (myNoteNum > 3) {
        myNoteNum -= 4;
        myOctave++;
      }
      myNoteNum = scaleRoot + majScale[(seventh[myNoteNum] + chordRoot) % 7] + 12 * myOctave - 12 + 12 * octaveOffset;
      osc2.setFreq(mtof(myNoteNum));
      env2a.noteOn();
      osc2Pan = irand(0, 1);

      createMIDINote(constrainMIDI(myNoteNum), 100, 2, envVal);
    }
  }
}

void initializeMU15() {
  MIDI.sendProgramChange(12, 1);   // channel 1 marimba-13
  MIDI.sendProgramChange(0, 2);    // channel 2 piano-1
  MIDI.sendProgramChange(91, 3);   // channel 3 vocal pad-92
  MIDI.sendProgramChange(24, 4);  // channel 4 nylon guitar 25
  MIDI.sendProgramChange(42, 5);   // channel 5 Cello 43
  MIDI.sendProgramChange(10, 6);   // channel 6 Music Box-11
  MIDI.sendProgramChange(14, 7);   // channel 7 Tubular Bells-15

  MIDI.sendProgramChange(9, 8);   // channel 8 Glockenspiel-10
  MIDI.sendProgramChange(32, 9);   // channel 9 Acoustic Bass-33
  MIDI.sendProgramChange(80, 10);   // channel 10 Square Lead-81
  MIDI.sendProgramChange(89, 11);   // channel 11 Warm Pad-89
  MIDI.sendProgramChange(12, 12);   // channel 12 Marimba 13

  MIDI.sendProgramChange(12, 13);   // channel 13 Marimba 13
  MIDI.sendProgramChange(46, 14);   // channel 14 Harp 47
  MIDI.sendProgramChange(12, 15);   // channel 15 Marimba 13
  MIDI.sendProgramChange(46, 16);   // channel 16 Harp 47

  MIDI.sendControlChange(0x48, 1, 13);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x48, 1, 14);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x48, 1, 15);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x48, 1, 16);  // pad rel = 127 = +63

  MIDI.sendControlChange(0x4A, 20, 13);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x4A, 20, 14);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x4A, 20, 15);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x4A, 20, 16);  // pad rel = 127 = +63

  MIDI.sendControlChange(0x49, 109, 3);  // pad atk = 109 = +45
  MIDI.sendControlChange(0x48, 127, 3);  // pad rel = 127 = +63
  MIDI.sendControlChange(0x5B, 127, 3);  // pad reverb = 127
  MIDI.sendControlChange(0x5D, 127, 3);  // pad chorus = 127

  MIDI.sendControlChange(0x5B, 127, 3);  // tubular bells reverb = 127

  // MIDI.sendControlChange(0x48, 107, 10);  // square rel = 107 = +44
}

void midiCloudSetup(){
  for (int i =0; i < NUM_CLOUDS; i++){
    cloud_delay[i].start(irand(0, 2000));
  }
  // my_midi_cloud[0].setDensities(1, 30);
  // my_midi_cloud[1].setDensities(1, 128);
  // my_midi_cloud[2].setDensities(100, 10);
  // my_midi_cloud[3].setDensities(50, 1);

  my_midi_cloud[0].setDensities(1, 2);
  my_midi_cloud[1].setDensities(1, 2);
  my_midi_cloud[2].setDensities(2, 1);
  my_midi_cloud[3].setDensities(2, 1);

  my_midi_cloud[0].setChannel(13);
  my_midi_cloud[1].setChannel(14);
  my_midi_cloud[2].setChannel(15);
  my_midi_cloud[3].setChannel(16);
}

void midiCloudLoop(){
  midiCloudUpdateDensities();

  for(int i = 0; i < NUM_CLOUDS; i++){
    MidiNote* this_note = my_midi_cloud[i].next();
    if (!this_note){
    } else{
      createMIDINote(this_note->noteNum, cloud_velocity, this_note->channel, this_note->endTime);
      delete this_note;
    }
    if (cloud_delay[i].ready()){
      int start_pitch_min = irand(30, 100);
      int end_pitch_min = irand(30, 100);
      int start_pitch_max = start_pitch_min + irand(0, 30);
      int end_pitch_max = end_pitch_min + irand(0, 30);
      my_midi_cloud[i].setPitchRange(start_pitch_min, start_pitch_max, end_pitch_min, end_pitch_max);
      int cloud_length = irand(500, 10000);
      my_midi_cloud[i].start(cloud_length);
      cloud_delay[i].start(cloud_length + irand(0, 2000));
    }
  }
}

void midiCloudUpdateDensities(){
  unsigned long curr_time = millis();
  // debugln(map(curr_time, clouds_start_time, clouds_end_time, 1, 128));
  if(clouds_dir == 1){
    my_midi_cloud[0].setDensities(1, map(curr_time, clouds_start_time, clouds_end_time, 1, 10)); //30
    my_midi_cloud[1].setDensities(1, map(curr_time, clouds_start_time, clouds_end_time, 1, 20)); //128
    my_midi_cloud[2].setDensities(1, map(curr_time, clouds_start_time, clouds_end_time, 1, 20)); // 100
    my_midi_cloud[3].setDensities(1, map(curr_time, clouds_start_time, clouds_end_time, 1, 10)); // 50
    if (curr_time > clouds_end_time){
      clouds_dir = 2;
      clouds_start_time = curr_time;
      clouds_end_time = curr_time + 20000;
      my_midi_cloud[0].setDensities(1, 128); //30
      my_midi_cloud[1].setDensities(1, 128); //128
      my_midi_cloud[2].setDensities(128, 1); // 100
      my_midi_cloud[3].setDensities(128, 1); // 50
      cloud_velocity = 90;
    }
  } else if (clouds_dir == 2){
    // if (curr_time > clouds_end_time){
    //   clouds_dir = -1;
    //   clouds_start_time = curr_time;
    //   clouds_end_time = curr_time + 20000;
    //   cloud_velocity = 80;
    // }
  }else if (clouds_dir == -1){
    my_midi_cloud[0].setDensities(map(curr_time, clouds_start_time, clouds_end_time, 10, 1), 1);
    my_midi_cloud[1].setDensities(map(curr_time, clouds_start_time, clouds_end_time, 20, 1), 1);
    my_midi_cloud[2].setDensities(map(curr_time, clouds_start_time, clouds_end_time, 20, 1), 1);
    my_midi_cloud[3].setDensities(map(curr_time, clouds_start_time, clouds_end_time, 10, 1), 1);
    if (curr_time > clouds_end_time){
      clouds_dir = -2;
      clouds_start_time = curr_time;
      clouds_end_time = curr_time + 20000;
      my_midi_cloud[0].setDensities(1, 2); //30
      my_midi_cloud[1].setDensities(1, 2); //128
      my_midi_cloud[2].setDensities(2, 1); // 100
      my_midi_cloud[3].setDensities(2, 1); // 50
    }
  } else if (clouds_dir == -2){
    if (curr_time > clouds_end_time){
      clouds_dir = 0;
      clouds_active = false;
    }
  }
}