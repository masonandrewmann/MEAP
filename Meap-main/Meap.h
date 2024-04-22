/*
  Meap.h - Library accompanying MEAP boards.
  Created by Mason Mann, January 24, 2024.
*/
#ifndef Meap_h
#define Meap_h
#include "Arduino.h"
#include <Mux.h>  // Arduino Analog-Digital Multiplexers library https://github.com/stechio/arduino-ad-mux-lib
#include <MIDI.h> // Arduino Midi library https://github.com/FortySevenEffects/arduino_midi_library

// Attempts to include all Mozzi modules, though I may have missed them. Surely Mozzi has an option to include all modules in one line???
#include <MozziGuts.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <mozzi_rand.h>
#include <ADSR.h>
#include <AudioDelay.h>
#include <AudioDelayFeedback.h>
#include <AutoMap.h>
#include <AutoRange.h>
#include <CapPoll.h>
#include <CircularBuffer.h>
#include <ControlDelay.h>
#include <DCFilter.h>
#include <Ead.h>
#include <EventDelay.h>
#include <IntMap.h>
#include <Line.h>
#include <Metronome.h>
#include <ResonantFilter.h>
#include <Oscil.h>
#include <PdResonant.h>
#include <Phasor.h>
#include <Portamento.h>
#include <RCPoll.h>
#include <ReverbTank.h>
#include <RollingAverage.h>
#include <Sample.h>
#include <SampleHuffman.h>
#include <Smooth.h>
#include <StateVariable.h>
#include <WaveFolder.h>
#include <WavePacket.h>
#include <WaveShaper.h>

using namespace admux;

struct StereoSample
{
  int16_t left;
  int16_t right;
};

class Meap
{
public:
  // Constructor
  Meap();

  // methods
  /**
   * @brief Sets up various processes for MEAP
   *
   */
  void begin();

  /**
   * @brief Generates a random integer within a specified range, inclusive of the limit numbers.
   *
   * @param howsmall is the lower limit of the range, inclusive
   * @param howbig is the upper limit of the range, inclusive
   * @return long returns the random number
   */
  long irand(long howsmall, long howbig);

  /**
   * @brief Generates a random decimal between 0 and 1, inclusive, with four decimal points of precision
   *
   * @return float returns the random number
   */
  float frand();

  /**
   * @brief Reads the two built-in potentiometers, storing the results in the potVals[] array
   *
   */
  void readPots();

  /**
   * @brief Reads the touch pad breakout board, storing the results in the touchVals[] array
   *
   */
  void readTouch();

  /**
   * @brief Reads the built-in dip switches, storing the results in the dipVals[] array
   *
   */
  void readDip();

  /**
   * @brief Reads the auxilliary multiplexer, storing the result in auxMuxVals[] array
   *
   */
  void readAuxMux();

  /**
   * @brief User defined function that handles what to do when a touch pad is pressed or released.
   *
   * @param number is the number of the pad that was pressed 0-7 corresponding to pads #1-8 respectively
   * @param pressed is true when a pad is pressed and false when the pad is released
   */
  void updateTouch(int number, bool pressed);

  /**
   * @brief User defined function that handles what to do when a dip switch
   *
   * @param number is the number of the switch that was toggled 0-7 corresponding to switches #1-8 respectively
   * @param up is true when a switch was toggled up and false when a switch was toggled down
   */
  void updateDip(int number, bool up);

  /**
   * @brief A stereo equal power panner
   *
   * @param sample is the sample that you want to pan between the left and right output channels
   * @param pos is the position in the stereo field, 0 is hard left, 127 is center and 255 is hard right
   * @return StereoSample
   */
  StereoSample pan2(int sample, uint8_t pos);

  /**
   * @brief Calculates the length (in milliseconds) of a quarter note at a specified BPM
   *
   * @param tempo in BPM
   * @return float number of milliseconds in one quarter note at the specified BPM
   */
  float tempoToPeriod(float tempo);

  /**
   * @brief Calculates the length (in microseconds) of a MIDI clock pulse (at 24PPQ) at a specified tempo
   *
   * @param tempo in BPM
   * @return float number of microseconds in one MIDI clock pulse at the specified BPM
   */
  float midiPulseMicros(float tempo);

  // variables
  int dip_pins[8] = {5, 6, 7, 4, 3, 0, 2, 1};
  int dip_vals[8] = {2, 2, 2, 2, 2, 2, 2, 2}; // set to 2 so dip up/down will execute when program is started
  int prev_dip_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  int aux_mux_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  int touch_pins[8] = {2, 4, 6, 8, 1, 3, 5, 7};
  int touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int prev_touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_avgs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_threshold = 400;

  int pot_vals[2] = {0, 0};
  int pot_pins[2] = {9, 10};
  Mux *dip_mux;
  Mux *aux_mux;

private:
};

#endif
