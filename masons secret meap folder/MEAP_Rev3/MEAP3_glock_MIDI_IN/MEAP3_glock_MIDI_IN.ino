/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <Meap.h>
#include <MIDI.h>
#include <tables/sin8192_int8.h> // loads sine wavetable

#define DEBUG 1 // 1 to enable serial prints, 0 to disable

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

#define SRCLK_PIN 12
#define RLCK_PIN 13
#define DATA_PIN 16
#define OE_PIN 18

#define NUM_NOTES 19

Meap meap;

uint8_t notes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long end_times[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup(){
  Serial.begin(115200); // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE); // starts Mozzi engine with control rate defined above
  meap.begin();

  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RLCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);

  digitalWrite(OE_PIN, LOW);
}


void loop(){
  audioHook();
    if (MIDI.read())                // Is there a MIDI message incoming ?
    {
      int my_note;
        switch(MIDI.getType())      // Get the type of the message we caught
        {
            case midi::NoteOn:       // If it is a Program Change,
              my_note = MIDI.getData1();
              debug("note: ");
              debug(my_note);
              debug(" velocity: ");
              debugln(MIDI.getData2());
              if(my_note >= 60 && my_note < 79){
                my_note -= 60;
                notes[my_note] = 1;
                end_times[my_note] = millis() + 4;
              }
                break;
            // See the online reference for other message types
            default:
                break;
        }
    }
  // handles deactivating solenoids
  unsigned long curr_time = millis();
  for(int i = 0; i < NUM_NOTES; i++){
    if(notes[i]){
      if(curr_time > end_times[i]){
        notes[i] = 0;
      }
    }
  }
}


void updateControl(){
  meap.readPots();
  meap.readTouch();
  meap.readDip();
}


AudioOutput_t updateAudio(){
  writeSR();
  return StereoOutput::from8Bit(0, 0);
}

void Meap::updateTouch(int number, bool pressed){
  switch(number){
    case 0:
      if(pressed){ // Pad 1 pressed
        Serial.print("t1 pressed ");
      } else{ // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 1:
      if(pressed){ // Pad 2 pressed
        Serial.println("t2 pressed");
      } else{ // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 2:
      if(pressed){ // Pad 3 pressed
        Serial.println("t3 pressed");
      } else{ // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 3:
      if(pressed){ // Pad 4 pressed
        Serial.println("t4 pressed");
      } else{ // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 4:
      if(pressed){ // Pad 5 pressed
        Serial.println("t5 pressed");
      } else{ // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 5:
      if(pressed){ // Pad 6 pressed
        Serial.println("t6 pressed");
      } else{ // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 6:
      if(pressed){ // Pad 7 pressed
        Serial.println("t7 pressed");
      } else{ // Pad 7 released
        Serial.println("t7 released");
      }
      break;
    case 7:
      if(pressed){ // Pad 8 pressed
        Serial.println("t8 pressed");
      } else{ // Pad 8 released
        Serial.println("t8 released");
      }
      break;
  }
}

void Meap::updateDip(int number, bool up){
  switch(number){
    case 0:
      if(up){ // DIP 1 up
        Serial.println("d1 up");
      } else{ // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 1:
      if(up){ // DIP 2 up
        Serial.println("d2 up");
      } else{ // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 2:
      if(up){ // DIP 3 up
        Serial.println("d3 up");
      } else{ // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 3:
      if(up){ // DIP 4 up
        Serial.println("d4 up");
      } else{ // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 4:
      if(up){ // DIP 5 up
        Serial.println("d5 up");
      } else{ // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 5:
      if(up){ // DIP 6 up
        Serial.println("d6 up");
      } else{ // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 6:
      if(up){ // DIP 7 up
        Serial.println("d7 up");
      } else{ // DIP 7 down
        Serial.println("d7 down");
      }
      break;
    case 7:
      if(up){ // DIP 8 up
        Serial.println("d8 up");
      } else{ // DIP 8 down
        Serial.println("d8 down");
      }
      break;
  }
}

void writeSR(){
  // grab LED states and pack them into 8-bit numbers to be sent out the shift registers
  uint8_t sr0val = notes[0] | notes[1] << 1 | notes[2] << 2 | notes[3] << 3 |
                notes[4] << 4 | notes[5] << 5 | notes[6] << 6 | notes[7] << 7;

  uint8_t sr1val = notes[8] | notes[9] << 1 | notes[10] << 2 | notes[11] << 3 |
                notes[12] << 4 | notes[13] << 5 | notes[14] << 6 | notes[15] << 7;

  uint8_t sr2val = notes[16] | notes[17] << 1 | notes[18] << 2 | 0 << 3 |
                0 << 4 | 0 << 5 | 0 << 6 | 0 << 7;

  digitalWrite(RLCK_PIN, LOW);
  shiftOut(DATA_PIN, SRCLK_PIN, MSBFIRST, sr2val);
  shiftOut(DATA_PIN, SRCLK_PIN, MSBFIRST, sr1val);
  shiftOut(DATA_PIN, SRCLK_PIN, MSBFIRST, sr0val);
  digitalWrite(RLCK_PIN, HIGH);
}


