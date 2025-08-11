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

#include "esp_random.h"
#include "bootloader_random.h"

// #include <Meap.h>

bool Meap::cap1280_touch_flag;

extern i2s_chan_handle_t tx_handle;
extern i2s_chan_handle_t rx_handle;

Meap::Meap()
{
  // Constructor
}

void Meap::begin()
{
  Meap::begin(mMEAP4C);
}

void Meap::begin(meap_hardware_versions hardware_version_)
{
  hardware_version = hardware_version_;
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
    cap1280_write_reg(0x1F, 0b00001111); // set sensitivity (default 0 010 1111)
    cap1280_write_reg(0x44, 0b01000000); // set interrupt on release
    cap1280_write_reg(0x28, 0b00000000); // disable retriggering
    cap1280_write_reg(0x27, 0b11111111); // enable all interrupts
    cap1280_write_reg(0x26, 0b11111111); // force calibration routine
    cap1280_write_reg(0x2A, 0b00001100); // turn off MTB

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

  pinMode(18, INPUT); // for external multiplexer

  codecInit();

  bootloader_random_enable();
  randomSeed(esp_random()); // initializes random number generator
  bootloader_random_disable();
  xorshiftSeed((long)random(1000));
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

float Meap::frand() // generates a random float between 0 and 1
{
  return ((float)xorshift96() / 0xFFFFFFFF);
}

// void Meap::readInputsFast()
// {
//   uint64_t curr_time = millis();

//   // HANDLING DIP AND AUX MUX
//   if (curr_time > aux_mux_ready_time)
//   {
//     // ******** DIP INPUTS ********
//     dip_vals[dip_pins[aux_mux_read_channel]] = !digitalRead(MEAP_MUX_DIP_PIN); // read the DIP mux common pin

//     // check if dip switch changed states
//     if (dip_vals[dip_pins[aux_mux_read_channel]] != prev_dip_vals[dip_pins[aux_mux_read_channel]])
//     {
//       updateDip(dip_pins[aux_mux_read_channel], dip_vals[dip_pins[aux_mux_read_channel]]);
//       prev_dip_vals[dip_pins[aux_mux_read_channel]] = dip_vals[dip_pins[aux_mux_read_channel]];
//     }

//     // ******** AUX MUX INPUTS ********
//     if (REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DONE_SAR))
//     {
//       aux_mux_vals[aux_mux_read_channel] = REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DATA_SAR);
//     }

//     // move to next channel
//     aux_mux_read_channel = (aux_mux_read_channel + 1) % 8;
//     setMuxChannel(aux_mux_read_channel);

//     // start an adc conversion for aux mux
//     REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD, 1);     // select channel. no bitshift for GPIO11 ADC2 Channel 0
//     REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_SAR, 0); // reset conversion register
//     REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_SAR, 1); // start a conversion

//     // update timer
//     aux_mux_ready_time = millis() + mux_propogation_delay;
//   }

//   // reading built-in pot conversions
//   if (adc1_mode == kWAITING && REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DONE_SAR))
//   {
//     pot_vals[pot_read_num] = REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DATA_SAR);
//     pot_read_num = (pot_read_num + 1) % 2;
//     adc1_mode = kREADY;
//   }

//   // starting built-in pot conversions
//   if (adc1_mode == kREADY)
//   {
//     REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD, 1 << pot_pins[pot_read_num]); // select channel
//     REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 0);                       // reset conversion register
//     REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 1);                       // start a conversion

//     // pot_update_time = curr_time + pot_update_delay;
//     adc1_mode = kWAITING;
//   }

//   // Starting touch conversion
//   if (curr_time > touch_update_time)
//   {
//     if (touch_mode == kREADY)
//     {
//       REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << touch_pins[touch_read_channel]);

//       REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0); // clear reading start register
//       REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 1); // begin a reading

//       touch_update_time = curr_time + touch_update_delay;
//       touch_mode = kWAITING;
//     }
//   }

//   int64_t temp_reading;

//   switch (touch_read_channel)
//   {
//   case 0:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS2_REG, SENS_TOUCH_PAD2_DATA);
//     break;
//   case 1:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS4_REG, SENS_TOUCH_PAD4_DATA);
//     break;
//   case 2:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS6_REG, SENS_TOUCH_PAD6_DATA);
//     break;
//   case 3:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS8_REG, SENS_TOUCH_PAD8_DATA);
//     break;
//   case 4:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS1_REG, SENS_TOUCH_PAD2_DATA);
//     break;
//   case 5:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS3_REG, SENS_TOUCH_PAD3_DATA);
//     break;
//   case 6:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS5_REG, SENS_TOUCH_PAD5_DATA);
//     break;
//   case 7:
//     temp_reading = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS7_REG, SENS_TOUCH_PAD7_DATA);
//     break;
//   }

//   // reading touch conversions
//   if (touch_mode == kWAITING && (REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE) || temp_reading < 2000))
//   {
//     switch (touch_read_channel)
//     {
//     case 0:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS2_REG, SENS_TOUCH_PAD2_DATA);
//       break;
//     case 1:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS4_REG, SENS_TOUCH_PAD4_DATA);
//       break;
//     case 2:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS6_REG, SENS_TOUCH_PAD6_DATA);
//       break;
//     case 3:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS8_REG, SENS_TOUCH_PAD8_DATA);
//       break;
//     case 4:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS1_REG, SENS_TOUCH_PAD2_DATA);
//       break;
//     case 5:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS3_REG, SENS_TOUCH_PAD3_DATA);
//       break;
//     case 6:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS5_REG, SENS_TOUCH_PAD5_DATA);
//       break;
//     case 7:
//       touch_avgs[touch_read_channel] = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS7_REG, SENS_TOUCH_PAD7_DATA);
//       break;
//     }

//     if (touch_avgs[touch_read_channel] > touch_threshold)
//     {
//       touch_vals[touch_read_channel] = 1;
//     }
//     else
//     {
//       touch_vals[touch_read_channel] = 0;
//     }
//     if (touch_vals[touch_read_channel] != prev_touch_vals[touch_read_channel])
//     {
//       updateTouch(touch_read_channel, touch_vals[touch_read_channel]);
//       prev_touch_vals[touch_read_channel] = touch_vals[touch_read_channel];
//     }

//     touch_read_channel = (touch_read_channel + 1) % 8;
//     touch_mode = kREADY;
//   }
// }

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

uint8_t Meap::cap1280_read_reg(uint8_t reg)
{
  uint8_t val;
  Wire.beginTransmission(0x28);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)0x28, 2);
  val = Wire.read();
  return val;
}

