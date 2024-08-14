/*
  Meap.h - Library accompanying MEAP boards.
  Created by Mason Mann, January 24, 2024.
*/
#ifndef MEAP_CLASSES_LEGACY_3_H_
#define MEAP_CLASSES_LEGACY_3_H_

#include <hal/adc_hal.h>
#include <esp32-hal-adc.h>
#include "tables/m_sin1024_uint8.h"
#include "esp32-hal-touch.h"

#include <soc/sens_reg.h>

#include <mozzi_rand.h>

#define MEAP_PI 3.14159265358979;

struct StereoSample
{
  int32_t l;
  int32_t r;
};

struct MeapNoteAndVoice
{
  uint16_t note_num;
  uint16_t voice_num;
};

enum AdcModes
{
  kWAITING,
  kREADY
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
  static StereoSample pan2(int64_t sample, uint8_t pos);

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

  void setMuxChannel(int8_t a, int8_t b, int8_t c, int8_t channel);

  void turnLedDOn();

  void turnLedDOff();

  void readInputs();

  void mHandleTouch();

  bool mTouchReady();

  uint16_t mTouchResult();

  void mSetCycles(uint16_t measure, uint16_t sleep);

  void mTouchInit();

  uint16_t startTouchConversion(uint8_t pin);

  void mTouchChannelInit(int pad);

  // variables
  int8_t dip_pins[8] = {5, 6, 7, 4, 3, 0, 2, 1}; // previously
  int8_t dip_vals[8] = {2, 2, 2, 2, 2, 2, 2, 2}; // set to 2 so dip up/down will execute when program is started
  int8_t prev_dip_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int16_t dip_common_pin = 14;

  int aux_mux_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int16_t aux_mux_common_pin = 11;

  int touch_pins[8] = {2, 4, 6, 8, 1, 3, 5, 7};
  int touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int prev_touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_avgs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_threshold = 210;

  int pot_vals[2] = {0, 0};
  int pot_pins[2] = {8, 9}; // GPIO 9 and 10 (ADC1 channels 8 and 9)

  // for mux and adc
  int8_t dip_mux_read_channel = 0;
  int8_t aux_mux_read_channel = 0;
  uint64_t dip_mux_ready_time = 0;
  uint64_t aux_mux_ready_time = 0;
  uint64_t mux_propogation_delay = 2; // 1ms propogation delay
  uint64_t pot_update_time = 0;
  uint64_t pot_update_delay = 1;
  uint8_t pot_read_num = 0;
  bool adc1flag = false;
  AdcModes adc1_mode = kREADY;

  int8_t touch_read_channel = 0;
  uint64_t touch_update_time = 0;
  uint64_t touch_update_delay = 1;
  AdcModes touch_mode = kREADY;

private:
};

long irand(long howsmall, long howbig);

float frand();

#endif // MEAP_CLASSES_LEGACY_3_H_
