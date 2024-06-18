/*
  Basic template for working with a stock MEAP board.

 */

#include <Meap.h> // MEAP library, includes all dependent libraries, including all Mozzi modules

#define CONTROL_RATE 64   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;  // creates MEAP object to handle inputs and other MEAP library functions

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

MEAP_Guitar<8> my_first_guitar(4);

// MEAP_Guitar<8>* my_first_guitar;

MEAP_PRCRev<int32_t> my_first_reverb;

MEAP_Chorus<int16_t>my_first_chorus;

int current_string = 0;

float freqs[8] = {440, 493.88, 554.37, 587.33, 659.25, 739.99, 830.61, 880};

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  my_first_guitar.setPluckPosition(0.5, -1);
  my_first_guitar.setLoopGain(0.5, -1);
  my_first_reverb.init(5);
}


void loop() {
  audioHook();  // handles Mozzi audio generation behind the scenes
}


/** Called automatically at rate specified by CONTROL_RATE macro, most of your mode should live in here
	*/
void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches

  // my_first_guitar.setPluckPosition(((float)meap.pot_vals[0])/4095.f, -1);
  // my_first_guitar.setLoopGain(((float)meap.pot_vals[1])/4095.f, -1);
my_first_guitar.setPluckPosition(1.0);
  my_first_guitar.setLoopGain(0.95);
  

  my_first_reverb.setEffectMix(0.3);

  my_first_chorus.setModDepth(((float)meap.pot_vals[0])/4095.f);

  my_first_chorus.setModFrequency(((float)meap.pot_vals[1])/4095.f*20 + 0.2);
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
	*/
AudioOutput_t updateAudio() {
int sample = my_first_guitar.next(0);
// int rev_l = my_first_reverb.next(sample, 0);
// int rev_r = my_first_reverb.lastOut(1);

int c_L = my_first_chorus.next(sample, 0);
int c_R = my_first_chorus.lastOut(1);

  return StereoOutput::fromAlmostNBit(16, c_L, c_R);
}

/** User defined function called whenever a touch pad is pressed or released
  @param number is the number of the pad that was pressed or released: 0-7
  @param pressed is true if the pad was pressed and false if the pad was released
	*/
void Meap::updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed
    my_first_guitar.noteOn(freqs[number], 1.0, number);
    Serial.println("touched");
  } else {  // Any pad released
  my_first_guitar.noteOff(0.5, number);
  Serial.println("released");
  
  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
      
      } else {        // Pad 1 released
      
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
      } else {        // Pad 2 released
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
      } else {        // Pad 3 released
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
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
  // my_first_guitar.setBodyFile();
  } else {  //Any DIP down
  }
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
      Serial.println("set t60"); 
      // my_first_reverb.setT60(5);
      } else {   // DIP 1 down
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
      Serial.println("init");
      // my_first_reverb.init(5);
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
