/*
  32-step drum pattern with 8 drums

  pot 0 controls tempo

  pot 1 controls randomness; random steps will be played

  dip switches bring in/out each drum; up is enabled, down is disabled
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include "samples/kick.h"
#include "samples/snare1.h"
#include "samples/hhc.h"
#include "samples/hho.h"
#include "samples/shaker.h"
#include "samples/claves.h"
#include "samples/snare2.h"
#include "samples/snare3.h"

#define NUM_DRUMS 8
mSample<16715, AUDIO_RATE> drum_bank[NUM_DRUMS];                   // maybe change the MAX_SAMPLE_LENGTH? calc how it corresponds to seconds etc
int drum_gains[NUM_DRUMS] = {150, 127, 80, 60, 80, 127, 240, 127}; // manually setting volume of each drum ~8-bit maximum
bool drum_enable[NUM_DRUMS] = {true, true, true, true, true, true, true, true};
float default_freq;

#define PATTERN_LENGTH 32

int sample_pattern[NUM_DRUMS][PATTERN_LENGTH] = {
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1}, // kick
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, // snare
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // hi-hat closed
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // hi-hat open
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}, // shaker
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0}, // clave
    {1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // snare 2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // snare 3
};

int pattern_index = 0;

EventDelay metro;
int metro_period = 100;

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------

  default_freq = (float)AUDIO_RATE / (float)16715;

  // because our sample bank uses all drums of length 16715, we need to manually tell mSample how long each drum will actually be with setEnd
  drum_bank[0].setTable(kick_DATA);
  drum_bank[0].setEnd(kick_NUM_CELLS);

  drum_bank[1].setTable(snare1_DATA);
  drum_bank[1].setEnd(snare1_NUM_CELLS);

  drum_bank[2].setTable(hhc_DATA);
  drum_bank[2].setEnd(hhc_NUM_CELLS);

  drum_bank[3].setTable(hho_DATA);
  drum_bank[3].setEnd(hho_NUM_CELLS);

  drum_bank[4].setTable(shaker_DATA);
  drum_bank[4].setEnd(shaker_NUM_CELLS);

  drum_bank[5].setTable(claves_DATA);
  drum_bank[5].setEnd(claves_NUM_CELLS);

  drum_bank[6].setTable(snare2_DATA);
  drum_bank[6].setEnd(snare2_NUM_CELLS);

  drum_bank[7].setTable(snare3_DATA);
  drum_bank[7].setEnd(snare3_NUM_CELLS);

  int drum_glitch_pitches = true;
  // while writing this patch I made a mistake initializing the pitch of my drums
  // but I thought it sounded nice so I left it in
  // if you want to play your drums at the pitch they were recorded at, set drum_glitch_pitches to false and look at the "else" block below
  if (drum_glitch_pitches)
  {
    drum_bank[0].setFreq(((float)kick_SAMPLERATE / (float)kick_NUM_CELLS) / 6);
    drum_bank[1].setFreq(((float)snare1_SAMPLERATE / (float)snare1_NUM_CELLS) / 4);
    drum_bank[2].setFreq(((float)hhc_SAMPLERATE / (float)hhc_NUM_CELLS) / 5);
    drum_bank[3].setFreq((float)hho_SAMPLERATE / (float)hho_NUM_CELLS);
    drum_bank[4].setFreq((float)shaker_SAMPLERATE / (float)shaker_NUM_CELLS);
    drum_bank[5].setFreq((float)claves_SAMPLERATE / (float)claves_NUM_CELLS);
    drum_bank[6].setFreq((float)snare2_SAMPLERATE / (float)snare2_NUM_CELLS);
    drum_bank[7].setFreq((float)snare3_SAMPLERATE / (float)snare3_NUM_CELLS);
  }
  else
  {
    for (int i = 0; i < 8; i++)
    {
      drum_bank[i].setFreq(default_freq);
    }
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
    metro_period = map(meap.pot_vals[0], 0, 4095, 300, 50); // pot 0 controls tempo
    metro.start(metro_period);
    int curr_step = pattern_index; // what step to we play next?

    if (meap.irand(0, 4095) < meap.pot_vals[1])
    { // if entropy event occurs, we play a random step instead
      curr_step = meap.irand(0, 31);
    }

    for (int i = 0; i < NUM_DRUMS; i++)
    { // check which drums play on current step and trigger them
      if (sample_pattern[i][curr_step] == 1)
      {
        if (drum_enable[i])
        {
          drum_bank[i].start();
        }
      }
    }

    pattern_index = (pattern_index + 1) % 32; // move to next step and cycle to beginning if at end
  }
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int64_t out_sample = 0;

  for (int i = 0; i < NUM_DRUMS; i++)
  {
    out_sample += drum_bank[i].next() * drum_gains[i];
  }

  return StereoOutput::fromNBit(17, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

/**
 * Runs whenever a touch pad is pressed or released
 *
 * int number: the number (0-7) of the pad that was pressed
 * bool pressed: true indicates pad was pressed, false indicates it was released
 */
void updateTouch(int number, bool pressed)
{
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
  {
    drum_enable[number] = true;
  }
  else
  {
    drum_enable[number] = false;
  }
}