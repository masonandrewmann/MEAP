/*
  Basic template for working with a stock MEAP board.
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include "glock_loop.h"

mSampleGrainCloud<glock_loop_NUM_CELLS, 8, int16_t> cloud;

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
  cloud.init(glock_loop_DATA);
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
  cloud.update();
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int32_t out_sample = cloud.next();
  return StereoOutput::fromNBit(19, out_sample, out_sample);
}

/**
 * Runs whenever a touch pad is pressed or released
 *
 * int number: the number (0-7) of the pad that was pressed
 * bool pressed: true indicated pad was pressed, false indicates it was released
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
      Serial.println("t1 pressed ");
      cloud.setLength(meap.irand(500, 8000));
      cloud.setDensities(meap.irand(1, 128), meap.irand(1, 128));
      cloud.setAmplitudes(meap.irand(1, 255), meap.irand(1, 255));
      float min_start = meap.frand() + 0.2;
      float min_end = meap.frand() + 0.2;
      float max_start = min_start + meap.frand();
      float max_end = min_end + meap.frand();
      cloud.setFreqMins(min_start / 4.f, min_end / 4.f);
      cloud.setFreqMaxs(min_end / 4.f, max_end / 4.f);
      cloud.setDurations(4, 100);
      cloud.noteOn();
    }
    else
    { // Pad 1 released
      Serial.println("t1 released");
    }
    break;
  }

  /**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
  void updateDip(int number, bool up)
  {
  }
