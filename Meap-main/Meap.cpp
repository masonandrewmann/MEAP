/*
  Meap.cpp - Library accompanying MEAP boards.
  Created by Mason Mann, January 24, 2024.
*/
#include "Arduino.h"
#include "Meap.h"
#include <mozzi_rand.h>
#include <MozziGuts.h>
#include <Mux.h>
#include <tables/sin1024_int8.h>

Meap::Meap()
{
  // Constructor
}

void Meap::begin()
{
  dip_mux = new Mux(Pin(14, INPUT, PinType::Digital), Pinset(38, 45, 46));
  aux_mux = new Mux(Pin(11, INPUT, PinType::Analog), Pinset(38, 45, 46));
  touchSetCycles(1, 1); // sets up touch pads
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
  pot_vals[0] = mozziAnalogRead(pot_pins[0]);
  pot_vals[1] = mozziAnalogRead(pot_pins[1]);
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
  for (int i = 0; i < 8; i++)
  {
    dip_mux->read(i); // read once and throw away result (for reliability)
    dip_vals[dip_pins[i]] = !dip_mux->read(i);
  }
  // Check if any switches changed position
  for (int i = 0; i < 8; i++)
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
  // Read auxMux values
  for (int i = 0; i < 8; i++)
  {
    aux_mux->read(i); // read once and throw away result (for reliability)
    aux_mux_vals[i] = aux_mux->read(i);
  }
}

StereoSample Meap::pan2(int sample, uint8_t pos)
{
  StereoSample my_sample;
  my_sample.left = ((SIN1024_DATA[255 - pos]) * sample) >> 8;
  my_sample.right = ((SIN1024_DATA[pos]) * sample) >> 8;
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
