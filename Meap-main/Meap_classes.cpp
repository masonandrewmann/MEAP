/*
  Meap.cpp - Library accompanying MEAP boards.
  Created by Mason Mann, January 24, 2024.
*/
#include "Meap_classes.h"

#include "soc/soc_caps.h"

#include <soc/sens_reg.h>

#include "driver/touch_sensor.h"
#include "esp32-hal-touch.h"
#include "esp32-hal-periman.h"

#include "driver/i2s_std.h"

extern i2s_chan_handle_t tx_handle;
extern i2s_chan_handle_t rx_handle;

Meap::Meap()
{
  // Constructor
}

void Meap::begin()
{
  pinMode(MEAP_MUX_CONTROL_PIN_A, OUTPUT); // mux a
  pinMode(MEAP_MUX_CONTROL_PIN_B, OUTPUT); // mux b
  pinMode(MEAP_MUX_CONTROL_PIN_C, OUTPUT); // mux c

  digitalWrite(MEAP_MUX_CONTROL_PIN_A, LOW);
  digitalWrite(MEAP_MUX_CONTROL_PIN_B, LOW);
  digitalWrite(MEAP_MUX_CONTROL_PIN_C, LOW);

  pinMode(MEAP_MUX_DIP_PIN, INPUT); // dip mux input

  pinMode(MEAP_LED_0_PIN, OUTPUT);
  pinMode(MEAP_LED_1_PIN, OUTPUT);

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
  analogRead(8); // rlly hacky, just let the API get ADC1 set up bc it seems like i was missing something

  // set up ADC2
  // REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_FORCE, 1); // adc controlled by software
  // REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD_FORCE, 1); // channel select controlled by software
  analogRead(11); // rlly hacky, just let the API get ADC2 set up bc it seems like i was missing something!

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
  touchSetCycles(1, 1);

  pinMode(18, INPUT); // for external multiplexer

#ifndef MEAP_LEGACY
  codecInit();
#endif
}

long Meap::irand(long howsmall, long howbig)
{ // generates a random integer between howsmall and howbig (inclusive of both numbers)
  howbig++;
  if (howsmall >= howbig)
  {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return (xorshift96() % diff) + howsmall;
}

float Meap::frand() // generates a random float between 0 and 1 with 4 decimals of precision
{
  return (xorshift96() % 10000 / 10000.f);
}

void Meap::readInputsFast()
{
  uint64_t curr_time = millis();

  // HANDLING DIP AND AUX MUX
  if (curr_time > aux_mux_ready_time)
  {
    // ******** DIP INPUTS ********
    dip_vals[dip_pins[aux_mux_read_channel]] = !digitalRead(MEAP_MUX_DIP_PIN); // read the DIP mux common pin

    // check if dip switch changed states
    if (dip_vals[dip_pins[aux_mux_read_channel]] != prev_dip_vals[dip_pins[aux_mux_read_channel]])
    {
      updateDip(dip_pins[aux_mux_read_channel], dip_vals[dip_pins[aux_mux_read_channel]]);
      prev_dip_vals[dip_pins[aux_mux_read_channel]] = dip_vals[dip_pins[aux_mux_read_channel]];
    }

    // ******** AUX MUX INPUTS ********
    if (REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DONE_SAR))
    {
      aux_mux_vals[aux_mux_read_channel] = REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DATA_SAR);
    }

    // move to next channel
    aux_mux_read_channel = (aux_mux_read_channel + 1) % 8;
    setMuxChannel(aux_mux_read_channel);

    // start an adc conversion for aux mux
    REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD, 1);     // select channel. no bitshift for GPIO11 ADC2 Channel 0
    REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_SAR, 0); // reset conversion register
    REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_SAR, 1); // start a conversion

    // update timer
    aux_mux_ready_time = millis() + mux_propogation_delay;
  }

  // reading built-in pot conversions
  if (adc1_mode == kWAITING && REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DONE_SAR))
  {
    pot_vals[pot_read_num] = REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DATA_SAR);
    pot_read_num = (pot_read_num + 1) % 2;
    adc1_mode = kREADY;
  }

  // starting built-in pot conversions
  if (adc1_mode == kREADY)
  {
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD, 1 << pot_pins[pot_read_num]); // select channel
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 0);                       // reset conversion register
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 1);                       // start a conversion

    // pot_update_time = curr_time + pot_update_delay;
    adc1_mode = kWAITING;
  }

  // Starting touch conversion
  if (curr_time > touch_update_time)
  {
    if (touch_mode == kREADY)
    {
      REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << touch_pins[touch_read_channel]);

      REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0); // clear reading start register
      REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 1); // begin a reading

      touch_update_time = curr_time + touch_update_delay;
      touch_mode = kWAITING;
    }
  }

  int64_t temp_reading;

  switch (touch_read_channel)
  {
  case 0:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS2_REG, SENS_TOUCH_PAD2_DATA);
    break;
  case 1:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS4_REG, SENS_TOUCH_PAD4_DATA);
    break;
  case 2:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS6_REG, SENS_TOUCH_PAD6_DATA);
    break;
  case 3:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS8_REG, SENS_TOUCH_PAD8_DATA);
    break;
  case 4:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS1_REG, SENS_TOUCH_PAD2_DATA);
    break;
  case 5:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS3_REG, SENS_TOUCH_PAD3_DATA);
    break;
  case 6:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS5_REG, SENS_TOUCH_PAD5_DATA);
    break;
  case 7:
    temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS7_REG, SENS_TOUCH_PAD7_DATA);
    break;
  }

  // reading touch conversions
  if (touch_mode == kWAITING && (REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE) || temp_reading < 2000))
  {
    switch (touch_read_channel)
    {
    case 0:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS2_REG, SENS_TOUCH_PAD2_DATA);
      break;
    case 1:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS4_REG, SENS_TOUCH_PAD4_DATA);
      break;
    case 2:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS6_REG, SENS_TOUCH_PAD6_DATA);
      break;
    case 3:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS8_REG, SENS_TOUCH_PAD8_DATA);
      break;
    case 4:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS1_REG, SENS_TOUCH_PAD2_DATA);
      break;
    case 5:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS3_REG, SENS_TOUCH_PAD3_DATA);
      break;
    case 6:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS5_REG, SENS_TOUCH_PAD5_DATA);
      break;
    case 7:
      touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS7_REG, SENS_TOUCH_PAD7_DATA);
      break;
    }

    if (touch_avgs[touch_read_channel] > touch_threshold)
    {
      touch_vals[touch_read_channel] = 1;
    }
    else
    {
      touch_vals[touch_read_channel] = 0;
    }
    if (touch_vals[touch_read_channel] != prev_touch_vals[touch_read_channel])
    {
      updateTouch(touch_read_channel, touch_vals[touch_read_channel]);
      prev_touch_vals[touch_read_channel] = touch_vals[touch_read_channel];
    }

    touch_read_channel = (touch_read_channel + 1) % 8;
    touch_mode = kREADY;
  }
}

