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

Meap::Meap()
{
  // Constructor
}

void Meap::begin()
{
  pinMode(38, OUTPUT); // mux a
  pinMode(45, OUTPUT); // mux b
  pinMode(46, OUTPUT); // mux c

  digitalWrite(38, LOW);
  digitalWrite(45, LOW);
  digitalWrite(46, LOW);

  pinMode(14, INPUT); // dip mux input

  // touchSetCycles(1, 1); // sets up touch pads

  pinMode(36, OUTPUT); // LED E
  pinMode(37, OUTPUT); // LED D

  // set up ADC1
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_FORCE, 1); // adc controlled by software
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD_FORCE, 1); // channel select controlled by software
  REG_WRITE(SENS_SAR_ATTEN1_REG, 0xFFFFFFFF);                         // set attenuation
  analogRead(8);

  // set up ADC2
  REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_FORCE, 1); // adc controlled by software
  REG_SET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD_FORCE, 1); // channel select controlled by software
  // REG_WRITE(SENS_SAR_ATTEN2_REG, 0x00000000);                         // set attenuation
  analogRead(11); // rlly hacky, just let the API get ADC2 set up bc it seems like i was missing something!

  // set up touch FSM
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_FORCE, 1); // software will start readings
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0);    // clear reading start register
  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_DATA_SEL, 0);         // raw data (no smoothing or benchmark)
  touchRead(1);                                                           // letting the API finish setup for me again, hey it works!
  touchRead(2);
  touchRead(3);
  touchRead(4);
  touchRead(5);
  touchRead(6);
  touchRead(7);
  touchRead(8);
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL1_REG, RTC_CNTL_TOUCH_MEAS_NUM, 1); // only one reading
  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << 1);    // choose channel 1 to begin
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

void Meap::readPots()
{
  pot_vals[0] = analogRead(pot_pins[0]);
  pot_vals[1] = analogRead(pot_pins[1]);
}

void Meap::readTouch()
{
  int pin_val = 0;
  for (int i = 0; i < 8; i++)
  {
    pin_val = touchRead(touch_pins[i]);
    touch_avgs[i] = 0.6 * touch_avgs[i] + 0.4 * pin_val;
    if (touch_avgs[i] > touch_threshold)
    {
      touch_vals[i] = 1;
    }
    else
    {
      touch_vals[i] = 0;
    }
    if (touch_vals[i] != prev_touch_vals[i])
    {
      updateTouch(i, touch_vals[i]);
    }
    prev_touch_vals[i] = touch_vals[i];
  }
}

void Meap::readDip()
{
  // Read DIP values using mux
  for (int8_t i = 0; i < 8; i++)
  {
    setMuxChannel(38, 45, 46, i);
    dip_vals[dip_pins[i]] = !digitalRead(14);
  }

  // Check if any switches changed position
  for (int8_t i = 0; i < 8; i++)
  {
    if (dip_vals[i] != prev_dip_vals[i])
    {
      updateDip(i, dip_vals[i]);
    }
    prev_dip_vals[i] = dip_vals[i]; // update prevDipVals array
  }
}

void Meap::readAuxMux()
{

  if (millis() > aux_mux_ready_time)
  {
    aux_mux_vals[aux_mux_read_channel] = analogRead(11);
    aux_mux_read_channel = (aux_mux_read_channel + 1) % 8;
    setMuxChannel(38, 45, 46, aux_mux_read_channel);
    aux_mux_ready_time = millis() + mux_propogation_delay;
  }
}

void Meap::readInputs()
{
  uint64_t curr_time = millis();

  // HANDLING DIP AND AUX MUX
  if (curr_time > aux_mux_ready_time)
  {
    // ******** DIP INPUTS ********
    dip_vals[dip_pins[aux_mux_read_channel]] = !digitalRead(dip_common_pin); // read the DIP mux common pin

    // check if dip switch changed states
    if (dip_vals[dip_pins[aux_mux_read_channel]] != prev_dip_vals[dip_pins[aux_mux_read_channel]])
    {
      updateDip(dip_pins[aux_mux_read_channel], dip_vals[dip_pins[aux_mux_read_channel]]);
      prev_dip_vals[dip_pins[aux_mux_read_channel]] = dip_vals[dip_pins[aux_mux_read_channel]];
    }

    // ******** AUX MUX INPUTS ********
    if (REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DONE_SAR))
    {
      aux_mux_vals[aux_mux_read_channel] = REG_GET_FIELD(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_DATA_SAR); // reading this raw without checking if its ready,, whooooo!
    }
    else
    {
      aux_mux_vals[aux_mux_read_channel] = 69;
    }

    // move to next channel
    aux_mux_read_channel = (aux_mux_read_channel + 1) % 8;
    setMuxChannel(38, 45, 46, aux_mux_read_channel);

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
  // if (curr_time > pot_update_time)
  // {
  if (adc1_mode == kREADY)
  {
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD, 1 << pot_pins[pot_read_num]); // select channel
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 0);                       // reset conversion register
    REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 1);                       // start a conversion

    // pot_update_time = curr_time + pot_update_delay;
    adc1_mode = kWAITING;
  }
  // }

  // Starting touch conversion
  if (curr_time > touch_update_time)
  {
    if (touch_mode == kREADY)
    {
      REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << touch_pins[touch_read_channel]);
      // REG_SET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_CHANNEL_CLR, 1); // don't know if i need to clear here

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

    // REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 0); // dont think i need this bc i set whole register before reading

    touch_read_channel = (touch_read_channel + 1) % 8;
    touch_mode = kREADY;
  }
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

void Meap::setMuxChannel(int8_t a, int8_t b, int8_t c, int8_t channel)
{
  digitalWrite(a, bitRead(channel, 0));
  digitalWrite(b, bitRead(channel, 1));
  digitalWrite(c, bitRead(channel, 2));
};

void Meap::turnLedDOn()
{
  digitalWrite(36, HIGH);
}

void Meap::turnLedDOff()
{
  digitalWrite(36, LOW);
}

void Meap::mHandleTouch()
{
  uint64_t curr_time = millis();

  // start next conversion
  if (curr_time > touch_update_time)
  {
    if (touch_mode == kREADY)
    {
      startTouchConversion(touch_read_channel);
      touch_update_time = curr_time + touch_update_delay;
      touch_mode = kWAITING;
    }
  }

  // read the result if its ready
  if (touch_mode == kWAITING && mTouchReady())
  {
    touch_vals[touch_read_channel] = mTouchResult();

    touch_avgs[touch_read_channel] = 0.6 * touch_avgs[touch_read_channel] + 0.4 * touch_vals[touch_read_channel];
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
    }
    prev_touch_vals[touch_read_channel] = touch_vals[touch_read_channel];

    touch_read_channel = (touch_read_channel + 1) % 8;
    touch_mode = kREADY;
  }
}

bool Meap::mTouchReady()
{
}

uint16_t Meap::mTouchResult()
{
}

void Meap::mSetCycles(uint16_t measure, uint16_t sleep)
{
}

void Meap::mTouchInit()
{
}

uint16_t Meap::startTouchConversion(uint8_t pin)
{
}

void mTouchChannelInit(int pad)
{
}

long irand(long howsmall, long howbig)
{ // generates a random integer between howsmall and howbig (inclusive of both numbers)
  howbig++;
  if (howsmall >= howbig)
  {
    return howsmall;
  }
  long diff = howbig - howsmall;

  return (xorshift96() % diff) + howsmall;
}

float frand() // generates a random float between 0 and 1 with 4 decimals of precision
{
  return (xorshift96() % 10000 / 10000.f);
}