void Meap::cap1280_write_reg(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(0x28);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void Meap::cap1280_clear_interrupt()
{
  cap1280_write_reg(0x00, 0b00000000);
}

void Meap::cap1280_get_touch_data(int data[8])
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

void Meap::cap1280_interrupt()
{
  cap1280_touch_flag = true;
}

StereoSample Meap::pan2(int64_t sample, uint8_t pos)
{
  StereoSample my_sample;
  my_sample.l = (m_power_curve_DATA[255 - pos] * sample) >> 7;
  my_sample.r = (m_power_curve_DATA[pos] * sample) >> 7;
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

void Meap::setCodecGain(uint16_t volume)
{
  uint16_t register_volume = (volume << 8) + volume;
  SGwrite(CHIP_DAC_VOL, register_volume); // digital gain, 0dB
}

void Meap::setCodecInputGain(uint16_t gain)
{
  SGwrite(CHIP_ANA_ADC_CTRL, gain + (gain << 4));
}

void Meap::setCodecMicGain(int gain)
{
  SGwrite(CHIP_MIC_CTRL, gain);
}

void Meap::setCodecInputMic()
{
  SGwrite(CHIP_ANA_CTRL, 0x0022); // enable zero cross detectors / mic input
}

void Meap::setCodecInputLine()
{
  SGwrite(CHIP_ANA_CTRL, 0x0026); // enable zero cross detectors / mic input
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