// Reads DIP inputs, touch pads and potentiometers using blocking functions. Not the most efficient way to read them but it works.
// takes ~133us eek
// total touchreads take ~20us
// three analogreads at end take 100us total
void Meap::readInputs()
{

  // read dips and buttons
  for (int i = 8; --i >= 0;)
  {
    setMuxChannel(i); // repeated several times to allow address signals to propogate through multiplexers
    touch_avgs[i] = touchRead(touch_pins[i]);

    // process the touch pads
    touch_vals[i] = touch_avgs[i] > touch_threshold;
    if (touch_vals[i] != prev_touch_vals[i])
    {
      updateTouch(i, touch_vals[i]);
      prev_touch_vals[i] = touch_vals[i];
    }

    dip_vals[dip_pins[i]] = !digitalRead(MEAP_MUX_DIP_PIN); // read the DIP mux common pin

    // was a dip pressed?
    if (dip_vals[dip_pins[i]] != prev_dip_vals[dip_pins[i]])
    {
      updateDip(dip_pins[i], dip_vals[dip_pins[i]]);
      prev_dip_vals[dip_pins[i]] = dip_vals[dip_pins[i]];
    }
  }

  // read builtin pots
  pot_vals[0] = (pot_vals[0] * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_POT_0_PIN));
  pot_vals[1] = (pot_vals[1] * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_POT_1_PIN));
  volume_val = (volume_val * meap_alpha) + (meap_one_minus_alpha * analogRead(MEAP_VOLUME_POT_PIN));

  // pot_vals[0] = analogRead(MEAP_POT_0_PIN);
  // pot_vals[1] = analogRead(MEAP_POT_1_PIN);
  // volume_val = analogRead(MEAP_VOLUME_POT_PIN);
}

StereoSample Meap::pan2(int64_t sample, uint8_t pos)
{
  StereoSample my_sample;
  my_sample.l = (m_sin1024_uint_DATA[255 - pos] * sample) >> 8;
  my_sample.r = (m_sin1024_uint_DATA[pos] * sample) >> 8;
  return my_sample;
}

