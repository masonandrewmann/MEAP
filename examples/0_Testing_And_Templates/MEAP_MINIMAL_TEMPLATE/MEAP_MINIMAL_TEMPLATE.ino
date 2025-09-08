/*
  Minimal template for working with a stock MEAP board.
 */

#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Meap.h>        // MEAP library, includes all dependent libraries, including all Mozzi modules

Meap meap;                                           // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------

void setup()
{
    Serial.begin(115200);                                                            // begins Serial communication with computer
    meap.begin();                                                                    // sets up MEAP object
    Serial1.begin(31250, SERIAL_8N1, meap.MEAP_MIDI_IN_PIN, meap.MEAP_MIDI_OUT_PIN); // sets up MIDI: baud rate, serial mode, rx pin, tx pin
    startMozzi(CONTROL_RATE);                                                        // starts Mozzi engine with control rate defined above

    // ---------- YOUR SETUP CODE BELOW ----------
}

void loop()
{
    audioHook();
}

void updateControl()
{
    meap.readInputs();
    // ---------- YOUR updateControl CODE BELOW ----------
}

AudioOutput_t updateAudio()
{
    int32_t out_sample = 0;

    return StereoOutput::fromNBit(8, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

void updateTouch(int number, bool pressed)
{
}

void updateDip(int number, bool up)
{
}