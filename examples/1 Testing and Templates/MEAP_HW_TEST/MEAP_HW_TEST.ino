/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <Meap.h>
#include <tables/sin8192_int8.h>  // loads sine wavetable

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable
// #define CV1_PIN 41
// #define CV2_PIN 42 

// int cv_value = 0;

Meap meap;

Oscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);
Oscil<8192, AUDIO_RATE> my_sine2(SIN8192_DATA);

// unsigned long timer = 0;

void setup() {
  Serial.begin(115200);      // begins Serial communication with computer
  startMozzi(CONTROL_RATE);  // starts Mozzi engine with control rate defined above
  meap.begin();

  my_sine.setFreq(440);
  my_sine2.setFreq(220);

  // ledcSetup(0, 5000, 12);  // 0-8191
  // ledcSetup(1, 5000, 12);

  // ledcAttachPin(CV1_PIN, 0);
  // ledcAttachPin(CV2_PIN, 1);
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readInputs();

  // ledcWrite(0, cv_value);
  // ledcWrite(1, cv_value);

  // if (meap.dip_vals[0]) {
  //   if (millis() > timer) {
  //     cv_value = meap.irand(0, 8191);
  //     timer += meap.pot_vals[0];
  //   }
  // } else {
  //   cv_value += meap.pot_vals[0];
  //   if (cv_value > meap.pot_vals[1]) {
  //     Serial.println("wv_rst");
  //     cv_value = 0;
  //   }
  // }

  // Serial.print(meap.pot_vals[0]);
  // Serial.print("   ");
  // Serial.println(meap.pot_vals[1]);

  my_sine.setFreq((float)map(meap.pot_vals[0], 0, 4095, 100, 2000));
  my_sine2.setFreq((float)map(meap.pot_vals[1], 0, 4095, 100, 2000));
}


AudioOutput_t updateAudio() {
  int sample = my_sine.next();
  int sample2 = my_sine2.next();
  return StereoOutput::fromNBit(8, sample, sample2);
}

void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed ");
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
      } else {  // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
      } else {  // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
      } else {  // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
      } else {  // Pad 7 released
        Serial.println("t7 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
        Serial.println("t8 pressed");
      } else {  // Pad 8 released
        Serial.println("t8 released");
      }
      break;
  }
}

void Meap::updateDip(int number, bool up) {
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
        Serial.println("d8 up");
      } else {  // DIP 8 down
        Serial.println("d8 down");
      }
      break;
  }
}
