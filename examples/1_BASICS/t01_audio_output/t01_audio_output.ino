/*
  Constant sine wave output
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int16.h> // loads sine wavetable
mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> my_sine(sin8192_int16_DATA);

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
  my_sine.setFreq(220);
}

void loop()
{
  audioHook(); // handles Mozzi audio generation behind the scenes
}

/** Called automatically at rate specified by CONTROL_RATE macro, most of your code should live in here
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
  int32_t out_sample = my_sine.next();
  return StereoOutput::fromNBit(16, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

void updateTouch(int number, bool pressed)
{
}

void updateDip(int number, bool up)
{
}