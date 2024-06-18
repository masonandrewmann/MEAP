/*
  Basic FM Patch
  
  Plays a constant sine wave at 440Hz modulates its frequency using another sine wave.
  Pot 1 controls the modulation oscillator's frequency.
  Pot 2 controls modulation depth.

  Mason Mann, CC0
 */

#include <Meap.h>
#include <tables/sin8192_int8.h>  // loads sine wavetable
#include <tables/cos4096_int8.h>  // loads sine wavetable

#define CONTROL_RATE 128   // Hz, powers of 2 are most reliable
#define AUDIO_RATE 32768  // Hz, powers of 2 are most reliable

Meap meap;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

Oscil<COS4096_NUM_CELLS, AUDIO_RATE> carrier_osc(COS4096_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> modulator_osc(SIN8192_DATA);

Q16n16 modDepth = 0;

MEAP_Delay_Allpass<int8_t> my_first_delay(32768, 20000);


void setup() {
  Serial.begin(115200);                      // begins Serial communication with computer
  Serial1.begin(31250, SERIAL_8N1, 43, 44);  // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);                  // starts Mozzi engine with control rate defined above
  meap.begin();                              // sets up MEAP object
  carrier_osc.setFreq(440);                  //set frequency of carrier oscillator
  modulator_osc.setFreq(440);                //set frequency of modulation oscillator
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();   // Reads on-board MEAP potentionmeters, results are accessed using meap.pot_vals[0] and meap.pot_vals[1]
  meap.readTouch();  // reads MEAP capacitive touch breakout
  meap.readDip();    // reads on-board MEAP dip switches                                                     // reads potentiometers
  modulator_osc.setFreq((float)map(meap.pot_vals[0], 0, 4095, 1, 1000));  // set mod oscillator range to 1->1000Hz
  // modDepth = Q7n0_to_Q7n8(map(meap.pot_vals[1], 0, 4095, 0, 127));        // convert mod depth to fixed point signed int between 0 and 127
  modDepth = Q7n0_to_Q7n8(70);
  Serial.println(meap.pot_vals[1]>>2);
  my_first_delay.setDelay(meap.pot_vals[1]>>2);
}


AudioOutput_t updateAudio() {
  Q15n16 modulation = modDepth * modulator_osc.next() >> 8;  // calculate mod depth in 15n16 signed int
  int sample = carrier_osc.phMod(modulation * 4);
  int del_sample = my_first_delay.next(sample);
  sample = sample + del_sample;
  return StereoOutput::fromAlmostNBit(9, sample, sample);  // scale by factor of 4 and apply phase mod to carrier oscillator
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
