/*
  Triggers notes from a major chord.
  pot 0 controls tempo
  pot 1 controls octave offset

  touch pads control chord the notes are chosen from as follows:
  C  F  G  C^
  C# F# G# C#^
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/tri8192_int16.h>

#define POLYPHONY 8 // num of voices

mOscil<tri8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc_bank[POLYPHONY]; // bank of oscillators
ADSR<AUDIO_RATE, AUDIO_RATE> env_bank[POLYPHONY];                         // bank of envelopes
int current_osc = 0;                                                      // current oscillator index to trigger (between 0 and POLYPHONY-1)

int num_waits = 1; // how many metronome ticks to wait in between notes

EventDelay metro;
int metro_period = 100;                         // period of metronome in ms
int major_chord[7] = {0, 4, 7, 12, 16, 19, 24}; // two octaves of a major chord in semitone offsets from root
int chord_root = 24;                            // MIDI note num of chord root

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------

  for (int i = 0; i < POLYPHONY; i++)
  {
    env_bank[i].setTimes(1, 500, 1, 1);
    env_bank[i].setADLevels(255, 0);
    osc_bank[i].setTable(tri8192_int16_DATA);
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

  if (metro.ready())
  {
    metro_period = map(meap.pot_vals[0], 0, 4095, 200, 10);
    num_waits--;
    metro.start(metro_period);
    if (num_waits <= 0)
    {
      num_waits = meap.irand(2, 8);                                 // choose how long to wait until next note
      int template_index = max(meap.irand(0, 6), meap.irand(0, 6)); // choose note from 2-octave major chord, with a greater chance of a higher note
      int decay_val = map(template_index, 0, 6, 1000, 200);         // map scale degree to note length, high notes will be shorter
      int octave = map(meap.pot_vals[1], 0, 4095, 0, 7) * 12;
      osc_bank[current_osc].setFreq(mtof(octave + chord_root + major_chord[template_index]));
      // env_bank[current_osc].setDecayTime(decay_val);
      env_bank[current_osc].noteOn();
      current_osc = (current_osc + 1) % POLYPHONY;
    }
  }
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int64_t out_sample = 0;

  // 8 voices of 16*8=24bits each so 26 bits total
  for (int i = 0; i < POLYPHONY; i++)
  {
    env_bank[i].update();
    out_sample += osc_bank[i].next() * env_bank[i].next();
  }

  return StereoOutput::fromNBit(26, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
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
      chord_root = 24;
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
      chord_root = 29;
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
      chord_root = 31;
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
      chord_root = 36;
      // chord_root = 25;
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
      chord_root = 25;
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
      chord_root = 30;
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
      chord_root = 32;
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
      chord_root = 37;
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