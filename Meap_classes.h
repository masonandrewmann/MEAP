
#ifndef MEAP_CLASSES_MM_H_1
#define MEAP_CLASSES_MM_H_1

#include <Wire.h>
#include <hal/adc_hal.h>
#include <esp32-hal-adc.h>
#include "tables/m_sin1024_uint8.h"
#include "tables/m_power_curve.h" // 7-bit quarter cycle of sine
#include "esp32-hal-touch.h"
#include "driver/i2s_std.h"
#include "dependencies/sgtl_reg.h"

// ---
#include "soc/soc_caps.h"

#include <soc/sens_reg.h>

#include "driver/touch_sensor.h"
#include "esp32-hal-touch.h"
#include "esp32-hal-periman.h"

#include "driver/i2s_std.h"

#include "esp_random.h"
#include "bootloader_random.h"
// ---

#include <soc/sens_reg.h>

#include <MozziHeadersOnly.h>
#include <mozzi_rand.h>

struct MeapNoteAndVoice
{
  uint16_t note_num;
  uint16_t voice_num;
};

#define MEAP_PI 3.14159265358979;
#define MEAP_2_PI 6.28318530717958;
#define MEAP_SQRT_2_PI 2.5066282746309994;
#define MEAP_EULER 2.718281828459045

struct StereoSample
{
  int32_t l;
  int32_t r;
};

enum AdcModes
{
  kWAITING,
  kREADY
};

enum meap_hardware_versions
{
  mMEAP4B,
  mMEAP4C
};

enum meap_interpolation
{
  mINTERP_NONE,
  mINTERP_LINEAR
};

#define MEAP_DEFAULT_VERSION mMEAP4C

bool cap1280_touch_flag;

extern i2s_chan_handle_t tx_handle;
extern i2s_chan_handle_t rx_handle;

// GLOBAL VARIABLES SO THEY CAN BE SHARED WITH MOZZI FUNCTIONS
// meap_hardware_versions hardware_version;

/**
 * @brief User defined function that handles what to do when a touch pad is pressed or released.
 *
 * @param number is the number of the pad that was pressed 0-7 corresponding to pads #1-8 respectively
 * @param pressed is true when a pad is pressed and false when the pad is released
 */
extern void updateTouch(int number, bool pressed);

/**
 * @brief User defined function that handles what to do when a dip switch
 *
 * @param number is the number of the switch that was toggled 0-7 corresponding to switches #1-8 respectively
 * @param up is true when a switch was toggled up and false when a switch was toggled down
 */
extern void updateDip(int number, bool up);

template <meap_hardware_versions hardware_version = MEAP_DEFAULT_VERSION>
class Meap
{
public:
  // Constructor
  Meap() {

  };