float Meap::tempoToPeriod(float tempo)
{
  return 60000.f / tempo;
}

float Meap::midiPulseMicros(float tempo)
{
  return (2500000.f / tempo); // 2500 = 60000ms/24pulses
}

void Meap::setMuxChannel(int8_t channel)
{
  digitalWrite(MEAP_MUX_CONTROL_PIN_A, bitRead(channel, 0));
  digitalWrite(MEAP_MUX_CONTROL_PIN_B, bitRead(channel, 1));
  digitalWrite(MEAP_MUX_CONTROL_PIN_C, bitRead(channel, 2));
};

void Meap::writeLED(uint8_t led_num, uint8_t led_level)
{
  digitalWrite(led_pins[led_num], led_level);
}

#ifndef MEAP_LEGACY
uint32_t Meap::SGread(uint32_t reg)
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

bool Meap::SGwrite(uint32_t reg, uint32_t val)
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

uint32_t Meap::SGmodify(uint32_t reg, uint32_t val, uint32_t iMask)
{
  uint32_t val1 = (SGread(reg) & (~iMask)) | val;
  if (!SGwrite(reg, val1))
    return 0;
  return val1;
}

bool Meap::sgtlInit()
{
  delay(5);
  // Serial.print("chip ID = ");
  // delay(5);
  // unsigned int n = SGread(CHIP_ID);
  // Serial.println(n, HEX);
  int r = SGwrite(CHIP_ANA_POWER, 0x4060); // VDDD is externally driven with 1.8V
  if (!r)
    return false;
  SGwrite(CHIP_LINREG_CTRL, 0x006C);   // VDDA & VDDIO both over 3.1V
  SGwrite(CHIP_REF_CTRL, 0x01F2);      // VAG=1.575, normal ramp, +12.5% bias current
  SGwrite(CHIP_LINE_OUT_CTRL, 0x0F22); // LO_VAGCNTRL=1.65V, OUT_CURRENT=0.54mA
  SGwrite(CHIP_SHORT_CTRL, 0x4446);    // allow up to 125mA
  SGwrite(CHIP_ANA_CTRL, 0x0137);      // enable zero cross detectors

  int extMCLK = 0; // SETTING THIS MANUALLY RIGHT NOW TO BYPASS THIS IF STATEMENT BC WE ARE DOING ESP32 CLOCK!

  SGwrite(CHIP_ANA_POWER, 0x40FF); // power up: lineout, hp, adc, dac

  SGwrite(CHIP_DIG_POWER, 0x0073); // power up all digital stuff
  delay(400);
  SGwrite(CHIP_LINE_OUT_VOL, 0x1F1F); // default approx 1.3 volts peak-to-peak
  // SGwrite(CHIP_LINE_OUT_VOL, 0x1414); // default approx 1.3 volts peak-to-peak

  SGwrite(CHIP_CLK_CTRL, 0x0000); // 32 kHz, 256*Fs
  SGwrite(CHIP_I2S_CTRL, 0x0030); // SCLK=64*Fs, 16bit, I2S format

  // default signal routing is ok?
  SGwrite(CHIP_SSS_CTRL, 0x0010);    // ADC->I2S, I2S->DAC
  SGwrite(CHIP_ADCDAC_CTRL, 0x0000); // disable dac mute
  SGwrite(CHIP_DAC_VOL, 0x3C3C);     // digital gain, 0dB

  // SGwrite(CHIP_ANA_HP_CTRL, 0x0000); // set output volume to +12dB
  SGwrite(CHIP_ANA_HP_CTRL, 0x1818); // set output volume to 0dB
  SGwrite(CHIP_ANA_CTRL, 0x0026);    // enable zero cross detectors -- was 36 but i think that mutes headphones??

  return true;
}

void Meap::setCodecGain(uint16_t volume)
{
  uint16_t register_volume = (volume << 8) + volume;
  SGwrite(CHIP_DAC_VOL, register_volume); // digital gain, 0dB
}

void Meap::codecInit()
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

  /* Set the configurations for BOTH TWO channels, since TX and RX channel have to be same in full-duplex mode */
  i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION), // 32000 sample rate... eek i want 32768 orrr do i want 32768*8
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
      .gpio_cfg = {
          .mclk = GPIO_NUM_40,
          .bclk = GPIO_NUM_35,
          .ws = GPIO_NUM_39,
          .dout = GPIO_NUM_47,
          .din = GPIO_NUM_48,
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
  sgtlInit();                                     // initialize codec
}

#endif