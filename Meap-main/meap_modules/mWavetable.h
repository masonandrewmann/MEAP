/*
 * Oscil.h
 *
 * Oscil.h owes much to AF_precision_synthesis.pde, 2009, Adrian Freed.
 *
 * Copyright 2012 Tim Barrass, 2009 Adrian Freed.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef MEAP_WAVETABLE_H
#define MEAP_WAVETABLE_H

// fractional bits for oscillator index precision
#define OSCIL_F_BITS 16
#define OSCIL_F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 15n16 fixed-point number
#define OSCIL_PHMOD_BITS 16

/**
  mWavetable is a slight extension of mOscil to allow for multi-frame wavetables. These wavetables are often used
  in wavetable synth plugins. mWavetable has specifically been written with a process for supporting wavetables converted
  from the free VST plugin Vital https://vital.audio/

  The process is as follows:

    1. Design/choose a wavetable in vital
    2. Export as a .wav file. (From the main vital window, click the pencil symbol next to your wavetable. In the wavetable window,
    choose "Export as a .wav File" from the hamburger menu button)
    3. Convert to a compatible format using the vital tool here: https://ogreto.es/meapscripts
    4. Move to sketch folder and import

  The wav file exported from vital consists of 256 frames of 2048 samples each. In the vital tool from step #3, you are able to decrease the frame size or
  specify the step size which will only keep every nth frame. Both of these options allow you to decrease the file size at the expense of some fidelity.
  Leaving them at 2048 and 1 respectively will preserve full wavetable fidelity.

  Beyond that, mWavetable functions exactly as mOscil, with the exception of the setFrame function. This allows you to choose which of the wavetable frames
  will be read. It's really just a quick and convenient way of swapping between tables as you might do manually with mOscil. By default, these vital wavetables
  have 256 frames so you can easily connect the output of an 8-bit oscillator or envelope to the setFrame function to sweep through frames.
*/
// template <uint16_t FRAME_SIZE, uint16_t UPDATE_RATE, bool DITHER_PHASE=false>
template <uint32_t FRAME_SIZE, uint32_t NUM_FRAMES, uint16_t UPDATE_RATE>
class mWavetable
{

public:
  /** Constructor.
    @param TABLE_NAME the name of the array the Oscil will be using. This
    can be found in the table ".h" file if you are using a table made for
    Mozzi by the int8_t2mozzi.py python script in Mozzi's python
    folder.*/
  mWavetable(const int16_t *TABLE_NAME)
      : table(TABLE_NAME)
  {
  }

  /** Constructor.
    Declare an Oscil with template TABLE_NUM_CELLS and UPDATE_RATE
    parameters, without specifying a particular wave table for it to play.
    The table can be set or changed on the fly with setTable(). Any tables
    used by the Oscil must be the same size.
    */
  mWavetable()
  {
  }

  /** Updates the phase according to the current frequency and returns the sample at the new phase position.
    @return the next sample.
    */
  inline int16_t next()
  {

    incrementPhase();

    uint64_t index = frame_offset + ((phase_fractional >> OSCIL_F_BITS) & (FRAME_SIZE - 1));

    int64_t out = FLASH_OR_RAM_READ<const int16_t>(table + index);
    int64_t difference = FLASH_OR_RAM_READ<const int16_t>((table + 1) + index) - out;

    int64_t fractional = phase_fractional & 65535; // remove integer component leaving only fraction, maybe i could just cast to uint16_t

    int64_t diff_fraction = (difference * fractional) >> 16;
    out += diff_fraction;

    return out;
  }

  /** Sets frame of wavetable.
   */
  inline void setFrame(uint16_t frame_num)
  {
    if (frame_num >= NUM_FRAMES)
    {
      frame_num = NUM_FRAMES - 1;
    }
    frame_offset = FRAME_SIZE * frame_num;
  }

  /** Change the sound table which will be played by the Oscil.
    @param TABLE_NAME is the name of the array in the table ".h" file you're using.
    */
  void setTable(const int16_t *TABLE_NAME)
  {
    table = TABLE_NAME;
  }

  /** Set the phase of the Oscil.  This does the same thing as Sample::start(offset).  Just different ways of thinking about oscillators and samples.
    @param phase a position in the wavetable.
    */
  // This could be called in the control interrupt, so phase_fractional should really be volatile,
  // but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
  // (in loop()), it's probably worth keeping it nonvolatile until it causes problems
  void setPhase(uint64_t phase)
  {
    phase_fractional = (uint64_t)phase << OSCIL_F_BITS;
  }

  /** Set the phase of the Oscil.  Might be useful with getPhaseFractional().
    @param phase a position in the wavetable.
    */
  // This could be called in the control interrupt, so phase_fractional should really be volatile,
  // but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
  // (in loop()), it's probably worth keeping it nonvolatile until it causes problems
  void setPhaseFractional(uint64_t phase)
  {
    phase_fractional = phase;
  }

  /** Get the phase of the Oscil in fractional format.
    @return position in the wavetable, shifted left by OSCIL_F_BITS (which is 16 when this was written).
    */
  uint64_t getPhaseFractional()
  {
    return phase_fractional;
  }

