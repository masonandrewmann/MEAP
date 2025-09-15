/*
  Tests all hardware of system, and prints to serial when relevant.

  DIPs and Touch Pads will print to serial port when toggled or touched/released

  Pots are used to set frequency of two hard-panned oscillators.

  A MIDI Note On message (note 60, velocity 100) is sent every second, followed by a Note Off (note 60) 500ms later

  Received MIDI messages are logged on the serial port
*/

#define CONTROL_RATE 128
#include <Meap.h>

Meap meap;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

uint32_t midi_timer = 0;
bool midi_state = true;

#include "tables/sin8192_int16.h"

mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc1(sin8192_int16_DATA);
mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc2(sin8192_int16_DATA);

void setup()
{
  Serial.begin(115200);                                                            // begins Serial communication with computer
  meap.begin();                                                                    // sets up MEAP object
  Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
  startMozzi(CONTROL_RATE);
}

void loop()
{
  audioHook(); // handles Mozzi audio generation behind the scenes

  if (millis() > midi_timer)
  {
    midi_timer = millis() + 500;
    if (midi_state)
    {
      MIDI.sendNoteOn(60, 100, 1);
    }
    else
    {
      MIDI.sendNoteOff(60, 0, 1);
    }
    midi_state = !midi_state;
  }

  if (MIDI.read()) // Is there a MIDI message incoming ?
  {
    midiEventHandler();
  }
}

/** Called automatically at rate specified by CONTROL_RATE macro, most of your code should live in here
 */
void updateControl()
{
  meap.readInputs(); // reads DIP switches, potentiometers and touch inputs

  osc1.setFreq(meap.pot_vals[0] + 30);
  osc2.setFreq(meap.pot_vals[1] + 30);
}

/** Called automatically at rate specified by AUDIO_RATE macro, for calculating samples sent to DAC, too much code in here can disrupt your output
 */
AudioOutput_t updateAudio()
{
  int32_t left_sample = osc1.next();
  int32_t right_sample = osc2.next();
  return StereoOutput::fromNBit(18, (left_sample * meap.volume_val) >> 12, (right_sample * meap.volume_val) >> 12);
}

/**
 * Runs whenever a touch pad is pressed or released
 *
 * int number: the number (0-7) of the pad that was pressed
 * bool pressed: true indicates pad was pressed, false indicates it was released
 */
void updateTouch(int number, bool pressed)
{
  Serial.print("Touch ");
  Serial.print(number);
  if (pressed)
  {
    Serial.println(" pressed");
  }
  else
  {
    Serial.println(" released");
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
  Serial.print("DIP ");
  Serial.print(number);
  if (up)
  {
    Serial.println(" up");
  }
  else
  {
    Serial.println(" down");
  }
}

/**
 * Called whenever a MIDI message is recieved.
 */
void midiEventHandler()
{
  Serial.print("MIDI MSG - type: ");
  Serial.print(MIDI.getType());
  Serial.print(" channel: ");
  Serial.print(MIDI.getChannel());
  Serial.print(" data1: ");
  Serial.print(MIDI.getData1());
  Serial.print(" data2: ");
  Serial.println(MIDI.getData2());
}
