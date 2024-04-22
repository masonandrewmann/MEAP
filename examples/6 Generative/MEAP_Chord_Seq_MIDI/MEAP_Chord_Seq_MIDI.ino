/*
  Example that generates baroque-style chord progressions in the key of C major

  Pot #1 Control attack time of envelope
  Pot #2 Controls entropy amount

  Mason Mann, CC0
 */

#include <Meap.h>  // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <tables/triangle_warm8192_int8.h>
#include <ArduinoQueue.h>

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

#define DEBUG 1  // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc1(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc2(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc3(TRIANGLE_WARM8192_DATA);
Oscil<TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> osc4(TRIANGLE_WARM8192_DATA);

Ead env1(CONTROL_RATE);  // resolution will be CONTROL_RATE
Ead env2(CONTROL_RATE);  // resolution will be CONTROL_RATE
Ead env3(CONTROL_RATE);  // resolution will be CONTROL_RATE
Ead env4(CONTROL_RATE);  // resolution will be CONTROL_RATE

// envelope values
int gain1 = 0;
int gain2 = 0;
int gain3 = 0;
int gain4 = 0;

int majScale[] = { 0, 2, 4, 5, 7, 9, 11 };  // template for building major scale on top of 12TET
int scaleRoot = 60;                         // root of major scale

int triad[] = { 0, 2, 4 };  // template for triad on top of major scale
int chordRoot = 2;          // root of triad

EventDelay noteDelay;
int noteLength = 600;  // number of milliseconds between notes of arpeggio
int arpCounter = 0;    // current note of arpeggio (0->3)

int treeLevel = 4;  // what level of chord tree are we on

int entropy = 0;  // amount of entropy from 0 to 100


int noteMin = 50;
int noteMax = 200;

int offset = 0;

struct midiNote {
  int noteNum;
  int channel;
  unsigned long endTime;
};

ArduinoQueue<struct midiNote*> myNoteQueue(500);

