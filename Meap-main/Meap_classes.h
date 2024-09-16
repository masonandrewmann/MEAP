/*
  Meap.h - Library accompanying MEAP boards.
  Created by Mason Mann, January 24, 2024.
*/
#ifndef MEAP_CLASSES_H_1
#define MEAP_CLASSES_H_1

#define MEAP_MUX_CONTROL_PIN_A 38
#define MEAP_MUX_CONTROL_PIN_B 45
#define MEAP_MUX_CONTROL_PIN_C 46
#define MEAP_MUX_DIP_PIN 12
#define MEAP_MUX_AUX_PIN 11

#define MEAP_I2C_SDA_PIN 21
#define MEAP_I2C_SCL_PIN 14

#define MEAP_MIDI_IN_PIN 43
#define MEAP_MIDI_OUT_PIN 44

#define MEAP_CV1_PIN 41
#define MEAP_CV2_PIN 42

#define MEAP_LED_0_PIN 36
#define MEAP_LED_1_PIN 37

#define MEAP_POT_0_PIN 10
#define MEAP_POT_1_PIN 9

#define MEAP_VOLUME_POT_PIN 17

#include <Wire.h>
#include <hal/adc_hal.h>
#include <esp32-hal-adc.h>
#include "tables/m_sin1024_uint8.h"
#include "esp32-hal-touch.h"
#include "driver/i2s_std.h"
#include "dependencies/sgtl_reg.h"

#include <soc/sens_reg.h>

#include <MozziHeadersOnly.h>
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

// GLOBAL VARIABLES SO THEY CAN BE SHARED WITH MOZZI FUNCTIONS

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
  static long irand(long howsmall, long howbig);

  /**
   * @brief Generates a random decimal between 0 and 1, inclusive, with four decimal points of precision
   *
   * @return float returns the random number
   */
  static float frand();

  /**
   * @brief Reads DIP switches, touch inputs, built-in potentiometers and aux mux in a non-blocking way.
   * WARNING - this function may not play nicely with additional analog reads!!
   *
   */
  void readInputsFast();

  /**
   * @brief Reads DIP switches, touch inputs, built-in potentiometers and aux mux in a blocking way. Slower
   * but easier to add extra inputs
   *
   */
  void readInputs();

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

  /**
   * @brief Turns a built-in led on or off
   *
   * @param led_num number of the led (0 or 1)
   * @param led_level 0 for off, 1 for on
   */
  void writeLED(uint8_t led_num, uint8_t led_level);

  /**
   * @brief Sets master volume of output
   *
   * @param gain between 60 and 252, 60 being maximum gain
   */
  static void setCodecGain(uint16_t gain);

  /**
   * @brief Sets channel (0-7) on multiplexer A, B, C control pins
   *
   * @param tempo in BPM
   * @return float number of microseconds in one MIDI clock pulse at the specified BPM
   */
  void setMuxChannel(int8_t channel);

  /**
   * @brief Reads a register from SGTL5000
   *
   * @param reg address of register to read
   * @return value at the register
   */
  static uint32_t SGread(uint32_t reg);

  /**
   * @brief Attempts to write a register in SGTL5000
   *
   * @param reg address of register to write
   * @param val value to write in register
   * @return success or failure of write operation
   */
  static bool SGwrite(uint32_t reg, uint32_t val);

  /**
   * @brief Modifies specified bits of SGTL5000 register
   *
   * @param reg address of register to write
   * @param val value to write in register
   * @param iMask bit mask of values to modify in register (1s get modified)
   * @return returns 0 if write fails, or the masked value if write succeeds
   */
  static uint32_t SGmodify(uint32_t reg, uint32_t val, uint32_t iMask);

  // variables
  int8_t dip_pins[8] = {0, 1, 2, 3, 4, 7, 5, 6}; // previously {5, 6, 7, 4, 3, 0, 2, 1};
  int8_t dip_vals[8] = {2, 2, 2, 2, 2, 2, 2, 2};
  int8_t prev_dip_vals[8] = {2, 2, 2, 2, 2, 2, 2, 2}; // set to 2 so dip up/down will execute when program is started

  int aux_mux_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int extra_mux_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int external_mux_vals[8] = {1, 1, 1, 1, 1, 1, 1, 1};

  int touch_pins[8] = {2, 4, 6, 8, 1, 3, 5, 7};
  int touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int prev_touch_vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_avgs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int touch_threshold = 210;

  int led_pins[2] = {MEAP_LED_0_PIN, MEAP_LED_1_PIN};

  int pot_vals[2] = {0, 0};
  int pot_pins[2] = {MEAP_POT_0_PIN - 1, MEAP_POT_1_PIN - 1}; // GPIO 9 and 10 (ADC1 channels 8 and 9)
  int16_t volume_val = 0;

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

  // input smoothing
  float meap_alpha = 0.92;
  float meap_one_minus_alpha = 0.08;

protected:
  /**
   * @brief Initializes the SGTL5000 audio codec
   *
   * @param led_num number of the led (0 or 1)
   * @param led_level 0 for off, 1 for on
   */
  void codecInit();

  /**
   * @brief Sets registers in SGTL5000 audio codec chip, initializing and starting it
   *
   * @return success or failure of initialization
   */
  bool sgtlInit();

  // VARIABLES
};

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

#endif // MEAP_CLASSES_H_1