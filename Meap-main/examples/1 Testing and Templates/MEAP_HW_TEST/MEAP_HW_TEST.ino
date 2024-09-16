/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Generates two sine waves with pitches controlled by potentiometers
  and sends one out the left channel and one out the right channel
  Additionally, prints whenever a DIP switch or capacitive touch input 
  is pressed.
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable

#include <Meap.h>
#include <tables/sin8192_int8.h>  // loads sine wavetable

Meap meap;

Oscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);
Oscil<8192, AUDIO_RATE> my_sine2(SIN8192_DATA);

void setup() {
  Serial.begin(115200);      // begins Serial communication with computer
  meap.begin();
  startMozzi(CONTROL_RATE);  // starts Mozzi engine with control rate defined above
}


void loop() {
  audioHook();
}


void updateControl() {
  uint64_t before = micros();
  meap.readInputs();
  uint64_t after = micros();
  // Serial.println(after-before); // prints time one call of readInputs takes

  my_sine.setFreq((float)map(meap.pot_vals[0], 0, 4095, 100, 2000)); // pots will set sines to range 100Hz - 2000Hz
  my_sine2.setFreq((float)map(meap.pot_vals[1], 0, 4095, 100, 2000));

  // Serial.print("p0:");
  // Serial.print(meap.pot_vals[0]);
  // Serial.print(",");
  // Serial.print("p1:");
  // Serial.print(meap.pot_vals[1]); 
  // Serial.print(",");
  // Serial.print("v:");
  // Serial.println(meap.volume_val); 
}


AudioOutput_t updateAudio() {
  int sample = my_sine.next();
  int sample2 = my_sine2.next();
  return StereoOutput::fromNBit(8, (sample * meap.volume_val) >> 12, (sample2 * meap.volume_val) >> 12); // testing pots
  // return StereoOutput::fromNBit(16, (meap_input_frame[0] * meap.volume_val) >> 12, (meap_input_frame[1] * meap.volume_val) >> 12); // testing line in
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