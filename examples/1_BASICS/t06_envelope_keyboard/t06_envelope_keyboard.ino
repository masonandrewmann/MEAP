/*
  Capacitive touch keyboard, playing A major scale.
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int16.h> // loads sine wavetable
mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> my_sine(sin8192_int16_DATA);
ADSR<CONTROL_RATE, AUDIO_RATE> my_envelope;

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
  my_sine.setFreq(220);
  my_envelope.setADLevels(255, 200);
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
  my_envelope.setTimes(meap.pot_vals[0], 100, meap.max_sustain, meap.pot_vals[1]);
  my_envelope.update();
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int32_t out_sample = (my_sine.next() * my_envelope.next()) >> 8;
  return StereoOutput::fromNBit(16, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
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
    my_envelope.noteOn();
  }
  else
  { // Any pad released
    my_envelope.noteOff();
  }
  switch (number)
  {
  case 0:
    if (pressed)
    { // Pad 0 pressed
      Serial.println("t0 pressed ");
      my_sine.setFreq(220); // A3
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
      my_sine.setFreq(246.94); // B3
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
      my_sine.setFreq(277.18); // C#4
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
      my_sine.setFreq(293.66); // D4
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
      my_sine.setFreq(329.63); // E4
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
      my_sine.setFreq(369.99); // F#4
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
      my_sine.setFreq(415.3); // G#4
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
      my_sine.setFreq(440); // A4
    }
    else
    { // Pad 7 released
      Serial.println("t7 released");
    }
    break;
  }
}

void updateDip(int number, bool up)
{
}