int myMappedNum = 0;

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  //set oscillators to pitches of first chord (iii chord on c major scale)
  osc1.setFreq(mtof(scaleRoot + majScale[(triad[0] + chordRoot) % 7]));
  osc2.setFreq(mtof(scaleRoot + majScale[(triad[1] + chordRoot) % 7]));
  osc3.setFreq(mtof(scaleRoot + majScale[(triad[2] + chordRoot) % 7]));
  osc4.setFreq(2 * mtof(scaleRoot + majScale[(triad[0] + chordRoot) % 7]));

  setAtkDec(10, 1000);  // sets all envelopes at once, function defined at bottom of code

  noteDelay.start(noteLength);  //start first note timer
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  noteMax = map(meap.pot_vals[0], 0, 4095, 100, 4000);
  noteLength = map(meap.pot_vals[1], 0, 4095, 1000, 50);

  // entropy = map(potVals[1], 0, 4095, 0, 100); // pot #2 controls amount of entropy, which chooses order of notes in arpeggio
  entropy = 0;

  if (noteDelay.ready()) {               // if it is time to play the next note
    int myEntropy = meap.irand(0, 100);  // decide if we should have a random attack time
    int myDecay = meap.irand(10, 2000);  // decide if we should have a random attack time
    if (myEntropy < entropy) {
      setAtkDec(map(myEntropy, 0, 100, 10, 2000), myDecay);  // pot #1 controls attack time of envelope
    } else {
      setAtkDec(map(meap.pot_vals[0], 0, 4095, 10, 2000), 2000);  // pot #1 controls attack time of envelope
    }

    // --------------------WHAT STATE SHOULD WE MOVE TO NEXT?--------------------------------------
    if (arpCounter == 4) {  // if we have finished our arpeggio, prepare to move to next chord
      arpCounter = 0;
      if (treeLevel != 0) {
        treeLevel = treeLevel - 1;  // move one level down the tree
      } else {
        treeLevel = meap.irand(0, 4);  // if already at bottom of tree jump to a random level
      }

      // --------------------WHAT HAPPENS IN EACH STATE?-------------------------------------------
      switch (treeLevel) {  // choose chord based on tree level
        case 4:             // leaves : iii
          chordRoot = 2;    // iii chord (E minor)
          break;
        case 3:  // twigs: I or vi
          myMappedNum = meap.irand(0, 3);
          if (myMappedNum == 0) {  // 25% chance of I, 75% chance of vi
            chordRoot = 0;         // I chord (C major)
          } else {
            chordRoot = 5;  // vi chord (A minor)
          }
          break;
        case 2:  // branches: IV or ii
          myMappedNum = meap.irand(0, 2);
          if (myMappedNum == 0) {  // 33% chance of ii, 66% chance off IV
            chordRoot = 1;         // ii chord (D minor)
          } else {
            chordRoot = 3;  // IV chord (F major)
          }
          break;
        case 1:  // boughs: V or vii˚
          myMappedNum = meap.irand(0, 3);
          if (myMappedNum == 0) {  // 25% chance of vii˚, 75% chance of V
            chordRoot = 6;         // vii˚ chord (B diminished)
          } else {
            chordRoot = 4;  // V chord (G major)
          }
          break;
        case 0:  // trunk: I or vi
          myMappedNum = meap.irand(0, 3);
          ;
          if (myMappedNum == 0) {  // 25% chance of vi, 75% chance of I
            chordRoot = 5;         // vi chord (A minor)
          } else {
            chordRoot = 0;  // I chord (C major)
          }
          break;
      }
      osc1.setFreq(mtof(scaleRoot + majScale[(triad[0] + chordRoot) % 7]));      // root of chord (modded to octave)
      osc2.setFreq(mtof(scaleRoot + majScale[(triad[1] + chordRoot) % 7]));      // third of chord (modded to octave)
      osc3.setFreq(mtof(scaleRoot + majScale[(triad[2] + chordRoot) % 7]));      // fifth of chord (modded to octave)
      osc4.setFreq(2 * mtof(scaleRoot + majScale[(triad[0] + chordRoot) % 7]));  // octave above root
    } else {
      noteDelay.start(noteLength);  // set length of note

      // --------------------Handling Arpeggio--------------------
      myMappedNum = meap.irand(0, 100);  // generate random number between 0 and 100
      int myNoteNum = arpCounter;        // set note to be triggered equal to note next note in arpeggio
      if (myMappedNum < entropy) {       // compare random number to entropy
        myNoteNum = myMappedNum % 4;     // if random number is less than entropy value, select a random note from chord to play
      }
      switch (myNoteNum) {
        case 0:
          env1.start();
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 24, 80, 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, 80, 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, 80, 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, 80, 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, 80, 1, meap.irand(noteMin, noteMax));
          delay(4);
          createMIDINote(offset + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, 80, 1, meap.irand(noteMin, noteMax));
          break;
        case 1:
          env2.start();
          createMIDINote(offset + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          delay(4);
          createMIDINote(offset + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          break;
        case 2:
          env3.start();
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          delay(4);
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + scaleRoot + majScale[(triad[2] + 2 + chordRoot) % 7], meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          break;
        case 3:
          env4.start();
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          delay(1);
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[0] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
          createMIDINote(offset + 12 + scaleRoot + majScale[(triad[1] + chordRoot) % 7] - 12, meap.irand(20, 60), 1, meap.irand(noteMin, noteMax));
      }
      arpCounter++;  // move on to next note of arpeggio
    }
  }



  // --------------------Grab next envelope values--------------------
  gain1 = env1.next();
  gain2 = env2.next();
  gain3 = env3.next();
  gain4 = env4.next();
}


AudioOutput_t updateAudio() {
  if (!myNoteQueue.isEmpty()) {
    struct midiNote* myNote = myNoteQueue.getHead();
    if (millis() > myNote->endTime) {
      MIDI.sendNoteOff(myNote->noteNum, 100, myNote->channel);
      free(myNote);

      myNoteQueue.dequeue();
    }
  }
  // adds all four oscillators together and multiplies them by envelope
  int sample = gain1 * osc1.next() + gain2 * osc2.next() + gain3 * osc3.next() + gain4 * osc4.next();
  return StereoOutput::fromAlmostNBit(18, sample, sample);
}

//Function for setting all four envelopes at once
void setAtkDec(int atk, int dec) {
  env1.setAttack(atk);
  env1.setDecay(dec);
  env2.setAttack(atk);
  env2.setDecay(dec);
  env3.setAttack(atk);
  env3.setDecay(dec);
  env4.setAttack(atk);
  env4.setDecay(dec);
}

void createMIDINote(int noteNum, int velocity, int channel, int length) {
  struct midiNote* myMidiNote = (struct midiNote*)malloc(sizeof(struct midiNote));
  myMidiNote->noteNum = noteNum;
  myMidiNote->channel = channel;
  myMidiNote->endTime = millis() + length;
  myNoteQueue.enqueue(myMidiNote);
  MIDI.sendNoteOn(noteNum, velocity, channel);
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
        offset = 0;
      } else {  // Pad 1 released
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        offset = 12;
      } else {  // Pad 2 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        offset = 24;
      } else {  // Pad 3 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        offset = -12;
      } else {  // Pad 4 released
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
        offset = 7;
      } else {  // Pad 5 released
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
        offset = 1;
      } else {  // Pad 6 released
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
        offset = 2;
      } else {  // Pad 7 released
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
        offset = 3;
      } else {  // Pad 8 released
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