  /** Set the oscillator frequency with an uint16_t. This is faster than using a
    float, so it's useful when processor time is tight, but it can be tricky with
    low and high frequencies, depending on the size of the wavetable being used. If
    you're not getting the results you expect, try explicitly using a float, or try
    setFreq_Q24n8() or or setFreq_Q16n16().
    @param frequency to play the wave table.
    */
  inline void setFreq(int frequency)
  {
    // TB2014-8-20 change this following Austin Grossman's suggestion on user list
    // https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mozzi-users/u4D5NMzVnQs/pCmiWInFvrkJ
    // phase_increment_fractional = ((((uint32_t)FRAME_SIZE<<ADJUST_FOR_FRAME_SIZE)*frequency)/UPDATE_RATE) << (OSCIL_F_BITS - ADJUST_FOR_FRAME_SIZE);
    // to this:
    phase_increment_fractional = ((uint64_t)frequency) * ((OSCIL_F_BITS_AS_MULTIPLIER * FRAME_SIZE) / UPDATE_RATE);
  }

  /** Set the oscillator frequency with an uint16_t. This is faster than using a
    float, so it's useful when processor time is tight, but it can be tricky with
    low and high frequencies, depending on the size of the wavetable being used. If
    you're not getting the results you expect, try explicitly using a float, or try
    setFreq_Q24n8() or or setFreq_Q16n16().
    @param frequency to play the wave table.
    */
  inline void setFreq(long int frequency)
  {
    // TB2014-8-20 change this following Austin Grossman's suggestion on user list
    // https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mozzi-users/u4D5NMzVnQs/pCmiWInFvrkJ
    // phase_increment_fractional = ((((uint32_t)FRAME_SIZE<<ADJUST_FOR_FRAME_SIZE)*frequency)/UPDATE_RATE) << (OSCIL_F_BITS - ADJUST_FOR_FRAME_SIZE);
    // to this:
    phase_increment_fractional = ((uint64_t)frequency) * ((OSCIL_F_BITS_AS_MULTIPLIER * FRAME_SIZE) / UPDATE_RATE);
  }

  /** Set the oscillator frequency with a float. Using a float is the most reliable
    way to set frequencies, -Might- be slower than using an int but you need either
    this, setFreq_Q24n8() or setFreq_Q16n16() for fractional frequencies.
    @param frequency to play the wave table.
     */
  inline void setFreq(float frequency)
  { // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
    phase_increment_fractional = (uint64_t)((((float)FRAME_SIZE * frequency) / UPDATE_RATE) * OSCIL_F_BITS_AS_MULTIPLIER);
  }

  /** Set the frequency using Q24n8 fixed-point number format.
    This might be faster than the float version for setting low frequencies such as
    1.5 Hz, or other values which may not work well with your table size. A Q24n8
    representation of 1.5 is 384 (ie. 1.5 * 256). Can't be used with UPDATE_RATE
    less than 64 Hz.
    @param frequency in Q24n8 fixed-point number format.
    */
  inline void setFreq_Q24n8(Q24n8 frequency)
  {
    if ((256UL * FRAME_SIZE) >= UPDATE_RATE)
    {
      phase_increment_fractional = ((uint64_t)frequency) * ((256UL * FRAME_SIZE) / UPDATE_RATE);
    }
    else
    {
      phase_increment_fractional = ((uint64_t)frequency) / (UPDATE_RATE / (256UL * FRAME_SIZE));
    }
  }

  /** Set the frequency using Q16n16 fixed-point number format. This is useful in
    combination with Q16n16_mtof(), a fast alternative to mtof(), using Q16n16
    fixed-point format instead of floats.
    @note This should work OK with tables 2048 cells or smaller and
    frequencies up to 4096 Hz.  Can't be used with UPDATE_RATE less than 64 Hz.
    @note This didn't run faster than float last time it was tested, after 2014 code changes.  Need to see if 2014 changes improved or worsened performance.
    @param frequency in Q16n16 fixed-point number format.
    */
  inline void setFreq_Q16n16(Q16n16 frequency)
  {
    if (FRAME_SIZE >= UPDATE_RATE)
    {
      phase_increment_fractional = ((uint64_t)frequency) * (FRAME_SIZE / UPDATE_RATE);
    }
    else
    {
      phase_increment_fractional = ((uint64_t)frequency) / (UPDATE_RATE / FRAME_SIZE);
    }
  }

  /** Set a specific phase increment.  See phaseIncFromFreq().
    @param phaseinc_fractional a phase increment value as calculated by phaseIncFromFreq().
     */
  inline void setPhaseInc(uint64_t phaseinc_fractional)
  {
    phase_increment_fractional = phaseinc_fractional;
  }

private:
  /** Used for shift arithmetic in setFreq() and its variations.
   */
  static const uint16_t ADJUST_FOR_FRAME_SIZE = (FRAME_SIZE < 2048) ? 8 : 0;

  /** Increments the phase of the oscillator without returning a sample.
   */
  inline void incrementPhase()
  {
    // phase_fractional += (phase_increment_fractional | 1); // odd phase incr, attempt to reduce frequency spurs in output
    phase_fractional += phase_increment_fractional;
  }

  /** Returns the current sample.
   */
  inline int16_t readTable()
  {
    return FLASH_OR_RAM_READ<const int16_t>(table + frame_offset + ((phase_fractional >> OSCIL_F_BITS) & (FRAME_SIZE - 1)));
  }

  uint64_t phase_fractional;
  uint64_t phase_increment_fractional;
  uint64_t startpos_fractional, endpos_fractional;
  uint64_t frame_offset;
  const int16_t *table;
};

#endif /* MEAP_WAVETABLE_H */
