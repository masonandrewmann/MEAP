/*
  Constant sine wave output
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int8.h>  // loads sine wavetable
mOscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);

void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object

  // ---------- YOUR SETUP CODE BELOW ----------
  my_sine.setFreq(220);
}


void loop() {
  audioHook();  // handles Mozzi audio generation behind the scenes
}


/** Called automatically at rate specified by CONTROL_RATE macro, most of your mode should live in here
	*/
void updateControl() {
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
	*/
AudioOutput_t updateAudio() {
  int64_t out_sample = my_sine.next();
  return StereoOutput::fromNBit(8, (out_sample * meap.volume_val)>>12, (out_sample * meap.volume_val)>>12);
}

/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicates pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released

  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 0 pressed
        Serial.println("t0 pressed ");
      } else {  // Pad 0 released
        Serial.println("t0 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed");
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
      } else {  // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
      } else {  // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
      } else {  // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
      } else {  // Pad 7 released
        Serial.println("t7 released");
      }
      break;
  }
}

/**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
void updateDip(int number, bool up) {
  if (up) {  // Any DIP toggled up

  } else {  //Any DIP toggled down

  }
  switch (number) {
    case 0:
      if (up) {  // DIP 0 up
        Serial.println("d0 up");
      } else {  // DIP 0 down
        Serial.println("d0 down");
      }
      break;
    case 1:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 2:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 3:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 4:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 5:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 6:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 7:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
  }
}