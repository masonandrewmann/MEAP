/*
  Implements an additive approach to a bell sound designed by Jean-Claude Risset
  pot 0 controls decay time of the bell
  touch pads play the bell on an F major scale
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int16.h>
mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc_bank[11]; // create array of 11 sine waves
ADSR<AUDIO_RATE, AUDIO_RATE> env_bank[11];                         // create array of 11 envelopes

float amplitudes[11] = {1, 0.67, 1, 1.8, 2.67, 1.67, 1.46, 1.33, 1.33, 1, 1.33};         // amplitude ratios relative to fundamental
float decay_ratios[11] = {1, 0.9, 0.65, 0.55, 0.325, 0.35, 0.25, 0.2, 0.15, 0.1, 0.075}; // decays relative to fundamental's decay
float freq_ratios[11] = {0.56, 0.56, 0.92, 0.92, 1.19, 1.7, 2, 2.74, 3, 3.76, 4.07};     // freq multiple of each partial relative to unsounded fundamental
float detunes[11] = {0, 1, 0, 1.7, 0, 0, 0, 0, 0, 0, 0};                                 // detune in num Hz to add onto final frequency

int notes[8] = {mF2, mG2, mA2, mBb2, mC3, mD3, mE3, mF3};

float fundamental = 220;
int duration = 1000;

int transpose = 0;

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
  for (int i = 0; i < 11; i++)
  {
    osc_bank[i].setTable(sin8192_int16_DATA);
    osc_bank[i].setFreq(fundamental * freq_ratios[i] + detunes[i]);
    env_bank[i].setADLevels(255, 255);
    env_bank[i].setTimes(5, 1, 1, duration * decay_ratios[i]);
  }
}

void loop()
{
  audioHook(); // handles Mozzi audio generation behind the scenes
}

/** Called automatically at rate specified by CONTROL_RATE macro, most of your mode should live in here
 */
void updateControl()
{
  meap.readInputs();
  // ---------- YOUR updateControl CODE BELOW ----------
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int64_t out_sample = 0;
  for (int i = 0; i < 11; i++)
  {
    env_bank[i].update();
    out_sample += ((osc_bank[i].next() * env_bank[i].next()) >> 8) * amplitudes[i];
  }
  return StereoOutput::fromNBit(20, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

/**
 * Runs whenever a touch pad is pressed or released
 *
 * int number: the number (0-7) of the pad that was pressed
 * bool pressed: true indicates pad was pressed, false indicates it was released
 */
void updateTouch(int number, bool pressed)
{
  if (pressed)
  { // Any pad pressed
    fundamental = mtof(notes[number]);
    for (int i = 0; i < 11; i++)
    {
      env_bank[i].setTimes(5, 1, 1, meap.pot_vals[0]);
      osc_bank[i].setFreq(fundamental * freq_ratios[i] + detunes[i]);
      env_bank[i].noteOn();
    }
    Serial.println(meap.pot_vals[1] + 50);
  }
  else
  { // Any pad released
  }
  switch (number)
  {
  case 0:
    if (pressed)
    { // Pad 0 pressed
      Serial.println("t0 pressed ");
    }
    else
    { // Pad 0 released
      Serial.println("t0 released");
    }
    break;
  case 1:
    if (pressed)
    { // Pad 1 pressed
      Serial.println("t1 pressed");
    }
    else
    { // Pad 1 released
      Serial.println("t1 released");
    }
    break;
  case 2:
    if (pressed)
    { // Pad 2 pressed
      Serial.println("t2 pressed");
    }
    else
    { // Pad 2 released
      Serial.println("t2 released");
    }
    break;
  case 3:
    if (pressed)
    { // Pad 3 pressed
      Serial.println("t3 pressed");
    }
    else
    { // Pad 3 released
      Serial.println("t3 released");
    }
    break;
  case 4:
    if (pressed)
    { // Pad 4 pressed
      Serial.println("t4 pressed");
    }
    else
    { // Pad 4 released
      Serial.println("t4 released");
    }
    break;
  case 5:
    if (pressed)
    { // Pad 5 pressed
      Serial.println("t5 pressed");
    }
    else
    { // Pad 5 released
      Serial.println("t5 released");
    }
    break;
  case 6:
    if (pressed)
    { // Pad 6 pressed
      Serial.println("t6 pressed");
    }
    else
    { // Pad 6 released
      Serial.println("t6 released");
    }
    break;
  case 7:
    if (pressed)
    { // Pad 7 pressed
      Serial.println("t7 pressed");
    }
    else
    { // Pad 7 released
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
void updateDip(int number, bool up)
{
  if (up)
  { // Any DIP toggled up
  }
  else
  { // Any DIP toggled down
  }
  switch (number)
  {
  case 0:
    if (up)
    { // DIP 0 up
      Serial.println("d0 up");
    }
    else
    { // DIP 0 down
      Serial.println("d0 down");
    }
    break;
  case 1:
    if (up)
    { // DIP 1 up
      Serial.println("d1 up");
    }
    else
    { // DIP 1 down
      Serial.println("d1 down");
    }
    break;
  case 2:
    if (up)
    { // DIP 2 up
      Serial.println("d2 up");
    }
    else
    { // DIP 2 down
      Serial.println("d2 down");
    }
    break;
  case 3:
    if (up)
    { // DIP 3 up
      Serial.println("d3 up");
    }
    else
    { // DIP 3 down
      Serial.println("d3 down");
    }
    break;
  case 4:
    if (up)
    { // DIP 4 up
      Serial.println("d4 up");
    }
    else
    { // DIP 4 down
      Serial.println("d4 down");
    }
    break;
  case 5:
    if (up)
    { // DIP 5 up
      Serial.println("d5 up");
    }
    else
    { // DIP 5 down
      Serial.println("d5 down");
    }
    break;
  case 6:
    if (up)
    { // DIP 6 up
      Serial.println("d6 up");
    }
    else
    { // DIP 6 down
      Serial.println("d6 down");
    }
    break;
  case 7:
    if (up)
    { // DIP 7 up
      Serial.println("d7 up");
    }
    else
    { // DIP 7 down
      Serial.println("d7 down");
    }
    break;
  }
}