  // methods
  /**
   * @brief Sets up various processes for MEAP
   *
   */
  inline void begin()
  {
    switch (hardware_version)
    {
    case mMEAP4B:
      MEAP_MUX_CONTROL_PIN_A = 38;
      MEAP_MUX_CONTROL_PIN_B = 45;
      MEAP_MUX_CONTROL_PIN_C = 46;
      MEAP_MUX_DIP_PIN = 12;
      MEAP_MUX_AUX_PIN = 11;
      MEAP_POT_0_PIN = 10;
      MEAP_POT_1_PIN = 9;
      MEAP_VOLUME_POT_PIN = 17;
      MEAP_MIDI_IN_PIN = 43;
      MEAP_MIDI_OUT_PIN = 44;
      MEAP_CV1_PIN = 41;
      MEAP_CV2_PIN = 42;
      MEAP_LED_0_PIN = 36;
      MEAP_LED_1_PIN = 37;
      MEAP_I2C_SDA_PIN = 21;
      MEAP_I2C_SCL_PIN = 14;
      MEAP_I2S_MCLK = 40;
      MEAP_I2S_BCLK = 35;
      MEAP_I2S_WS = 39;
      MEAP_I2S_DOUT = 47;
      MEAP_I2S_DIN = 48;
      touchSetCycles(1, 1);
      pinMode(MEAP_LED_0_PIN, OUTPUT);
      pinMode(MEAP_LED_1_PIN, OUTPUT);
      digitalWrite(MEAP_LED_0_PIN, LOW);
      digitalWrite(MEAP_LED_0_PIN, LOW);

      break;
    case mMEAP4C:
      MEAP_MUX_CONTROL_PIN_A = 38;
      MEAP_MUX_CONTROL_PIN_B = 45;
      MEAP_MUX_CONTROL_PIN_C = 46;
      MEAP_MUX_DIP_PIN = 12;
      MEAP_MUX_AUX_PIN = 11;
      MEAP_POT_0_PIN = 10;
      MEAP_POT_1_PIN = 9;
      MEAP_VOLUME_POT_PIN = 17;
      MEAP_MIDI_IN_PIN = 44;
      MEAP_MIDI_OUT_PIN = 43;
      MEAP_CV1_PIN = 13;
      MEAP_CV2_PIN = 18;
      MEAP_LED_0_PIN = 42;
      MEAP_LED_1_PIN = 42;
      MEAP_I2C_SDA_PIN = 21;
      MEAP_I2C_SCL_PIN = 14;
      MEAP_I2S_MCLK = 40;
      MEAP_I2S_BCLK = 41;
      MEAP_I2S_WS = 39;
      MEAP_I2S_DOUT = 47;
      MEAP_I2S_DIN = 48;

      MEAP_CAP_IRQ_PIN = 8;
      cap1280_touch_flag = false;

      Wire.begin(MEAP_I2C_SDA_PIN, MEAP_I2C_SCL_PIN); // need to specify address 0x0A when sending messages

      cap1280_write_reg(0x0, 0b00000000);  // make sure we are in active state
      cap1280_write_reg(0x24, 0b00000000); // set averaging and sampling
      cap1280_write_reg(0x1F, 0b00111111); // set sensitivity (default 0 010 1111)

      cap1280_write_reg(0x44, 0b01000000); // set interrupt on release
      cap1280_write_reg(0x28, 0b00000000); // disable retriggering
      cap1280_write_reg(0x27, 0b11111111); // enable all interrupts
      cap1280_write_reg(0x26, 0b11111111); // force calibration routine
      cap1280_write_reg(0x2A, 0b00001100); // turn off MTB

      pinMode(MEAP_LED_0_PIN, OUTPUT);
      digitalWrite(MEAP_LED_0_PIN, LOW);

      pinMode(MEAP_CAP_IRQ_PIN, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(MEAP_CAP_IRQ_PIN), cap1280_interrupt, CHANGE);
      break;
    }

    pinMode(MEAP_MUX_CONTROL_PIN_A, OUTPUT); // mux a
    pinMode(MEAP_MUX_CONTROL_PIN_B, OUTPUT); // mux b
    pinMode(MEAP_MUX_CONTROL_PIN_C, OUTPUT); // mux c

    digitalWrite(MEAP_MUX_CONTROL_PIN_A, LOW);
    digitalWrite(MEAP_MUX_CONTROL_PIN_B, LOW);
    digitalWrite(MEAP_MUX_CONTROL_PIN_C, LOW);

    pinMode(MEAP_MUX_DIP_PIN, INPUT); // dip mux input

    // adjust smoothing amount based on control rate
    if (CONTROL_RATE == 128)
    {
      meap_alpha = 0.92;
      meap_one_minus_alpha = 0.08;
    }
    else if (CONTROL_RATE == 64)
    {
      meap_alpha = 0.88;
      meap_one_minus_alpha = 0.12;
    }
    else if (CONTROL_RATE == 256)
    {
      meap_alpha = 0.96;
      meap_one_minus_alpha = 0.04;
    }

    pot_vals[0] = analogRead(MEAP_POT_0_PIN);
    pot_vals[1] = analogRead(MEAP_POT_1_PIN);
    volume_val = analogRead(MEAP_VOLUME_POT_PIN);

    // set up ADC1
    // REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_FORCE, 1); // adc controlled by software
    // REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD_FORCE, 1); // channel select controlled by software
    // REG_WRITE(SENS_SAR_ATTEN1_REG, 0xFFFFFFFF); // set attenuation
    // analogRead(8); // rlly hacky, just let the API get ADC1 set up bc it seems like i was missing something

    // set up ADC2
    // REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_FORCE, 1); // adc controlled by software
    // REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD_FORCE, 1); // channel select controlled by software
    // analogRead(11); // rlly hacky, just let the API get ADC2 set up bc it seems like i was missing something!

    // set up touch FSM
    // REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_FORCE, 1); // software will start readings
    // REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0);    // clear reading start register
    // REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_DATA_SEL, 0);         // raw data (no smoothing or benchmark)
    // touchRead(1);                                                           // letting the API finish setup for me again, hey it works!
    // touchRead(2);
    // touchRead(3);
    // touchRead(4);
    // touchRead(5);
    // touchRead(6);
    // touchRead(7);
    // touchRead(8);
    // REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL1_REG, RTC_CNTL_TOUCH_MEAS_NUM, 1); // only one reading
    // REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << 1);    // choose channel 1 to begin

    // pinMode(18, INPUT); // for external multiplexer

    codecInit();

    bootloader_random_enable();
    randomSeed(esp_random()); // initializes random number generator
    bootloader_random_disable();
    xorshiftSeed((long)random(1000));
  }

