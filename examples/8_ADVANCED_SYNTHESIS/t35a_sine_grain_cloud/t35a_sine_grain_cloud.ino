/*
  Example of using mGrainCloud which is based on Curtis Roads' Cloud Generator
  Triggers a new cloud with random characteristics when pad 0 is pressed.
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int16.h> // table for Oscils to play
mGrainCloud<sin8192_int16_NUM_CELLS, 16, int16_t> cloud(sin8192_int16_DATA);

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
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
  int64_t out_sample = cloud.next();

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
      cloud.setLength(meap.irand(500, 8000));
      cloud.setDensities(meap.irand(1, 128), meap.irand(1, 128));
      cloud.setAmplitudes(meap.irand(1, 255), meap.irand(1, 255));
      float min_start = min(meap.irand(20, 4000), meap.irand(20, 4000));
      float min_end = min(meap.irand(20, 4000), meap.irand(20, 4000));
      float max_start = min_start + min(meap.irand(1, 4000), meap.irand(1, 4000));
      float max_end = min_end + min(meap.irand(1, 4000), meap.irand(1, 4000));

      cloud.setFreqMins(min_start, min_end);
      cloud.setFreqMaxs(min_end, max_end);
      cloud.setDurations(meap.irand(4, 50), meap.irand(4, 50));
      cloud.noteOn();
    }
    else
    { // Pad 0 released
      Serial.println("t0 released");
    }
    break;
  }
}

void updateDip(int number, bool up)
{
}