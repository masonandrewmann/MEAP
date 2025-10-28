/*
  // pot 0: time between sample triggers
  // volume pot: DOESNT CONTROL VOLUME. CONTROLS THRESHOLD WHEN DIP 4 IS ENGAGED
  // pot 1: pitch of samples

  // DIP 0: generates new sequences when up, repeats same sequence when down
  // DIP 1: randomizes tempo at end of each sequence when up
  // DIP 2: UNUSED
  // DIP 3: speeds up sequence towards end when up
  // DIP 4: enables input envelope following. samples will only be triggered when audio sent to line input is louder than threshold set by volume pot

 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include "sample_includes.h"

#define MAX_SAMPLE_LENGTH 300000
mSample<MAX_SAMPLE_LENGTH, AUDIO_RATE, int8_t> sample_rack[MICRO_NUM_SAMPLES];
float default_freq;

int row_length = 7;
int tone_row[] = {0, 6, 19, 3, 5, 20, 25};
int row_index = 0;
int row_transpose = 0;

EventDelay metro;
float metro_period = 200;
float tempo_mult = 1;

// int time_mult = 1;
float mult_vals[] = {1, 2, 1, 0.5, 0.25, 0.125};
int num_repeats = 0;
float pitch_transpose = 1.0;

Smooth<int16_t> envelope_detector(0.999);
int input_envelope = 0;
bool listen_enable = false;

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);

  // ---------- YOUR SETUP CODE BELOW ----------
  default_freq = (float)AUDIO_RATE / (float)MAX_SAMPLE_LENGTH;

  for (int i = 0; i < MICRO_NUM_SAMPLES; i++)
  {
    sample_rack[i].setFreq(default_freq);
    sample_rack[i].setTable(micro_list[i]);
    sample_rack[i].setEnd(micro_lengths[i]);
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
  metro_period = map(meap.pot_vals[0], 0, 4095, 10, 1000);
  // pitch_transpose = (((float)meap.pot_vals[1]) / 400.f) + 0.25;
  pitch_transpose = (float)map(meap.pot_vals[1], 0, 4095, 250, 10000) / 1000.f;

  if (metro.ready())
  {
    if (meap.dip_vals[3])
    {
      metro.start(metro_period * tempo_mult / ((float)row_index + 1));
    }
    else
    {
      metro.start(metro_period * tempo_mult);
    }

    if (!meap.dip_vals[4] || input_envelope > (meap.volume_val << 3))
    {
      int sample_num = (tone_row[row_index] + row_transpose) % MICRO_NUM_SAMPLES;
      sample_rack[sample_num].setFreq(default_freq * meap.irand(1, 3) * pitch_transpose);
      sample_rack[sample_num].start();
      row_index++;
      if (row_index > row_length)
      {
        if (meap.dip_vals[0])
        { // DIP 0 - randomize row transpose
          row_transpose = meap.irand(1, 15);
        }
        else
        {
          row_transpose = 0;
        }

        if (meap.dip_vals[1])
        { // DIP 1 - randomize row tempo
          tempo_mult = meap.irand(1, 5);
        }
        else
        {
          tempo_mult = 1;
        }

        row_index = 0;
      }
    }
  }
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int64_t out_sample = 0;
  int64_t l_sample = meap_input_frame[0]; // line in left channel
  int64_t r_sample = meap_input_frame[1]; // line in right channel

  if (meap.dip_vals[4])
  { // detect volume of input
    input_envelope = envelope_detector.next(abs(l_sample));
  }

  for (int i = 0; i < MICRO_NUM_SAMPLES; i++)
  { // attach samples to output
    out_sample += sample_rack[i].next();
  }

  out_sample = out_sample << 8; // samples up to 16bit
  l_sample += out_sample;       // add samples to line input signal
  r_sample += out_sample;

  return StereoOutput::fromNBit(18, l_sample, r_sample); // send to output with no volume control
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