  /**
   * @brief Generates a random integer within a specified range, inclusive of the limit numbers.
   *
   * @param min is the lower limit of the range, inclusive
   * @param max is the upper limit of the range, inclusive
   * @return long returns the random number
   */
  inline static int64_t irand(int64_t min, int64_t max)
  {
    {
      max++;
      if (min >= max)
      {
        return min;
      }
      int64_t diff = max - min;
      return (xorshift96() % diff) + min;
    }
  }

  /**
   * @brief Generates a random decimal between 0 and 1
   *
   * @return float returns the random number
   */
  inline static float frand()
  {
    return ((float)xorshift96() / 0xFFFFFFFF);
  }

  /**
   * @brief Reads DIP switches, touch inputs, built-in potentiometers and aux mux in a non-blocking way.
   * WARNING - this function may not play nicely with additional analog reads!!
   *
   */
  inline void readInputsFast();

  /**
   * @brief Reads DIP switches, touch inputs, built-in potentiometers and aux mux in a blocking way. Slower
   * but easier to add extra inputs
   *
   */
  inline void readInputs()
  {

    // read dips and buttons
    for (int i = 8; --i >= 0;)
    {
      setMuxChannel(i); // repeated several times to allow address signals to propogate through multiplexers
      setMuxChannel(i); // repeated several times to allow address signals to propogate through multiplexers

      // process the touch pads
      if (hardware_version == mMEAP4B)
      {
        touch_avgs[i] = touchRead(touch_pins[i]);
        touch_vals[i] = touch_avgs[i] > touch_threshold;
        if (touch_vals[i] != prev_touch_vals[i])
        {
          updateTouch(i, touch_vals[i]);
          prev_touch_vals[i] = touch_vals[i];
        }
      }

      dip_vals[dip_pins[i]] = !digitalRead(MEAP_MUX_DIP_PIN); // read the DIP mux common pin

      // was a dip pressed?
      if (dip_vals[dip_pins[i]] != prev_dip_vals[dip_pins[i]])
      {
        updateDip(dip_pins[i], dip_vals[dip_pins[i]]);
        prev_dip_vals[dip_pins[i]] = dip_vals[dip_pins[i]];
      }
    }

    if (hardware_version == mMEAP4C)
    {
      if (cap1280_touch_flag)
      {
        cap1280_get_touch_data(touch_vals);
        for (int i = 8; --i >= 0;)
        {
          if (prev_touch_vals[i] != touch_vals[i])
          {
            updateTouch(i, touch_vals[i]);
            prev_touch_vals[i] = touch_vals[i];
          }
        }

        cap1280_touch_flag = false;
      }
    }

    // read builtin pots
    pot_vals[0] = (pot_vals[0] * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_POT_0_PIN));
    pot_vals[1] = (pot_vals[1] * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_POT_1_PIN));
    volume_val = (volume_val * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_VOLUME_POT_PIN));
  }

  /**
   * @brief A stereo equal power panner
   *
   * @param sample is the sample that you want to pan between the left and right output channels
   * @param pos is the position in the stereo field, 0 is hard left, 127 is center and 255 is hard right
   * @return StereoSample
   */
  inline static StereoSample pan2(int64_t sample, uint8_t pos)
  {
    StereoSample my_sample;
    my_sample.l = (m_power_curve_DATA[255 - pos] * sample) >> 7;
    my_sample.r = (m_power_curve_DATA[pos] * sample) >> 7;
    return my_sample;
  }

  /**
   * @brief Calculates the length (in milliseconds) of a quarter note at a specified BPM
   *
   * @param tempo in BPM
   * @return float number of milliseconds in one quarter note at the specified BPM
   */
  inline float tempoToQuarter(float tempo)
  {
    return 60000.f / tempo;
  }

  /**
   * @brief Calculates the length (in microseconds) of a MIDI clock pulse (at 24PPQ) at a specified tempo
   *
   * @param tempo in BPM
   * @return float number of microseconds in one MIDI clock pulse at the specified BPM
   */
  inline float midiPulseMicros(float tempo)
  {
    return (2500000.f / tempo); // 2500 = 60000ms/24pulses
  }

  /**
   * @brief Normalizes input from potentiometers to 0-1 range
   *
   * @param input number between 0 and 4095
   * @return float number between 0 and 1
   */
  inline float normalize(int16_t input)
  {
    return ((float)input) * 0.000244140625;
  }

  /**
   * @brief Convert a value in milliseconds to a value in samples at sampling rate of 32768
   *
   * @param input number between 0 and 4095
   * @return float number between 0 and 1
   */
  inline float msToSamples(float ms)
  {
    return ms * 32.768;
  }

  /**
   * @brief Convert a value in samples at sampling rate of 32768 to a value in milliseconds
   *
   * @param input number between 0 and 4095
   * @return float number between 0 and 1
   */
  inline float samplesToMs(float samples)
  {
    return samples * 0.03051757812;
  }

  /**
   * @brief Turns a built-in led on or off
   *
   * @param led_num number of the led (0 or 1)
   * @param led_level 0 for off, 1 for on
   */
  inline void writeLED(uint8_t led_num, uint8_t led_level)
  {
    digitalWrite(led_pins[led_num], led_level);
  }

  /**
   * @brief Sets master volume of output
   *
   * @param gain between 60 and 252, 60 being maximum gain
   */
  inline static void setCodecGain(uint16_t gain)
  {
    uint16_t register_volume = (gain << 8) + gain;
    SGwrite(CHIP_DAC_VOL, register_volume); // digital gain, 0dB
  }

  /**
   * @brief Sets ADC input gain
   *
   * @param gain between 0 and 15, in 1.5dB steps between 0dB and 22.5dB
   */
  inline static void setCodecInputGain(uint16_t gain)
  {
    SGwrite(CHIP_ANA_ADC_CTRL, gain + (gain << 4));
  }

  inline static void setCodecInputMic()
  {
    SGwrite(CHIP_ANA_CTRL, 0x0022); // enable zero cross detectors / mic input
  }

  inline static void setCodecInputLine()
  {
    SGwrite(CHIP_ANA_CTRL, 0x0026); // enable zero cross detectors / mic input
  }

  /**
   * @brief Sets gain (0-3) of codec microphone input
   *
   * @param gain 0-3 corresponding to 0dB, +20dB, +30dB, +40dB
   */
  inline static void setCodecMicGain(int gain)
  {
    SGmodify(CHIP_MIC_CTRL, gain, 0b11);
  }

  /**
   * @brief Sets channel (0-7) on multiplexer A, B, C control pins
   *
   * @param tempo in BPM
   * @return float number of microseconds in one MIDI clock pulse at the specified BPM
   */
  inline void setMuxChannel(int8_t channel)
  {
    digitalWrite(MEAP_MUX_CONTROL_PIN_A, bitRead(channel, 0));
    digitalWrite(MEAP_MUX_CONTROL_PIN_B, bitRead(channel, 1));
    digitalWrite(MEAP_MUX_CONTROL_PIN_C, bitRead(channel, 2));
  };

  /**
   * @brief Reads a register from SGTL5000
   *
   * @param reg address of register to read
   * @return value at the register
   */
  inline static uint32_t SGread(uint32_t reg)
  {
    uint32_t val;
    Wire.beginTransmission(SGTL5000_I2C_ADDR_CS_LOW);
    Wire.write(reg >> 8);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
      return 0;
    if (Wire.requestFrom((int32_t)SGTL5000_I2C_ADDR_CS_LOW, 2) < 2)
      return 0;
    val = Wire.read() << 8;
    val |= Wire.read();
    return val;
  }

  /**
   * @brief Attempts to write a register in SGTL5000
   *
   * @param reg address of register to write
   * @param val value to write in register
   * @return success or failure of write operation
   */
  inline static bool SGwrite(uint32_t reg, uint32_t val)
  {
    Wire.beginTransmission(SGTL5000_I2C_ADDR_CS_LOW);
    Wire.write(reg >> 8);
    Wire.write(reg);
    Wire.write(val >> 8);
    Wire.write(val);
    if (Wire.endTransmission() == 0)
      return true;
    return false;
  }

  /**
   * @brief Modifies specified bits of SGTL5000 register
   *
   * @param reg address of register to write
   * @param val value to write in register
   * @param iMask bit mask of values to modify in register (1s get modified)
   * @return returns 0 if write fails, or the masked value if write succeeds
   */
  inline static uint32_t SGmodify(uint32_t reg, uint32_t val, uint32_t iMask)
  {
    uint32_t val1 = (SGread(reg) & (~iMask)) | val;
    if (!SGwrite(reg, val1))
      return 0;
    return val1;
  }

  /**
   * @brief Read a 1-byte register on cap1280
   */
  inline uint8_t cap1280_read_reg(uint8_t reg)
  {
    uint8_t val;
    Wire.beginTransmission(0x28);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)0x28, 2);
    val = Wire.read();
    return val;
  }

  /**
   * @brief Write a 1-byte register on cap1280
   */
  inline void cap1280_write_reg(uint8_t reg, uint8_t val)
  {
    Wire.beginTransmission(0x28);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  }

  /**
   * @brief Write a 1-byte register on cap1280
   */
  inline void cap1280_clear_interrupt()
  {
    cap1280_write_reg(0x00, 0b00000000);
  }

  /**
   * @brief Write a 1-byte register on cap1280
   */
  inline void cap1280_get_touch_data(int data[8])
  {
    cap1280_clear_interrupt();
    uint8_t my_reg = cap1280_read_reg(0x03);
    data[3] = (my_reg >> 7) & 0x01;
    data[7] = (my_reg >> 6) & 0x01;
    data[2] = (my_reg >> 5) & 0x01;
    data[6] = (my_reg >> 4) & 0x01;
    data[1] = (my_reg >> 3) & 0x01;
    data[5] = (my_reg >> 2) & 0x01;
    data[0] = (my_reg >> 1) & 0x01;
    data[4] = my_reg & 0x01;
  }

  /**
   * @brief Interrupt callback for cap1280
   */
  static void cap1280_interrupt()
  {
    cap1280_touch_flag = true;
  }

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

  uint32_t max_sustain = 4294967294;

  uint8_t MEAP_MUX_CONTROL_PIN_A;
  uint8_t MEAP_MUX_CONTROL_PIN_B;
  uint8_t MEAP_MUX_CONTROL_PIN_C;
  uint8_t MEAP_MUX_DIP_PIN;
  uint8_t MEAP_MUX_AUX_PIN;
  uint8_t MEAP_POT_0_PIN;
  uint8_t MEAP_POT_1_PIN;
  uint8_t MEAP_VOLUME_POT_PIN;
  int8_t MEAP_MIDI_IN_PIN;
  int8_t MEAP_MIDI_OUT_PIN;
  uint8_t MEAP_CV1_PIN;
  uint8_t MEAP_CV2_PIN;
  uint8_t MEAP_LED_0_PIN;
  uint8_t MEAP_LED_1_PIN;
  uint8_t MEAP_I2C_SDA_PIN;
  uint8_t MEAP_I2C_SCL_PIN;
  uint8_t MEAP_I2S_MCLK;
  uint8_t MEAP_I2S_BCLK;
  uint8_t MEAP_I2S_WS;
  uint8_t MEAP_I2S_DOUT;
  uint8_t MEAP_I2S_DIN;
  uint8_t MEAP_CAP_IRQ_PIN;

  /**
   * @brief Initializes the SGTL5000 audio codec
   *
   * @param led_num number of the led (0 or 1)
   * @param led_level 0 for off, 1 for on
   */
  inline void codecInit()
  {
    delay(300); // wait for boot
    /* Allocate a pair of I2S channel */
    // i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);

    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_AUTO,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 8, // number of the dma buffer?
        .dma_frame_num = 128,
        .auto_clear = false,
    };

    /* Allocate for TX and RX channel at the same time, then they will work in full-duplex mode */
    i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION), // 32000 sample rate... eek i want 32768 orrr do i want 32768*8
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = (gpio_num_t)MEAP_I2S_MCLK,
            .bclk = (gpio_num_t)MEAP_I2S_BCLK,
            .ws = (gpio_num_t)MEAP_I2S_WS,
            .dout = (gpio_num_t)MEAP_I2S_DOUT,
            .din = (gpio_num_t)MEAP_I2S_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_init_std_mode(rx_handle, &std_cfg);

    i2s_channel_enable(tx_handle);
    i2s_channel_enable(rx_handle);
    delay(1);
    Wire.begin(MEAP_I2C_SDA_PIN, MEAP_I2C_SCL_PIN); // need to specify address 0x0A when sending messages
    // i2s_channel_write(tx_handle, &_esp32_prev_sample, 4, &bytes_written, 0); // 4 = 2 * sizeof(int16_t)

    sgtlInit(); // initialize codec
  }

  /**
   * @brief Sets registers in SGTL5000 audio codec chip, initializing and starting it
   *
   * @return success or failure of initialization
   */
  inline bool sgtlInit()
  {
    delay(5);
    SGwrite(CHIP_LINREG_CTRL, 0x006C);   // VDDA & VDDIO both over 3.1V
    SGwrite(CHIP_REF_CTRL, 0x01FF);      // VAG=1.575, slow ramp, +12.5% bias current
    SGwrite(CHIP_LINE_OUT_CTRL, 0x0322); // LO_VAGCNTRL=1.65V, OUT_CURRENT=0.36mA
    SGwrite(CHIP_DIG_POWER, 0x0073);     // power up all digital stuff
    SGwrite(CHIP_ANA_POWER, 0x42FB);     // 4afb

    SGwrite(CHIP_CLK_CTRL, 0x0000); // 32 kHz, 256*Fs
    SGwrite(CHIP_I2S_CTRL, 0x0030); // SCLK=64*Fs, 16bit, I2S format

    SGwrite(CHIP_SSS_CTRL, 0x0010); // ADC->I2S, I2S->DAC

    SGwrite(CHIP_ANA_ADC_CTRL, 0xFF); // adc input gain
    SGwrite(CHIP_MIC_CTRL, 0x142);    // +40dB gain, 2.25v bias, 2kOhm pullup

    SGwrite(CHIP_ANA_HP_CTRL, 0x7F7F); // set output volume to minumum

    SGwrite(CHIP_ANA_CTRL, 0x0026); // enable zero cross detectors / line input, unmute

    int vol = 0x7F;
    while (vol > 0x18)
    {
      vol--;
      SGwrite(CHIP_ANA_HP_CTRL, (vol << 8) | vol);
      delay(10);
    }

    SGwrite(CHIP_DAC_VOL, 0x3C3C);     // digital gain, 0dB
    SGwrite(CHIP_ADCDAC_CTRL, 0x0000); // disable dac mute

    return true;
  }
};

#endif // MEAP_CLASSES_MM_H_1
